/**
 *  Project     StreetArtRobot
 *  @file		view_controller.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Controls the main menu
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

#include "view_controller.h"
#include "switch.h"
#include "rc.h"
#include "controller.h"
#include "gyro.h"
#include "led.h"

/* local variables ----------------------------------------------------------*/

/* local functions ----------------------------------------------------------*/

/* global variables ----------------------------------------------------------*/
en_view_main_menu view_main_menu = VIEW_MAINM_START;

/**
 * @brief  Initializes the module
 * @param  None
 * @retval None
 */
void VIEW_CONTROLLER_Init(void) {
}

/**
 * @brief  Cyclic view task
 * @param  None
 * @retval None
 */
void VIEW_CONTROLLER_Task(void) {
	if (SWITCH_GetClick(SWITCH_LEFT) || RC_GetClick(RC_KEY_GO)) {
		switch (view_main_menu) {

		case VIEW_MAINM_START:
			CONTROLLER_Reset();
			CONTROLLER_Enable(1); // Enable the motor controller
			break;

		case VIEW_MAINM_LED:
			LED_SetVal(LED_ON); // switch on the position light
			break;

		case VIEW_MAINM_GYRO_NULL:
			CONTROLLER_Enable(0); // Disable the motor controller
			GYRO_StartAutoNull(); // Autonull the gyro
			break;


		case VIEW_MAINM_GYRO_OFFSET:
			GYRO_StartOffset();
			break;

		case VIEW_MAINM_GYRO_MANOFF:
			GYRO_ManOffset(+1);
			break;

		default:
			; // do nothing
		}

	}

	if (SWITCH_GetClick(SWITCH_RIGHT) || RC_GetClick(RC_KEY_STOP)) {
		switch (view_main_menu) {

		case VIEW_MAINM_START:
			CONTROLLER_Enable(0); // Disable the motor controller
			break;

		case VIEW_MAINM_LED:
			LED_SetVal(LED_OFF); // switch off the position light
			break;

		case VIEW_MAINM_GYRO_MANOFF:
			GYRO_ManOffset(-1);
			break;

		default:
			; // do nothing
		}

	}


	if (SWITCH_GetClick(SWITCH_UP)) {
		if (view_main_menu != VIEW_MAINM_GYRO_OFFSET)
			view_main_menu++;
		else
			view_main_menu = VIEW_MAINM_START;
	}
	if (SWITCH_GetClick(SWITCH_DOWN) || RC_GetClick(RC_KEY_RED)) {
		if (view_main_menu != VIEW_MAINM_START)
			view_main_menu--;
		else
			view_main_menu = VIEW_MAINM_GYRO_OFFSET;
	}

}
