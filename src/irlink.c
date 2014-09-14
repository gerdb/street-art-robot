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
#include "motor.h"  // Because the Timer1 is shared with the motor module
/* local variables ----------------------------------------------------------*/
uint16_t irdata[4] = { 0, 0, 0, 0 };

TIM_IC_InitTypeDef sConfigIRlink;
CRC_HandleTypeDef CrcHandle;
uint32_t IRuwCRCValue = 0;
uint16_t IRlastCaptureVal = 0;
uint16_t IRperiode = 0;


/**
 * @brief  Initialize the module and configure PWM PB5 as PWM output with 36kHz
 * @param  None
 * @retval None
 */
void IRLINK_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = IRLINK_IN_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = IRLINK_IN_AF;
	HAL_GPIO_Init(IRLINK_PORT, &GPIO_InitStruct);

	// Timer configuration
	// Timer is already configured in motor.c during the
	// initialization of the pump motor

	sConfigIRlink.ICFilter = 0;
	sConfigIRlink.ICPolarity = TIM_ICPOLARITY_RISING;
	sConfigIRlink.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIRlink.ICSelection = TIM_ICSELECTION_DIRECTTI;

	// Configure the NVIC
	HAL_NVIC_SetPriority(IRLINK_IRQn, 0, 1);

	/* Enable the TIM1 global Interrupt */
	HAL_NVIC_EnableIRQ(IRLINK_IRQn);

	// Input capture mode
	HAL_TIM_IC_ConfigChannel(&htimPump, &sConfigIRlink, TIM_CHANNEL_4);
	HAL_TIM_IC_Start_IT(&htimPump, TIM_CHANNEL_4);

	// Configure the CRC module
	CrcHandle.Instance = CRC;
	HAL_CRC_Init(&CrcHandle);

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
 * @brief The input capture service function
 * @param None
 * @retval None
 */
void IRLINK_TimerIRQ(void) {
	uint16_t captureVal;

	// Capture compare 4 event
	if (__HAL_TIM_GET_FLAG(&htimPump, TIM_FLAG_CC4) != RESET) {
		if (__HAL_TIM_GET_ITSTATUS(&htimPump, TIM_IT_CC4) != RESET) {
			__HAL_TIM_CLEAR_IT(&htimPump, TIM_IT_CC4);
			htimPump.Channel = HAL_TIM_ACTIVE_CHANNEL_4;
			// Input capture event
			if ((htimPump.Instance->CCMR2 & TIM_CCMR2_CC4S) != 0x00) {
				captureVal = HAL_TIM_ReadCapturedValue(&htimPump, TIM_CHANNEL_4);
				IRperiode = captureVal - IRlastCaptureVal;
				IRlastCaptureVal = captureVal;
			}
			htimPump.Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
		}
	}
}
