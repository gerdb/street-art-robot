/**
 *  Project     StreetArtRobot
 *  @file		rc.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for rc.c
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
#ifndef __RC_H
#define __RC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* defines ------------------------------------------------------------------*/
#define USART_RC                           USART1
#define USART_RC_CLK_ENABLE()              __USART1_CLK_ENABLE()
#define USART_RC_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()

#define USART_RC_FORCE_RESET()             __USART1_FORCE_RESET()
#define USART_RC_RELEASE_RESET()           __USART1_RELEASE_RESET()

// Definition for USARTx Pins
#define USART_RC_RX_PIN                    GPIO_PIN_10
#define USART_RC_RX_GPIO_PORT              GPIOA
#define USART_RC_RX_AF                     GPIO_AF7_USART1

// Definition for USARTx's NVIC
#define USART_RC_IRQn                      USART1_IRQn
#define USART_RC_IRQHandler                USART1_IRQHandler

#define RC_KEY_GO	0
#define RC_KEY_STOP	1
#define RC_KEY_RED	2

/* Function Prototypes --------------------------------------------------------*/
void RC_Init(void);
void RC_1msTask(void);
void RC_USART_IRQHandler(void);
void RC_Receive(void);
int RC_GetKey(int key);
int RC_GetClick(int key);

#endif // __RC_H
