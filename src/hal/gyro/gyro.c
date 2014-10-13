/**
 *  Project     StreetArtRobot
 *  @file		gyro.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		gyro sensor
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
/* Includes ------------------------------------------------------------------*/
#include "gyro.h"

uint32_t SpiTimeout = GYRO_SPI_TIMEOUT_MAX; // Value of Timeout when SPI communication fails

static SPI_HandleTypeDef GyroSpiHandle;

static uint16_t GYRO_SPI_WriteRead(uint8_t address, uint8_t data);
static void GYRO_SPI_Error(void);

/******************************************************************************
 BUS OPERATIONS
 *******************************************************************************/

/******************************* SPI Routines**********************************/
/**
 * @brief  Configures SPI interface.
 * @param  None
 * @retval None
 */
void GYRO_Init(void) {
	if (HAL_SPI_GetState(&GyroSpiHandle) == HAL_SPI_STATE_RESET) {
		/* SPI configuration -------------------------------------------------------*/
		GyroSpiHandle.Instance = GYRO_SPI;
		GyroSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
		GyroSpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
		GyroSpiHandle.Init.CLKPhase = SPI_PHASE_2EDGE;
		GyroSpiHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;
		GyroSpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
		GyroSpiHandle.Init.CRCPolynomial = 7;
		GyroSpiHandle.Init.DataSize = SPI_DATASIZE_16BIT;
		GyroSpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
		GyroSpiHandle.Init.NSS = SPI_NSS_SOFT;
		GyroSpiHandle.Init.TIMode = SPI_TIMODE_DISABLED;
		GyroSpiHandle.Init.Mode = SPI_MODE_MASTER;

		GPIO_InitTypeDef GPIO_InitStructure;

		/* Enable the SPI periph */
		GYRO_SPI_CLK_ENABLE();

		/* Enable CS, SCK, MOSI and MISO GPIO clocks */
		__GPIOA_CLK_ENABLE();
		__GPIOC_CLK_ENABLE();

		/* SPI SCK, MOSI, MISO pin configuration */
		GPIO_InitStructure.Pin = (GYRO_SPI_SCK_PIN | GYRO_SPI_MISO_PIN
				| GYRO_SPI_MOSI_PIN);
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull = GPIO_PULLDOWN;
		GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
		GPIO_InitStructure.Alternate = GYRO_SPI_AF;
		HAL_GPIO_Init(GYRO_SPI_IO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = (GYRO_SPI_CS_PIN);
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStructure.Pull = GPIO_PULLDOWN;
		GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
		HAL_GPIO_Init(GYRO_SPI_CS_PORT, &GPIO_InitStructure);

		HAL_SPI_Init(&GyroSpiHandle);
	}
}

/**
 * @brief  Gets the Gyro angle as integer
 * @param  none.
 * @retval The angle from 0 to 9827
 */
uint16_t GYRO_GetAngle(void) {
	return GYRO_SPI_WriteRead(GYRO_ANGL_OUT, 0) & 0x3FFF;
}

/**
 * @brief  Sends a Byte through the SPI interface and return the Byte received
 *         from the SPI bus.
 * @param  address : the address byte
 * @param  address : the data byte
 * @retval The received word value
 */
static uint16_t GYRO_SPI_WriteRead(uint8_t address, uint8_t data) {
	uint8_t txbytes[2];
	uint16_t retval;

	txbytes[1] = address;
	txbytes[0] = data;

	// Send the 2 bytes and receive 2 bytes
	HAL_GPIO_WritePin(GYRO_SPI_CS_PORT, GYRO_SPI_CS_PIN, RESET);
	if (HAL_SPI_TransmitReceive(&GyroSpiHandle, (uint8_t*) &txbytes,
			(uint8_t*) &retval, 1, SpiTimeout) != HAL_OK) {
		GYRO_SPI_Error();
	}
	HAL_GPIO_WritePin(GYRO_SPI_CS_PORT, GYRO_SPI_CS_PIN, SET);
	return retval;
}

/**
 * @brief Bus error user callback function
 * @param None
 * @retval None
 */
static void GYRO_SPI_Error(void) {
	/* De-initialize the SPI comunication bus */
	HAL_SPI_DeInit(&GyroSpiHandle);

	/* Re-Initiaize the SPI comunication bus */
	//GYRO_Init();
}

