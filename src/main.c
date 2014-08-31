/**
 *  Project     StreetArtRobot
 *  @file		main.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Projects main file
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
#include "main.h"
#include "usartl1.h"
#include "irlink.h"
#include "power.h"
#include "oled.h"
#include "servo.h"
#include "motor.h"
#include "switch.h"

/* function prototypes ------------------------------------------------------*/
void SystemClock_Config(void);
char txt[20]; // Temporary memory for strings
int blink = 0;
int mytick = 0;
/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void) {

	int i = 12345;
	// Configure the system clock to 168 Mhz
	SystemClock_Config();

	// Initialize the hardware layer module
	HAL_Init();

	// Enable systick and configure 1ms tick
	HAL_SYSTICK_Config(168000000/ 1000);

	// Initialize the modules
	OLED_Init();
	SERVO_Init();
	MOTOR_Init();
	SWITCH_Init();
/*
	// Initialize the power module
	POWER_Init();

	// Initialize the IR link
	IRLINK_Init();

	// Initialize the debug port
	USARTL2_Init();
*/
	OLED_Clr();
	// Main loop
	 while (1) {

		// Generate a blink flag
		if (mytick > 200) {
			mytick = 0;
			blink = !blink;

			//200ms Task
			//POWER_Task();
		}

		SWITCH_Task();

		if (SWITCH_GetClick(SWITCH_LEFT)) {
			MOTOR_SetVal(MOTOR_M1, 800, 255);
		}
		if (SWITCH_GetClick(SWITCH_RIGHT)) {
			MOTOR_SetVal(MOTOR_M1, 0, 255);
		}
		if (SWITCH_GetClick(SWITCH_UP)) {
			MOTOR_SetVal(MOTOR_M1, -800, 255);
		}
		if (SWITCH_GetClick(SWITCH_DOWN)) {
			MOTOR_SetVal(MOTOR_M1, 2100, 255);
		}
		// Debug ports
		//USARTL1_RxBufferTask();
		//i = TIM8->CCR1;
		sprintf (txt,"%5u", (unsigned int)motorHallPeriode[1]);
		OLED_Print(0, 0, OLED_SIZE_LARGE, txt);
		OLED_Display();
	}

}



