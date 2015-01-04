/**
 *  Project     StreetArtRobot
 *  @file		irlink.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		infrared interface
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

#include "irlink.h"

/* global variables ----------------------------------------------------------*/
int IRLINK_position_x = 0;
int IRLINK_position_y = 0;
int IRLINK_intensity = 0;
en_irlink_StatusTypeDef IRLINK_status = TRACK_INIT;

/* local variables ----------------------------------------------------------*/
uint16_t irdata[4] = { 0, 0, 0, 0 };

TIM_HandleTypeDef htimIRlink;
TIM_IC_InitTypeDef sConfigIRlink;
CRC_HandleTypeDef CrcHandle;
uint32_t IRuwCRCValue = 0;
int ir_time;
en_irstate ir_state = IR_WAIT_IDLE;
int data_bit_cnt;
int data_word_cnt;
uint16_t crcReceived;

/* local functions ----------------------------------------------------------*/
static void IRLINK_Received(int bit);

/* functions ----------------------------------------------------------------*/

/**
 * @brief  Initialize the module and configure PWM PB5 as PWM output with 36kHz
 * @param  None
 * @retval None
 */
void IRLINK_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable the clocks
	IRLINK_IO_ENABLE();
	IRLINK_TIMER_CLK_ENABLE();

	GPIO_InitStruct.Pin = IRLINK_IN_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(IRLINK_PORT, &GPIO_InitStruct);

	// Timer configuration
	htimIRlink.Instance = IRLINK_TIMER;
	htimIRlink.Init.Period = 166 - 1; //166us
	htimIRlink.Init.Prescaler = 168 - 1;
	htimIRlink.Init.ClockDivision = 0;
	htimIRlink.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&htimIRlink);
	HAL_TIM_Base_Start_IT(&htimIRlink);

	// Configure the NVIC
	HAL_NVIC_SetPriority(IRLINK_IRQn, 0, 1);

	// Configure the CRC module
	CrcHandle.Instance = CRC;
	HAL_CRC_Init(&CrcHandle);

	// Enable the timer global Interrupt
	HAL_NVIC_EnableIRQ(IRLINK_IRQn);

}

/**
 * @brief CRC MSP Initialization
 *        This function configures the hardware resources used in this example:
 *           - Peripheral's clock enable
 * @param hcrc: CRC handle pointer
 * @retval None
 */
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc) {
	/* CRC Peripheral clock enable */
	__CRC_CLK_ENABLE();
}

/**
 * @brief CRC MSP De-Initialization
 *        This function freeze the hardware resources used in this example:
 *          - Disable the Peripheral's clock
 * @param hcrc: CRC handle pointer
 * @retval None
 */
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc) {
	/* Enable CRC reset state */
	__CRC_FORCE_RESET();

	/* Release CRC from reset state */
	__CRC_RELEASE_RESET();
}

/**
 * @brief The timer update event (every 0.5msec)
 * @param None
 * @retval None
 */
void IRLINK_TimerIRQ(void) {
	// TIM Update event
	if (__HAL_TIM_GET_FLAG(&htimIRlink, TIM_FLAG_UPDATE) != RESET) {
		if (__HAL_TIM_GET_ITSTATUS(&htimIRlink, TIM_IT_UPDATE) != RESET) {
			__HAL_TIM_CLEAR_IT(&htimIRlink, TIM_IT_UPDATE);
			IRLINK_Decode();
		}
	}
}

/**
 * @brief Decodes the incomming data stream as manchester code
 * @param None
 * @retval None
 */
void IRLINK_Decode(void) {
	GPIO_PinState ir_in;

	ir_in = HAL_GPIO_ReadPin(IRLINK_PORT, IRLINK_IN_PIN);

	switch (ir_state) {

	// Wait until there is silence
	case IR_WAIT_IDLE:
		if (ir_in == IR_IDLE)
			ir_state = IR_WAIT_HEADER;
		break;

		// Wait until the header is received
	case IR_WAIT_HEADER:
		if (ir_in == IR_ACTIVE) {
			ir_time = 0;
			ir_state = IR_HEADER;
		}

		break;

		// Wait until the header is complete
	case IR_HEADER:
		ir_time++;

		if (ir_in == IR_IDLE) {
			// If the header was at least 5us long, it was valid
			if (ir_time >= (3 * 5)) {
				ir_time = 0;
				ir_state = IR_PAUSE;
			} else {
				// invalid, wait for a new header
				ir_state = IR_WAIT_HEADER;
			}
		}
		break;

		// Wait until the header is complete
	case IR_PAUSE:
		ir_time++;

		// Reset some variables for data reception
		data_bit_cnt = 0;
		data_word_cnt = 0;

		// Exit, if the signal goes high
		if (ir_in == IR_ACTIVE) {
			if (ir_time >= (3 * 3)) {

				if (ir_time >= (3 * 4 - 1)) {
					IRLINK_Received(0);
					ir_state = IR_DATA_0B;
				} else {
					IRLINK_Received(1);
					ir_state = IR_DATA_1A;
				}
				ir_time = 0;
			} else
				ir_state = IR_WAIT_IDLE;
		}

		// signal was longer than 5us low
		if (ir_time >= (3 * 5)) {
			ir_time = 0;
			ir_state = IR_WAIT_IDLE;
		}
		break;

		// Wait for new data after a "0"
	case IR_DATA_0A:
		ir_time++;

		// Exit, if the signal goes low
		if (ir_in == IR_ACTIVE) {

			if ((ir_time >= (3 * 1 - 1)) && (ir_time <= (3 * 1 + 1))) {
				ir_state = IR_DATA_0B;
			} else
				ir_state = IR_WAIT_IDLE;

			ir_time = 0;
		}

		break;

		// Wait for new data after a "0"
	case IR_DATA_0B:
		ir_time++;

		// Exit, if the signal goes high
		if (ir_in == IR_IDLE) {

			if ((ir_time >= (3 * 1 - 1)) && (ir_time <= (3 * 1 + 1))) {
				IRLINK_Received(0);
				ir_state = IR_DATA_0A;
			} else if ((ir_time >= (3 * 2 - 1)) && (ir_time <= (3 * 2 + 1))) {
				IRLINK_Received(1);
				ir_state = IR_DATA_1B;
			} else
				ir_state = IR_WAIT_IDLE;

			ir_time = 0;
		}
		break;

		// Wait for new data after a "1"
	case IR_DATA_1A:
		ir_time++;
		// Exit, if the signal goes low
		if (ir_in == IR_IDLE) {

			if ((ir_time >= (3 * 1 - 1)) && (ir_time <= (3 * 1 + 1))) {
				ir_state = IR_DATA_1B;
			} else
				ir_state = IR_WAIT_IDLE;

			ir_time = 0;
		}
		break;

		// Wait for new data after a "1"
	case IR_DATA_1B:
		ir_time++;
		// Exit, if the signal goes high
		if (ir_in == IR_ACTIVE) {

			if ((ir_time >= (3 * 1 - 1)) && (ir_time <= (3 * 1 + 1))) {
				IRLINK_Received(1);
				ir_state = IR_DATA_1A;
			} else if ((ir_time >= (3 * 2 - 1)) && (ir_time <= (3 * 2 + 1))) {
				IRLINK_Received(0);
				ir_state = IR_DATA_0B;
			} else
				ir_state = IR_WAIT_IDLE;
			ir_time = 0;
		}

		break;
	}
}

/**
 * @brief Decodes one bit of the manchester code
 * @param bit: the received
 * @retval None
 */
void IRLINK_Received(int bit) {

	// Fill the data
	irdata[data_word_cnt] <<= 1;
	irdata[data_word_cnt] |= bit;

	data_bit_cnt++;
	if (data_bit_cnt >= 16) {

		// Next byte
		data_bit_cnt = 0;
		data_word_cnt++;
		if (data_word_cnt >= 4) {
			crcReceived = irdata[3];
			irdata[3] = 0;

			// Calculate the CRC
			IRuwCRCValue = (uint16_t) HAL_CRC_Calculate(&CrcHandle,
					(uint32_t *) irdata, 4 / 2);

			// CRC was correct
			if (crcReceived == IRuwCRCValue) {

				// Sender
				// https://code.google.com/p/stm32-campos/source/browse/trunk/Campos/src/irlink.c
				// irdata[0] = position_x*10 + position_subx / 100;
				// irdata[1] = position_y*10 + position_suby / 100;
				// irdata[2] = (intensity/256)*256 + (int)track_status;
				// irdata[3] = CRC Checksum;

				IRLINK_position_x = irdata[0];
				IRLINK_position_y = irdata[1];
				IRLINK_intensity = (irdata[2] >> 8) & 0x00FF;
				IRLINK_status = (en_irlink_StatusTypeDef) (irdata[2] & 0x00FF);
			}
			ir_state = IR_WAIT_IDLE;
		}
	}
}

/**
 * @brief  Returns the IR link status as text
 * @param  None
 * @retval The text to display
 */
char* IRLINK_GetStatusText(void) {

	switch (IRLINK_status) {
	case TRACK_INIT:
		return "INIT";
	case TRACK_SEARCHING:
		return "SEARCH";
	case TRACK_LIGHT_FOUND:
		return "FOUND";
	case TRACK_CENTER_DETECTED:
		return "CENTER";
	case TRACK_LOST:
		return "LOST";
	default:
		return "???";
	}

}

