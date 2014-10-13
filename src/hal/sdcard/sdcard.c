/**
 *  Project     StreetArtRobot
 *  @file		sdcard.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		SDCard SPI
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
#include "sdcard.h"

uint32_t SDCardSpiTimeout = SDCARD_SPI_TIMEOUT_MAX; // Value of Timeout when SPI communication fails

static SPI_HandleTypeDef SDCardSpiHandle;
static void SDCARD_SPI_Error(void);

/******************************************************************************
 BUS OPERATIONS
 *******************************************************************************/

/******************************* SPI Routines**********************************/
/**
 * @brief  Configures SPI interface.
 * @param  None
 * @retval None
 */
void SDCARD_Init(void) {
	if (HAL_SPI_GetState(&SDCardSpiHandle) == HAL_SPI_STATE_RESET) {
		/* SPI configuration -------------------------------------------------------*/
		SDCardSpiHandle.Instance = SDCARD_SPI;
		SDCardSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
		SDCardSpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
		SDCardSpiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
		SDCardSpiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
		SDCardSpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
		SDCardSpiHandle.Init.CRCPolynomial = 7;
		SDCardSpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
		SDCardSpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
		SDCardSpiHandle.Init.NSS = SPI_NSS_SOFT;
		SDCardSpiHandle.Init.TIMode = SPI_TIMODE_DISABLED;
		SDCardSpiHandle.Init.Mode = SPI_MODE_MASTER;

		GPIO_InitTypeDef GPIO_InitStructure;

		/* Enable the SPI periph */
		SDCARD_SPI_CLK_ENABLE();

		/* Enable CS, SCK, MOSI and MISO GPIO clocks */
		SDCARD_SPI_GPIO_CLK_ENABLE();

		/* SPI SCK, MOSI, MISO pin configuration */
		GPIO_InitStructure.Pin = (SDCARD_SPI_SCK_PIN
				| SDCARD_SPI_MISO_PIN | SDCARD_SPI_MOSI_PIN);
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull = GPIO_PULLDOWN;
		GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
		GPIO_InitStructure.Alternate = SDCARD_SPI_AF;
		HAL_GPIO_Init(SDCARD_SPI_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = (SDCARD_SPI_CS_PIN);
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStructure.Pull = GPIO_PULLDOWN;
		GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
		HAL_GPIO_Init(SDCARD_SPI_PORT, &GPIO_InitStructure);

		HAL_SPI_Init(&SDCardSpiHandle);
	}
}

/**
 * @brief  Sends a Byte through the SPI interface and return the Byte received
 *         from the SPI bus.
 * @param  Byte : Byte send.
 * @retval The received byte value
 */
uint8_t SDCARD_SPI_WriteRead(uint8_t data) {
	uint8_t retval;

	// Send the 2 bytes and receive 2 bytes
	//HAL_GPIO_WritePin(SDCARD_SPI_PORT, SDCARD_SPI_CS_PIN, RESET);
	if (HAL_SPI_TransmitReceive(&SDCardSpiHandle, (uint8_t*) &data,
			(uint8_t*) &retval, 1, SDCardSpiTimeout) != HAL_OK) {
		SDCARD_SPI_Error();
	}
	//HAL_GPIO_WritePin(SDCARD_SPI_PORT, SDCARD_SPI_CS_PIN, SET);
	return retval;
}

void SDCARD_SPI_WriteMulti(uint8_t *dataOut, uint16_t count) {
	uint16_t i;
	for (i = 0; i < count; i++) {
		SDCARD_SPI_WriteRead(dataOut[i]);
	}
}

void SDCARD_SPI_ReadMulti(uint8_t *dataIn, uint8_t dummy, uint16_t count) {
	uint16_t i;
	for (i = 0; i < count; i++) {
		dataIn[i] = SDCARD_SPI_WriteRead(dummy);
	}
}

/**
 * @brief Bus error user callback function
 * @param None
 * @retval None
 */
static void SDCARD_SPI_Error(void) {
	/* De-initialize the SPI comunication bus */
	HAL_SPI_DeInit(&SDCardSpiHandle);

	/* Re-Initiaize the SPI comunication bus */
	//SCARD_Init();
}

