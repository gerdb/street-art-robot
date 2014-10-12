/**
 *  Project     StreetArtRobot
 *  @file		sdcard.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for sdcard.c
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
#ifndef __SDCARD_H
#define __SDCARD_H


                                              
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"


/*##################### Gyro on SPI3 ###################################*/
#define SDCARD_SPI                              SPI2
#define SDCARD_SPI_CLK_ENABLE()                 __SPI2_CLK_ENABLE()
#define SDCARD_SPI_GPIO_CLK_ENABLE()            __GPIOB_CLK_ENABLE()
#define SDCARD_SPI_GPIO_CLK_DISABLE()           __GPIOB_CLK_DISABLE()
#define SDCARD_SPI_PORT                    		GPIOB                      /* GPIO  */
#define SDCARD_CD_PORT                    		GPIOB                      /* GPIO  */
#define SDCARD_SPI_AF                           GPIO_AF5_SPI2
#define SDCARD_CD_PIN                       	GPIO_PIN_1                 /* PB.1 */
#define SDCARD_SPI_CS_PIN                       GPIO_PIN_12                 /* PB.12 */
#define SDCARD_SPI_SCK_PIN                      GPIO_PIN_13                 /* PB.13 */
#define SDCARD_SPI_MISO_PIN                     GPIO_PIN_14                 /* PB.14 */
#define SDCARD_SPI_MOSI_PIN                     GPIO_PIN_15                 /* PB.15 */

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define SDCARD_SPI_TIMEOUT_MAX                            0x1000 //The value of the maximal timeout for BUS waiting loops




/* Function Prototypes --------------------------------------------------------*/
void SDCARD_Init(void);
uint8_t SDCARD_SPI_WriteRead(uint8_t data);
void SDCARD_SPI_SendMulti(uint8_t *dataOut, uint8_t *dataIn, uint16_t count);
void SDCARD_SPI_WriteMulti(uint8_t *dataOut, uint16_t count);
void SDCARD_SPI_ReadMulti(uint8_t *dataIn, uint8_t dummy, uint16_t count);


#endif /* __SDCARD_H */


