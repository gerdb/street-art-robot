/**
 *  Project     StreetArtRobot
 *  @file		switch.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for switch.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SWITCH_H
#define __SWITCH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* defines ------------------------------------------------------------------*/
#define SWITCH_CLK_ENABLE()			__GPIOE_CLK_ENABLE()
#define SWITCH_SW1_PIN				GPIO_PIN_12
#define SWITCH_SW2_PIN				GPIO_PIN_13
#define SWITCH_SW3_PIN				GPIO_PIN_14
#define SWITCH_SW4_PIN				GPIO_PIN_15
#define SWITCH_PORT					GPIOE

#define SWITCH_SW1	0
#define SWITCH_SW2	1
#define SWITCH_SW3	2
#define SWITCH_SW4	3

#define SWITCH_LEFT 	SWITCH_SW1
#define SWITCH_RIGHT 	SWITCH_SW2
#define SWITCH_UP 		SWITCH_SW3
#define SWITCH_DOWN		SWITCH_SW4

#define SWITCH_RELEASED	0
#define SWITCH_PRESSED 	1
#define SWITCH_CLICK 	2

#define SWITCH_DEBOUNCE_TIME 50 //ms

/* Function Prototypes --------------------------------------------------------*/
void SWITCH_Init(void);
void SWITCH_1msTask(void);
int SWITCH_GetState(int switch_nr);
int SWITCH_GetClick(int switch_nr);


#endif // __SWITCH_H
