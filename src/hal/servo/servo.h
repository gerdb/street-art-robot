/**
 *  Project     StreetArtRobot
 *  @file		servo.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for servo.c
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
#ifndef __SERVO_H
#define __SERVO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* defines ------------------------------------------------------------------*/
#define SERVO_GPIO_CLK_ENABLE()			__GPIOE_CLK_ENABLE()
#define SERVO_GPIO_1_PIN				GPIO_PIN_5
#define SERVO_GPIO_2_PIN				GPIO_PIN_6
#define SERVO_GPIO_PORT					GPIOE
#define SERVO_GPIO_TIMER_AF				GPIO_AF3_TIM9
#define SERVO_TIMER						TIM9

#define SERVO_1	1
#define SERVO_2 2

/* Function Prototypes --------------------------------------------------------*/
void SERVO_Init(void);
void SERVO_SetVal(int servoNr, int value);

#endif // __SERVO_H
