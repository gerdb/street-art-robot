/**
 *  Project     StreetArtRobot
 *  @file		stm32f4_board.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for stm32f4_board.c
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
#ifndef __STM32F4_BOARD_H
#define __STM32F4_BOARD_H


                                              
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"


/*##################### SPI1 ###################################*/
#define OLED_SPIx                              SPI1
#define OLED_SPIx_CLK_ENABLE()                 __SPI1_CLK_ENABLE()
#define OLED_SPIx_GPIO_PORT                    GPIOA                      /* GPIOA */
#define OLED_SPIx_AF                           GPIO_AF5_SPI1
#define OLED_SPIx_GPIO_CLK_ENABLE()            __GPIOA_CLK_ENABLE()
#define OLED_SPIx_GPIO_CLK_DISABLE()           __GPIOA_CLK_DISABLE()
#define OLED_SPIx_SCK_PIN                      GPIO_PIN_5                 /* PA.05 */
#define OLED_SPIx_MISO_PIN                     GPIO_PIN_6                 /* PA.06 */
#define OLED_SPIx_MOSI_PIN                     GPIO_PIN_7                 /* PA.07 */

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define SPIx_TIMEOUT_MAX                            0x1000 //The value of the maximal timeout for BUS waiting loops


#endif /* __STM32F4_BOARD */


