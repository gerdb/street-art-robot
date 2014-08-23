/**
 *  Project     StreetArtRobot
 *  @file		led.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for led.c
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
#ifndef __LED_H
#define __LED_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* defines ------------------------------------------------------------------*/
#define LED_CLK_ENABLE()		__GPIOA_CLK_ENABLE()
#define LED_PIN					GPIO_PIN_12
#define LED_PORT				GPIOA


#define LED_OFF		0
#define LED_ON		1

/* Function Prototypes --------------------------------------------------------*/
void LED_Init(void);
void LED_SetVal(int value);

#endif // __LED_H
