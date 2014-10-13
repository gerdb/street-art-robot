/**
 *  Project     StreetArtRobot
 *  @file		led.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Controls the position LED
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

#include "led.h"

/* local variables ----------------------------------------------------------*/


/* local functions ----------------------------------------------------------*/


/**
 * @brief  Configures IOs to control the LED
 * @param  None
 * @retval None
 */
void LED_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable the clock for all motor pins
	LED_CLK_ENABLE();

	// Configure the 6 motor pins
	GPIO_InitStruct.Pin = LED_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

	LED_SetVal(LED_OFF);

}

/**
 * @brief  Sets the LED on or off
 * @param  value 1 for on
 * @retval None
 */
void LED_SetVal(int value) {
	if (value == LED_ON)
		HAL_GPIO_WritePin(LED_PORT, LED_PIN, SET);
	else
		HAL_GPIO_WritePin(LED_PORT, LED_PIN, RESET);

}
