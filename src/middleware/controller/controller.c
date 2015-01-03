/**
 *  Project     StreetArtRobot
 *  @file		controller.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Controls the speed of both main motors
 *
 *  @copyright	GPL3
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* Includes -----------------------------------------------------------------*/

#include "controller.h"
#include "motor.h"
#include "gyro.h"

/* local variables ----------------------------------------------------------*/
int controller_I[2] = {0,0};
int controller_limit_neg[2] = {0,0};
int controller_limit_pos[2] = {0,0};

#ifdef CONTROLLER_YAW_PID
int controller_angle_I = 0;
#endif
int controller_angle_limit_neg = 0;
int controller_angle_limit_pos = 0;

int controller_pos[2] = {0,0};
int controller_initialized = 0;
int controller_enable[2] = {0,0};
int controller_speed_rl[2] = {0,0};

/* local functions ----------------------------------------------------------*/



/* global variables ----------------------------------------------------------*/

int controller_ki = 0;
int controller_kp = 4000;

int controller_angle_kp = 4;
#ifdef CONTROLLER_YAW_PID
int controller_angle_ki = 0;
int controller_angle_kd = 0;
#endif

int controller_speed = 10;
int16_t controller_angle = 0;
int controller_yaw_rate = 0;

/**
 * @brief  Configures the speed controller
 * @param  None
 * @retval None
 */
void CONTROLLER_Init(void) {
	controller_initialized = 1;
	CONTROLLER_Reset();
}

/**
 * @brief  Resets the speed controller
 * @param  None
 * @retval None
 */
void CONTROLLER_Reset(void) {
	int i;
	int sensor_pos = 0;

	for (i=0; i<2 ; i++) {

		// Get the direction from the encoder values
		if (i == 0)
			sensor_pos = TIM3->CNT;
		if (i == 1)
			sensor_pos = TIM4->CNT;

		controller_pos[i] = sensor_pos * 16 ;
	}
}

/**
 * @brief  1ms Task
 * @param  None
 * @retval None
 */
void CONTROLLER_1msTask(void) {

	int i;
	int diff;
	int sum;
	int sensor_pos = 0;
	int16_t sensor_ang;

#ifdef CONTROLLER_YAW_PID
	int d;
	int16_t sensor_ang_old=0;
#endif


	// Wait until controller is initialized
	if (!controller_initialized)
		return;

	// Motor: 6000rpm @12V = 100 rps
	// 16 controller_pos steps is one sensor step.
	// Gear ratio is 1:35 and 3:4
	// Diameter of wheels: 150mm
	// Wheel distance: 362mm
	// Sensor steps: 256
	// The quadrature encoder counts only every 2nd edge of the sensor
	//
	// 2048 controller_pos steps are one turn of the motor
	// 95573,33 controller_pos steps are one turn of the wheel
	// 230650,311 controller_pos steps are one turn of the robot
	// 204800 controller_pos steps per second @12V motor voltage

	sensor_ang = GYRO_GetAngle();
	diff = controller_angle - sensor_ang;

#ifdef CONTROLLER_YAW_PID
	d= -(sensor_ang-sensor_ang_old);

	sensor_ang_old = sensor_ang;

	// I-part with limitation
	if ((diff>0 && !controller_angle_limit_pos) ||
		(diff<0 && !controller_angle_limit_neg))
		controller_angle_I += controller_angle_ki * diff;

	// P, I and D part
	sum = controller_angle_I / 16384  + (controller_angle_kp * diff) / 256 + (controller_angle_kd * d) / 256;
#else
	// P part
	sum = (controller_angle_kp * diff) / 256;
#endif

	// Limit yaw rate
	if (sum > CONTROLLER_YAW_MAX) {
		sum = CONTROLLER_YAW_MAX;
		controller_angle_limit_pos = 1;
	}
	else {
		controller_angle_limit_pos = 0;
	}

	if (sum < -CONTROLLER_YAW_MAX) {
		sum = -CONTROLLER_YAW_MAX;
		controller_angle_limit_neg = 1;
	}
	else {
		controller_angle_limit_neg = 0;
	}

	controller_yaw_rate = sum;

	// Mix speed and yaw rate
	controller_speed_rl[0] = controller_speed - controller_yaw_rate;
	controller_speed_rl[1] = controller_speed + controller_yaw_rate;

	// Generate the position ramp for both motors
	controller_pos[0]+=controller_speed_rl[0];
	controller_pos[1]-=controller_speed_rl[1];

	// Speed control of both motors
	for (i=0; i<2 ; i++) {

		// Get the direction from the encoder values
		if (i == 0)
			sensor_pos = TIM3->CNT;
		if (i == 1)
			sensor_pos = TIM4->CNT;

		diff = controller_pos[i]/16 - sensor_pos;
		diff = (int16_t)diff;

		// Limit the difference
		if (diff > CONTROLLER_MAX_DIFF) {
			diff = CONTROLLER_MAX_DIFF;
			controller_pos[i] = (diff + sensor_pos) * 16 ;
		}
		if (diff < -CONTROLLER_MAX_DIFF) {
			diff = -CONTROLLER_MAX_DIFF;
			controller_pos[i] = (diff + sensor_pos) * 16 ;
		}

		// I-part with limitation
		if ((diff>0 && !controller_limit_pos[i]) ||
			(diff<0 && !controller_limit_neg[i]))
			controller_I[i] += controller_ki * diff;

		// P and I part
		sum = controller_I[i] / 1024  + (controller_kp * diff) / 32;

		// Limit output
		if (sum > MOTOR_MAX) {
			sum = MOTOR_MAX;
			controller_limit_pos[i] = 1;
		}
		else {
			controller_limit_pos[i] = 0;
		}

		if (sum < -MOTOR_MAX) {
			sum = -MOTOR_MAX;
			controller_limit_neg[i] = 1;
		}
		else {
			controller_limit_neg[i] = 0;
		}

//		MOTOR_SetVal(MOTOR_M1 + i, MOTOR_MAX / 2, 255);

		// Set the motor PWM
		if (controller_enable[i]) {
			MOTOR_SetVal(MOTOR_M1 + i, sum, 255);
		} else {
			MOTOR_SetVal(MOTOR_M1 + i, 0, 255);
			controller_I[i] = 0;
		}

	}

}

/**
 * @brief  Enabled/ disabled the motor controller
 * @param  enabled true, if controller should be enabled
 * @retval None
 */
void CONTROLLER_Enable(int enabled) {
	int i;
	for (i=0; i<2; i++) {
		controller_enable[i] = enabled;
		MOTOR_SetVal(MOTOR_M1 + i, 0, 255);
	}
}
