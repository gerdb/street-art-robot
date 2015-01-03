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
#include "printf.h"

/* local variables variables ------------------------------------------------------------------*/
static SPI_HandleTypeDef GyroSpiHandle;
en_gyro_state gyro_state = GYRO_INIT;
uint32_t SpiTimeout = GYRO_SPI_TIMEOUT_MAX; // Value of Timeout when SPI communication fails
int gyro_1ms_cnt = 0;
int gyro_4ms_cnt = 0;
int gyro_coll_cnt = 0;
int32_t gyro_mean = 0;

/* local functions variables ------------------------------------------------------------------*/
static uint16_t GYRO_SPI_WriteRead(uint8_t address, uint8_t data);
static void GYRO_SPI_Error(void);
char gyro_txt[20]; // Temporary memory for strings
int gyro_manOffs = 0;

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
 * @brief  1ms task
 * @param  None
 * @retval None
 */
void GYRO_1msTask(void) {
	gyro_1ms_cnt++;
	int16_t g;
	if (gyro_state == GYRO_OFFSET_COLLECT_DATA) {
		gyro_4ms_cnt++;
		if (gyro_4ms_cnt > 4) {
			gyro_4ms_cnt = 0;

			g = GYRO_SPI_WriteRead(GYRO_OUT, 0) & 0x3FFF;
			// 2's complement
			if (g & 0x2000)
				g |= 0xC000;

			if (gyro_coll_cnt < 16384) {
				gyro_mean += g;
				gyro_coll_cnt++;
			}

		}
	}
}

/**
 * @brief  slow gyro task
 * @param  None
 * @retval None
 */
void GYRO_Task(void) {

	switch (gyro_state) {

	case GYRO_INIT:
		gyro_1ms_cnt = 0;
		gyro_state = GYRO_INIT_WAIT;
		break;

	case GYRO_INIT_WAIT:
		if (gyro_1ms_cnt > 2000)
			gyro_state = GYRO_INIT_AUTONULL;
		break;

	case GYRO_INIT_AUTONULL:
		// Autonull
		GYRO_SPI_WriteRead(GYRO_GLOB_CMD | GYRO_WRITE, 0x01);
		gyro_1ms_cnt = 0;
		gyro_state = GYRO_INIT_AUTONULL_WAIT;
		break;

	case GYRO_INIT_AUTONULL_WAIT:
		if (gyro_1ms_cnt > 1000)
			gyro_state = GYRO_RUN;
		break;

	case GYRO_OFFSET_AUTONULL_WAIT1:
		if (gyro_1ms_cnt > 1000)
			gyro_state = GYRO_OFFSET_AUTONULL;
		break;

	case GYRO_OFFSET_AUTONULL:

		// Autonull
		GYRO_SPI_WriteRead(GYRO_GLOB_CMD | GYRO_WRITE, 0x01);

		gyro_1ms_cnt = 0;
		gyro_state = GYRO_OFFSET_AUTONULL_WAIT2;
		break;

	case GYRO_OFFSET_AUTONULL_WAIT2:
		if (gyro_1ms_cnt > 1000)
			gyro_state = GYRO_RUN;
		break;

	case GYRO_OFFSET_WAIT10s:
		if (gyro_1ms_cnt > 10000)
			gyro_state = GYRO_OFFSET_SENS_AVG;
		break;

	case GYRO_OFFSET_SENS_AVG:
		GYRO_SPI_WriteRead(GYRO_SENS_AVG | GYRO_HIGH | GYRO_WRITE, 0x01);
		gyro_state = GYRO_OFFSET_GYRO_OFF_0;
		break;

	case GYRO_OFFSET_GYRO_OFF_0:
		GYRO_SPI_WriteRead(GYRO_OFF | GYRO_LOW | GYRO_WRITE, 0);
		GYRO_SPI_WriteRead(GYRO_OFF | GYRO_HIGH | GYRO_WRITE, 0);
		gyro_1ms_cnt = 0;
		gyro_4ms_cnt = 0;
		gyro_mean = 0;
		gyro_coll_cnt = 0;
		gyro_state = GYRO_OFFSET_COLLECT_DATA;
		break;

	case GYRO_OFFSET_COLLECT_DATA:
		if (gyro_coll_cnt >= 16384)
			gyro_state = GYRO_OFFSET_CALC_OFFSET;
		break;

	case GYRO_OFFSET_CALC_OFFSET:
		// Calculate the new offset value
		gyro_mean += 8192;
		gyro_mean /= 16384;
		gyro_mean = -gyro_mean;
		gyro_state = GYRO_OFFSET_GYRO_OFF_NEW;
		break;

	case GYRO_OFFSET_GYRO_OFF_NEW:
		GYRO_SPI_WriteRead(GYRO_OFF | GYRO_LOW | GYRO_WRITE,
				gyro_mean & 0x00FF);
		GYRO_SPI_WriteRead(GYRO_OFF | GYRO_HIGH | GYRO_WRITE,
				(gyro_mean >> 8) & 0x003F);
		gyro_state = GYRO_OFFSET_UPDATE_FLASH;
		break;

	case GYRO_OFFSET_UPDATE_FLASH:
		// Update flash
		GYRO_SPI_WriteRead(GYRO_GLOB_CMD | GYRO_WRITE, 0x08);

		gyro_state = GYRO_OFFSET_WAIT50ms;
		gyro_1ms_cnt = 0;
		break;

	case GYRO_OFFSET_WAIT50ms:
		if (gyro_1ms_cnt > 5000)
			gyro_state = GYRO_RUN;
		break;

	default:
		;
	}
}

/**
 * @brief  Display a gyro specific text
 * @param  en_view_main_menu view_main_menu_old
 * @retval The text to display
 */
char* GYRO_GetText(en_view_main_menu main_menu) {

	// Initialize the gyro
	if (gyro_state >= GYRO_INIT && gyro_state <= GYRO_INIT_AUTONULL_WAIT)
		return "INIT";


	switch (main_menu) {

	case VIEW_MAINM_GYRO:
		sprintf(gyro_txt, "%5u", GYRO_GetAngle());
		return gyro_txt;

	case VIEW_MAINM_GYRO_NULL:
		if (gyro_state == GYRO_OFFSET_AUTONULL
				|| gyro_state == GYRO_OFFSET_AUTONULL_WAIT1
				|| gyro_state == GYRO_OFFSET_AUTONULL_WAIT2)
			return "AUTONULL";
		else
			return "-->     ";
		break;

	case VIEW_MAINM_GYRO_MANOFF:
		sprintf(gyro_txt, "%5u %5d", GYRO_GetAngle(), gyro_manOffs);
		return gyro_txt;

	case VIEW_MAINM_GYRO_OFFSET:
		switch (gyro_state) {

		case GYRO_OFFSET_WAIT10s:
			sprintf(gyro_txt, "wait: %2u", ((10500 - gyro_1ms_cnt) / 1000));
			return gyro_txt;

		case GYRO_OFFSET_SENS_AVG:
			return "SENS AVG";

		case GYRO_OFFSET_GYRO_OFF_0:
			return "OFFSET 0";

		case GYRO_OFFSET_COLLECT_DATA:
			sprintf(gyro_txt, "data:%3u", (16384 - gyro_coll_cnt) / 128);
			return gyro_txt;

		case GYRO_OFFSET_CALC_OFFSET:
			return "CALC OFF";

		case GYRO_OFFSET_GYRO_OFF_NEW:
			return "OFF NEW ";

		case GYRO_OFFSET_UPDATE_FLASH:
			return "FLASH   ";

		case GYRO_OFFSET_WAIT50ms:
			sprintf(gyro_txt, "m:%5d", (int) (gyro_mean));
			return gyro_txt;

		default:
			return "-->     ";
		}

		break;

	default: ; // Nothing

	}
	return "";
}

/**
 * @brief  start the offset calibration
 * @param  none.
 * @retval none
 */
void GYRO_StartOffset(void) {
	gyro_state = GYRO_OFFSET_WAIT10s;
	gyro_1ms_cnt = 0;
}

/**
 * @brief  start the autonull function
 * @param  none.
 * @retval none
 */
void GYRO_StartAutoNull(void) {
	gyro_state = GYRO_OFFSET_AUTONULL_WAIT1;
	gyro_1ms_cnt = 0;
}

/**
 * @brief  Manual offset
 * @param  none.
 * @retval none
 */
void GYRO_ManOffset(int delta) {

	gyro_state = GYRO_OFFSET_MANUAL;

	//gyro_manOffs = GYRO_SPI_WriteRead(GYRO_OFF, 0) & 0x3FFF;
	gyro_manOffs += delta;
	GYRO_SPI_WriteRead(GYRO_OFF | GYRO_LOW | GYRO_WRITE, gyro_manOffs & 0x00FF);
	GYRO_SPI_WriteRead(GYRO_OFF | GYRO_HIGH | GYRO_WRITE,
			(gyro_manOffs >> 8) & 0x003F);

	gyro_state = GYRO_RUN;
}

/**
 * @brief  Gets the Gyro angle as integer
 * @param  none.
 * @retval The angle from 0.. 65535 (0 to 9827)
 */
uint16_t GYRO_GetAngle(void) {
	if (gyro_state == GYRO_RUN)
		return ((int)(GYRO_SPI_WriteRead(GYRO_ANGL_OUT, 0) & 0x3FFF) *1707) >> 8;
	else
		return 0;
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

