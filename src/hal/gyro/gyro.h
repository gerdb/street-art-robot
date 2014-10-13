/**
 *  Project     StreetArtRobot
 *  @file		gyro.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for gyro.c
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
#ifndef __GYRO_H
#define __GYRO_H


                                              
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"


/*##################### Gyro on SPI3 ###################################*/
#define GYRO_SPI                              SPI3
#define GYRO_SPI_CLK_ENABLE()                 __SPI3_CLK_ENABLE()
#define GYRO_SPI_IO_PORT                      GPIOC                      /* GPIOA */
#define GYRO_SPI_CS_PORT                      GPIOA                      /* GPIOA */
#define GYRO_SPI_AF                           GPIO_AF6_SPI3
#define GYRO_SPI_CS_PIN                       GPIO_PIN_4                  /* PA.04 */
#define GYRO_SPI_SCK_PIN                      GPIO_PIN_10                 /* PC.10 */
#define GYRO_SPI_MISO_PIN                     GPIO_PIN_11                 /* PC.11 */
#define GYRO_SPI_MOSI_PIN                     GPIO_PIN_12                 /* PC.12 */

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define GYRO_SPI_TIMEOUT_MAX 0x1000 //The value of the maximal timeout for BUS waiting loops



/* Gyro addresses --------------------------------------------------------*/
#define GYRO_ANGL_OUT	0x0E
#define GYRO_PROD_ID	0x56
#define GYRO_SERIAL_NUM	0x58



/* Function Prototypes --------------------------------------------------------*/
void GYRO_Init(void);
uint16_t GYRO_GetAngle(void);

#endif /* __GYRO_H */


