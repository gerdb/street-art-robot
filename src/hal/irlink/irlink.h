/**
 *  Project     StreetArtRobot
 *  @file		irlink.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for irlink.c
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
#ifndef __IRLINK_H
#define __IRLINK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"



/* types ------------------------------------------------------------------*/
typedef enum {
	IR_WAIT_IDLE,
	IR_WAIT_HEADER,
	IR_HEADER,
	IR_PAUSE,
	IR_DATA_0A,
	IR_DATA_0B,
	IR_DATA_1A,
	IR_DATA_1B
} en_irstate;

typedef enum {
        TRACK_INIT = 0,
        TRACK_SEARCHING = 1,
        TRACK_LIGHT_FOUND = 2,
        TRACK_CENTER_DETECTED = 3,
        TRACK_LOST = 4
} en_irlink_StatusTypeDef;


/* global variables ------------------------------------------------------------------*/
extern int IRLINK_position_x;
extern int IRLINK_position_y;
extern int IRLINK_intensity;
extern en_irlink_StatusTypeDef IRLINK_status;

/* defines ------------------------------------------------------------------*/

#define IR_ACTIVE GPIO_PIN_RESET
#define IR_IDLE   GPIO_PIN_SET

// Clocks
#define IRLINK_IO_ENABLE()			__GPIOA_CLK_ENABLE()
// Pins
#define IRLINK_IN_PIN				GPIO_PIN_11

// Port
#define IRLINK_PORT					GPIOA

// Alternate functions
#define IRLINK_IN_AF				GPIO_AF1_TIM1
// Timer
#define IRLINK_TIMER				TIM10
//CLK
#define IRLINK_TIMER_CLK_ENABLE()	__TIM10_CLK_ENABLE()

//IRQ
#define IRLINK_IRQn                      TIM1_UP_TIM10_IRQn
#define IRLINK_IRQHandler                TIM1_UP_TIM10_IRQHandler

/* Function Prototypes --------------------------------------------------------*/
void IRLINK_Init(void);
void IRLINK_TimerIRQ(void);
void IRLINK_Decode(void);
char* IRLINK_GetStatusText(void);

#endif /* __IRLINK_H */
