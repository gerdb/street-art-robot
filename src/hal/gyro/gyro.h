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
#include "view_controller.h"

/* Types ------------------------------------------------------------------*/
typedef enum {
	GYRO_INIT,
	GYRO_INIT_WAIT,
	GYRO_INIT_AUTONULL,
	GYRO_INIT_AUTONULL_WAIT,
	GYRO_RUN,
	GYRO_OFFSET_AUTONULL_WAIT1,
	GYRO_OFFSET_AUTONULL,
	GYRO_OFFSET_AUTONULL_WAIT2,
	GYRO_OFFSET_WAIT10s,
	GYRO_OFFSET_SENS_AVG,
	GYRO_OFFSET_GYRO_OFF_0,
	GYRO_OFFSET_COLLECT_DATA,
	GYRO_OFFSET_CALC_OFFSET,
	GYRO_OFFSET_GYRO_OFF_NEW,
	GYRO_OFFSET_UPDATE_FLASH,
	GYRO_OFFSET_WAIT50ms,
	GYRO_OFFSET_MANUAL
} en_gyro_state;

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
#define GYRO_OUT		0x04
#define GYRO_ANGL_OUT	0x0E
#define GYRO_OFF		0x14
#define GYRO_SENS_AVG	0x38
#define GYRO_GLOB_CMD	0x3E
#define GYRO_PROD_ID	0x56
#define GYRO_SERIAL_NUM	0x58

#define GYRO_WRITE		0x80
#define GYRO_LOW		0x00
#define GYRO_HIGH		0x01





/* Function Prototypes --------------------------------------------------------*/
void GYRO_Init(void);
uint16_t GYRO_GetAngle(void);
void GYRO_1msTask(void);
void GYRO_Task(void);
char* GYRO_GetText(en_view_main_menu);
void GYRO_StartOffset(void);
void GYRO_StartAutoNull(void);
void GYRO_ManOffset(int delta);

#endif /* __GYRO_H */


