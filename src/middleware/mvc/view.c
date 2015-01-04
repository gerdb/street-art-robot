/**
 *  Project     StreetArtRobot
 *  @file		view.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Displays the main menu
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

#include "view.h"
#include "view_controller.h"
#include "switch.h"
#include "rc.h"
#include "oled.h"
#include "printf.h"
#include "gyro.h"


/* local variables ----------------------------------------------------------*/
char txt[20]; // Temporary memory for strings

/* local functions ----------------------------------------------------------*/
en_view_main_menu view_main_menu_old = VIEW_MAINM_START;


/* global variables ----------------------------------------------------------*/

/**
 * @brief  Initializes the module
 * @param  None
 * @retval None
 */
void VIEW_Init(void) {
}

/**
 * @brief  Cyclic view task
 * @param  None
 * @retval None
 */
void VIEW_Task(void) {

	// Clear the display
	if (view_main_menu != view_main_menu_old)
		OLED_Clr();
	view_main_menu_old = view_main_menu;

	switch (view_main_menu) {

	case VIEW_MAINM_START:
		OLED_Print(0, 0, OLED_SIZE_LARGE, "Street");
		OLED_Print(60, 0, OLED_SIZE_SMALL, "Art");
		OLED_Print(60, 1, OLED_SIZE_SMALL, "Robot");
		break;

	case VIEW_MAINM_LED:
		OLED_Print(0, 0, OLED_SIZE_SMALL, "LED");
		OLED_Print(0, 1, OLED_SIZE_SMALL, "on/off");
		break;

	case VIEW_MAINM_GYRO:
		OLED_Print(0, 0, OLED_SIZE_SMALL, "Gyro");
		OLED_Print(0, 1, OLED_SIZE_SMALL, GYRO_GetText(view_main_menu));
		break;

	case VIEW_MAINM_GYRO_NULL:
		OLED_Print(0, 0, OLED_SIZE_SMALL, "Gyro Null");
		OLED_Print(0, 1, OLED_SIZE_SMALL, GYRO_GetText(view_main_menu));
		break;

	case VIEW_MAINM_GYRO_OFFSET:
		OLED_Print(0, 0, OLED_SIZE_SMALL, "Gyro Offset");
		OLED_Print(0, 1, OLED_SIZE_SMALL, GYRO_GetText(view_main_menu));
		break;

	case VIEW_MAINM_GYRO_MANOFF:
		OLED_Print(0, 0, OLED_SIZE_SMALL, "Gyro ManOff");
		OLED_Print(0, 1, OLED_SIZE_SMALL, GYRO_GetText(view_main_menu));
		break;


	}


	OLED_Display();
}
