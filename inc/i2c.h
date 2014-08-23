/**
 *  Project     StreetArtRobot
 *  @file		i2c.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for i2c.c
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
#ifndef __I2C_H
#define __I2C_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* defines ------------------------------------------------------------------*/
#ifndef I2C_SPEED
#define I2C_SPEED                            100000
#endif /* I2C_SPEED */

#define I2C_PORT                       I2C1
#define I2C_CLK_ENABLE()               __I2C1_CLK_ENABLE()
#define I2C_SCL_SDA_GPIO_CLK_ENABLE()  __GPIOB_CLK_ENABLE()
#define I2C_SCL_SDA_AF                 GPIO_AF4_I2C1
#define I2C_SCL_SDA_GPIO_PORT          GPIOB
#define I2C_SCL_PIN                    GPIO_PIN_6
#define I2C_SDA_PIN                    GPIO_PIN_7

#define I2C_FORCE_RESET()              __I2C1_FORCE_RESET()
#define I2C_RELEASE_RESET()            __I2C1_RELEASE_RESET()

#define I2C_EV_IRQn                    I2C1_EV_IRQn
#define I2C_ER_IRQn                    I2C1_ER_IRQn

#define I2C_TIMEOUT_MAX    0x1000

/* Function Prototypes --------------------------------------------------------*/
void I2C_Init(void);
void I2C_WriteData(uint8_t Addr, uint8_t Reg, uint8_t Value);
void I2C_WriteDataBytes(uint8_t Addr, uint8_t Reg, uint8_t* pValue, uint8_t n);
uint8_t I2C_ReadData(uint8_t Addr, uint8_t Reg);

#endif /* __I2C_H */
