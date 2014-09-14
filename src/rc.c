/**
 *  Project     StreetArtRobot
 *  @file		rc.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Receives remote control commands
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

#include "rc.h"

/* local variables ----------------------------------------------------------*/
UART_HandleTypeDef UartRCHandle;
int RC_state = 0;
int RC_lowBat = 0;
int RC_key[3] = {0,0,0};

/* local functions ----------------------------------------------------------*/
uint8_t USART_RC_dummy_buffer[1]; // Just to feed the HAL functions
/**
 * @brief  Configures the RC module
 * @param  None
 * @retval None
 */
void RC_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable GPIO RX clock
	USART_RC_RX_GPIO_CLK_ENABLE();
	// Enable USART clock
	USART_RC_CLK_ENABLE();

	// UART RX GPIO pin configuration
	GPIO_InitStruct.Pin = USART_RC_RX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = USART_RC_RX_AF;

	HAL_GPIO_Init(USART_RC_RX_GPIO_PORT, &GPIO_InitStruct);

	UartRCHandle.Instance = USART_RC;
	UartRCHandle.Init.BaudRate = 9600;
	UartRCHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartRCHandle.Init.StopBits = UART_STOPBITS_1;
	UartRCHandle.Init.Parity = UART_PARITY_NONE;
	UartRCHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UartRCHandle.Init.Mode = UART_MODE_RX;
	HAL_UART_Init(&UartRCHandle);
	HAL_UART_Receive_IT(&UartRCHandle, (uint8_t *) USART_RC_dummy_buffer, 1);

	// NVIC for USART_RC
	HAL_NVIC_SetPriority(USART_RC_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USART_RC_IRQn);
}

/**
 * @brief  RC main task
 * @param  None
 * @retval None
 */
void RC_Task(void) {

}

/**
 * @brief  RC main task
 * @param  None
 * @retval None
 */
void RC_Receive(void) {
	uint8_t c;
	// Get the received byte
	c = (uint8_t) (UartRCHandle.Instance->DR & (uint8_t) 0x00FF);
	switch (RC_state) {
	// Receive the transmitter ID
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		// Allow only hexadecimal signs
		if (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F'))) {
			RC_state ++;
		} else
			RC_state = 0;
		break;

	// Get the key command
	case 7:
		// Allow only signes from A to O or a to o
		if ((c >= 'A') && (c <= 'o')) {

			// Low bat if lower case
			if (c >= 'a') {
				c = c-32;
				RC_lowBat = 1;
			}

			// Get the 3 keys
			if (c == 'A')
				RC_key[0] = 1;	// Δ
			if (c == 'B')
				RC_key[1] = 1;	// O
			if (c == 'D')
				RC_key[2] = 1;	// RED

			RC_state ++;
		} else
			RC_state = 0;
		break;
	case 8:
		// Terminated with CRLF
		if (c == '\r') {
			RC_state ++;
		} else {
			RC_state = 0;
		}
		break;
	case 9:
		// Terminated with CRLF
		RC_state = 0;
		break;
	}
}

/**
 * @brief  Get the state of a key
 * @param  key, the index of the key from 0 to 2
 * @retval 1 if pressed
 */
int RC_GetKey(int key) {
	int retval;

	// Get the key and reset it
	retval = RC_key[key];
	RC_key[key] = 0;

	return retval;
}

/**
 * @brief  RC interrupt handler
 * @param  None
 * @retval None
 */
void RC_USART_IRQHandler(void) {
	uint32_t tmp1 = 0, tmp2 = 0;
	UART_HandleTypeDef *huart = &UartRCHandle;

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);
	/* UART parity error interrupt occurred ------------------------------------*/
	if ((tmp1 != RESET) && (tmp2 != RESET)) {
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_PE);

		huart->ErrorCode |= HAL_UART_ERROR_PE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART frame error interrupt occurred -------------------------------------*/
	if ((tmp1 != RESET) && (tmp2 != RESET)) {
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_FE);

		huart->ErrorCode |= HAL_UART_ERROR_FE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART noise error interrupt occurred -------------------------------------*/
	if ((tmp1 != RESET) && (tmp2 != RESET)) {
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_NE);

		huart->ErrorCode |= HAL_UART_ERROR_NE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART Over-Run interrupt occurred ----------------------------------------*/
	if ((tmp1 != RESET) && (tmp2 != RESET)) {
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);
		RC_Receive();
		huart->ErrorCode |= HAL_UART_ERROR_ORE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
	/* UART in mode Receiver ---------------------------------------------------*/
	if ((tmp1 != RESET) && (tmp2 != RESET)) {
		RC_Receive();
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
	}

	if (huart->ErrorCode != HAL_UART_ERROR_NONE) {
		/* Set the UART state ready to be able to start again the process */
		huart->State = HAL_UART_STATE_READY;
	}
}
