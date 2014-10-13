/**
 *  Project     StreetArtRobot
 *  @file		delay.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		delay funtions
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
/* Includes ------------------------------------------------------------------*/
#include "delay.h"

static __IO uint32_t delay_time1;
static __IO uint32_t delay_time2;

/******************************* Funtions **********************************/

/**
 * @brief  Initializes the module.
 * @param  None
 * @retval None
 */
void DELAY_Init(void) {
	delay_time1 = 0;
	delay_time2 = 0;
}

/**
 * @brief  counts the delay. Call this function every 10ms in interrupt.
 * @param  None
 * @retval None
 */
void DELAY_1msTask(void) {
	if (delay_time1 != 0x00) {
		delay_time1--;
	}

	if (delay_time2 != 0x00) {
		delay_time2--;
	}

}

/**
 * @brief  Delay for n ms.
 * @param  ms delay time in ms
 * @retval None
 */
void DELAY_ms(uint32_t ms) {
}

uint32_t DELAY_GetTime(void) {
	return delay_time2;
}

void DELAY_SetTime(uint32_t ms) {
	delay_time2 = ms;
}

