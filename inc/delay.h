/**
 *  Project     StreetArtRobot
 *  @file		delay.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for delay.c
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
#ifndef __DELAY_H
#define __DELAY_H


                                              
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"



/* Function Prototypes --------------------------------------------------------*/
void DELAY_Init(void);
void DELAY_ms(uint32_t ms);
uint32_t DELAY_GetTime(void);
void DELAY_SetTime(uint32_t ms);
void DELAY_1msTask(void);

#endif /* __DELAY_H */


