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

/* local variables ----------------------------------------------------------*/
int controller_I[2] = {0,0};
int controller_limit_neg[2] = {0,0};
int controller_limit_pos[2] = {0,0};
int controller_pos[2] = {0,0};
int initialized = 0;


/* local functions ----------------------------------------------------------*/



/* global variables ----------------------------------------------------------*/
int controller_enable[2] = {0,0};
int controller_speed[2] = {10,10};
int controller_ki = 0;
int controller_kp = 4000;

/**
 * @brief  Configures the speed controller
 * @param  None
 * @retval None
 */
void CONTROLLER_Init(void) {
	initialized = 1;
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

	controller_pos[0]+=controller_speed[0];
	controller_pos[1]-=controller_speed[1];

	// Wait until controller is initialized
	if (!initialized)
		return;

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
