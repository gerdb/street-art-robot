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
int controller_speed_i[2] = {0,0};
int controller_speed_limit_neg[2] = {0,0};
int controller_speed_limit_pos[2] = {0,0};
int initialized = 0;

/* local functions ----------------------------------------------------------*/



/* global variables ----------------------------------------------------------*/
int controller_speed_enable[2] = {0,0};
int controller_speed_setpoint[2] = {800,-800};
int controller_speed_ki = 32;
int controller_speed_kp = 32;

/**
 * @brief  Configures the speed controller
 * @param  None
 * @retval None
 */
void CONTROLLER_Init(void) {
	initialized = 1;
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

	// Wait until controller is initialized
	if (!initialized)
		return;

	for (i=0; i<2 ; i++) {
		diff = controller_speed_setpoint[i] - MOTOR_GetSpeed(i + MOTOR_M1);

		// I-part with limitation
		if ((diff>0 && !controller_speed_limit_pos[i]) ||
			(diff<0 && !controller_speed_limit_neg[i]))
			controller_speed_i[i] += controller_speed_ki * diff;

		// P and I part
		sum = controller_speed_i[i] / 1024  + (controller_speed_kp * diff) / 32;

		// Limit output
		if (sum > MOTOR_MAX) {
			sum = MOTOR_MAX;
			controller_speed_limit_pos[i] = 1;
		}
		else {
			controller_speed_limit_pos[i] = 0;
		}

		if (sum < -MOTOR_MAX) {
			sum = -MOTOR_MAX;
			controller_speed_limit_neg[i] = 1;
		}
		else {
			controller_speed_limit_neg[i] = 0;
		}

		// Set the motor PWM
		if (controller_speed_enable[i]) {
			MOTOR_SetVal(MOTOR_M1 + i, sum, 255);
		} else {
			MOTOR_SetVal(MOTOR_M1 + i, 0, 255);
			controller_speed_i[i] = 0;
		}

	}

}

