/**
 *  Project     StreetArtRobot
 *  @file		switch.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Reads the 4 key switch
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

#include "switch.h"

/* local variables ----------------------------------------------------------*/
int switch_click[4]= {0,0,0,0};
int switch_debounced[4]= {0,0,0,0};
int switch_debounceCnt[4]= {0,0,0,0};

/* local functions ----------------------------------------------------------*/


/**
 * @brief  Configures IOs to read the switches
 * @param  None
 * @retval None
 */
void SWITCH_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;


	// Enable the clock for all switch inputs
	SWITCH_CLK_ENABLE();

	// Configure the 4 switch pins
	GPIO_InitStruct.Pin = SWITCH_SW1_PIN | SWITCH_SW2_PIN
						| SWITCH_SW3_PIN | SWITCH_SW4_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(SWITCH_PORT, &GPIO_InitStruct);

}

/**
 * @brief  debounce the switches
 * @param  None
 * @retval None
 */
void SWITCH_1msTask(void) {
	int i;
	for (i=0;i<4;i++) {
		if (HAL_GPIO_ReadPin(SWITCH_PORT, SWITCH_SW1_PIN << i) == GPIO_PIN_SET) {
			if (switch_debounceCnt[i] < SWITCH_DEBOUNCE_TIME) {
				switch_debounceCnt[i] ++;
			}
			else {
				if (switch_debounced[i] != SWITCH_PRESSED) {
					switch_click[i] = SWITCH_CLICK;
				}
				switch_debounced[i]= SWITCH_PRESSED;
			}
		} else {
			if (switch_debounceCnt[i] > 0) {
				switch_debounceCnt[i] --;
			}
			else {
				switch_debounced[i]= SWITCH_RELEASED;
			}
		}
	}
}

/**
 * @brief  Get the state of the switch
 * @param  switch_nr Nr from 0 to 3
 * @retval state of the switch
 */
int SWITCH_GetState(int switch_nr) {
	return switch_debounced[switch_nr];
}

/**
 * @brief  Get the state of the switch
 * @param  switch_nr Nr from 0 to 3
 * @retval 1, if switch was clicked
 */
int SWITCH_GetClick(int switch_nr) {
	int retval = switch_click[switch_nr];
	switch_click[switch_nr] = 0;
	return retval;
}
