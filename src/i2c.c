/**
 *  Project     StreetArtRobot
 *  @file		i2c.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		I2C communication with oled
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

/* Includes -----------------------------------------------------------------*/

#include "i2c.h"

/* local variables ----------------------------------------------------------*/
static I2C_HandleTypeDef I2cHandle;

/* local functions ----------------------------------------------------------*/
static void I2C_Error(uint8_t Addr);
static void I2C_MspInit(void);
uint32_t I2CTimeout = I2C_TIMEOUT_MAX; /*<! Value of Timeout when I2C communication fails */

/******************************* I2C Routines**********************************/
/**
 * @brief  Configures I2C interface.
 * @param  None
 * @retval None
 */
void I2C_Init(void) {
	if (HAL_I2C_GetState(&I2cHandle) == HAL_I2C_STATE_RESET) {
		/* I2C peripheral configuration */
		I2cHandle.Init.ClockSpeed = I2C_SPEED;
		I2cHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
		I2cHandle.Init.OwnAddress1 = 0x33;
		I2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		I2cHandle.Instance = I2C_PORT;

		/* Init the I2C */
		I2C_MspInit();
		HAL_I2C_Init(&I2cHandle);
	}
}

/**
 * @brief  Write a value in a register of the device through BUS.
 * @param  Addr: Device address on BUS Bus.
 * @param  Reg: The target register address to write
 * @param  Value: The target register value to be written
 * @retval HAL status
 */
void I2C_WriteData(uint8_t Addr, uint8_t Reg, uint8_t Value) {
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Write(&I2cHandle, Addr, Reg, I2C_MEMADD_SIZE_8BIT,
			&Value, 1, I2CTimeout);

	/* Check the communication status */
	if (status != HAL_OK) {
		/* Execute user timeout callback */
		I2C_Error(Addr);
	}
}

/**
 * @brief  Write a value in a register of the device through BUS.
 * @param  Addr: Device address on BUS Bus.
 * @param  Reg: The target register address to write
 * @param  pValue: pointer to the data array to be written
 * @param  n: number of bytes to be written
 * @retval HAL status
 */
void I2C_WriteDataBytes(uint8_t Addr, uint8_t Reg, uint8_t* pValue, uint8_t n) {
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Write(&I2cHandle, Addr, Reg, I2C_MEMADD_SIZE_8BIT,
			pValue, n, I2CTimeout);

	/* Check the communication status */
	if (status != HAL_OK) {
		/* Execute user timeout callback */
		I2C_Error(Addr);
	}
}
/**
 * @brief  Read a register of the device through BUS
 * @param  Addr: Device address on BUS .
 * @param  Reg: The target register address to read
 * @retval HAL status
 */
uint8_t I2C_ReadData(uint8_t Addr, uint8_t Reg) {
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value = 0;

	status = HAL_I2C_Mem_Read(&I2cHandle, Addr, Reg, I2C_MEMADD_SIZE_8BIT,
			&value, 1, I2CTimeout);

	/* Check the communication status */
	if (status != HAL_OK) {
		/* Execute user timeout callback */
		I2C_Error(Addr);
	}
	return value;
}

/**
 * @brief  Manages error callback by re-initializing I2C.
 * @param  Addr: I2C Address
 * @retval None
 */
static void I2C_Error(uint8_t Addr) {
	/* De-initialize the I2C comunication bus */
	HAL_I2C_DeInit(&I2cHandle);

	/* Re-Initiaize the I2C comunication bus */
	I2C_Init();
}

/**
 * @brief I2C MSP Initialization
 * @param None
 * @retval None
 */
static void I2C_MspInit(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Enable I2C GPIO clocks */
	I2C_SCL_SDA_GPIO_CLK_ENABLE();

	/* I2C SCL and SDA pins configuration -------------------------------------*/
	GPIO_InitStruct.Pin = I2C_SCL_PIN | I2C_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Alternate = I2C_SCL_SDA_AF;
	HAL_GPIO_Init(I2C_SCL_SDA_GPIO_PORT, &GPIO_InitStruct);

	/* Enable the I2C peripheral clock */
	I2C_CLK_ENABLE();

	/* Force the I2C peripheral clock reset */
	I2C_FORCE_RESET();

	/* Release the I2C peripheral clock reset */
	I2C_RELEASE_RESET();

	/* Enable and set I2C Interrupt to the highest priority */
	HAL_NVIC_SetPriority(I2C_EV_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(I2C_EV_IRQn);

	/* Enable and set I2C Interrupt to the highest priority */
	HAL_NVIC_SetPriority(I2C_ER_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(I2C_ER_IRQn);
}
