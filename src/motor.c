/**
 *  Project     StreetArtRobot
 *  @file		motor.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Controls the 2 motors and one pump
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

#include "motor.h"

/* local variables ----------------------------------------------------------*/
const uint8_t MOTOR_CURRENT_TAB[256]= {
		0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,
		2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,
		5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,8,8,
		8,9,9,9,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,
		11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,
		13,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,15,16,16,16,16,
		16,16,16,16,17,17,17,17,17,17,17,17,18,18,18,18,18,18,18,18,19,19,19,19,
		19,19,19,19,20,20,20,20,20,20,20,21,21,21,21,21,21,22,22,22,22,22,22,23,
		23,23,23,23,24,24,24,24,24,25,25,25,25,26,26,26,27,27,27,28,28,29,29,31
};

TIM_HandleTypeDef htimMotor;
TIM_HandleTypeDef htimPump;
TIM_HandleTypeDef htimEncM1;
TIM_HandleTypeDef htimEncM2;
TIM_HandleTypeDef htimEncSpeed;
TIM_Encoder_InitTypeDef sConfigEncM;
TIM_IC_InitTypeDef sConfigEncSpeed;
TIM_OC_InitTypeDef sConfigTimMotor;
TIM_OC_InitTypeDef sConfigTimPump;

uint16_t motorHallPeriode[4] = {0,0,0,0};
uint16_t motorHallLastCapVal[4] = {0,0,0,0};

/* Timer handler declaration */
TIM_HandleTypeDef    TimHandle;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sConfig;

/* local functions ----------------------------------------------------------*/


/**
 * @brief  Configures IOs to control the two motors and one pump
 * @param  None
 * @retval None
 */
void MOTOR_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable all timers
	PUMP_PWM_TIMER_CLK_ENABLE();
	MOTOR_PWM_TIMER_CLK_ENABLE();
	MOTOR_HALL_ENC1_TIMER_CLK_ENABLE();
	MOTOR_HALL_ENC2_TIMER_CLK_ENABLE();
	MOTOR_HALL_SPEED_TIMER_CLK_ENABLE();


	// Enable the clock for all IO pins
	MOTOR_PWM_CLK_ENABLE();
	MOTOR_CURR_CLK_ENABLE();
	MOTOR_HALL_M1_ENC_CLK_ENABLE();
	MOTOR_HALL_M2_ENC_CLK_ENABLE();
	MOTOR_HALL_SPEED_CLK_ENABLE();

	// Motor driver --------------------------------------------------------

	// Configure the 4 motor pins of motor 1 and 2 as normal IO
	GPIO_InitStruct.Pin = MOTOR_M2_IN1_PIN | MOTOR_M2_IN2_PIN
						| MOTOR_M1_IN1_PIN| MOTOR_M1_IN2_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = MOTOR_PWM_TIMER_AF;
	HAL_GPIO_Init(MOTOR_PWM_PORT, &GPIO_InitStruct);

	// Configure the 2 motor pins of motor 3 as PWM output
	GPIO_InitStruct.Pin = MOTOR_M3_IN1_PIN | MOTOR_M3_IN2_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = PUMP_PWM_TIMER_AF;
	HAL_GPIO_Init(MOTOR_PWM_PORT, &GPIO_InitStruct);

	// Configure the 10 motor current pins
	GPIO_InitStruct.Pin = MOTOR_M2_I0_PIN | MOTOR_M2_I1_PIN | MOTOR_M2_I2_PIN
						| MOTOR_M2_I3_PIN | MOTOR_M2_I4_PIN
						| MOTOR_M1_I0_PIN | MOTOR_M1_I1_PIN | MOTOR_M1_I2_PIN
						| MOTOR_M1_I3_PIN | MOTOR_M1_I4_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(MOTOR_CURR_PORT, &GPIO_InitStruct);

    // Timer configuration for motor pwm
	htimMotor.Instance = MOTOR_PWM_TIMER;
	htimMotor.Init.Period = 4200 - 1; // = 20kHz = 84MHz / 1 / 4200
	htimMotor.Init.Prescaler = 1-1;
	htimMotor.Init.ClockDivision = 1;
	htimMotor.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init(&htimMotor);

    // Timer configuration for pump
	htimPump.Instance = PUMP_PWM_TIMER;
	htimPump.Init.Period = 420 - 1; // = 20kHz = 168MHz / 2 / 42000
	htimPump.Init.Prescaler = 20-1;
	htimPump.Init.ClockDivision = 1;
	htimPump.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init(&htimPump);

    // Configure Timer 1 channel 1 and 2 as PWM output
    sConfigTimMotor.OCMode = TIM_OCMODE_PWM1;
    sConfigTimMotor.Pulse = 0;
    sConfigTimMotor.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigTimMotor.OCFastMode  = TIM_OCFAST_ENABLE;
    sConfigTimMotor.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigTimMotor.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigTimMotor.OCNIdleState= TIM_OCNIDLESTATE_SET;

    // Configure Timer 1 channel 1 as PWM output
    sConfigTimPump.OCMode = TIM_OCMODE_PWM1;
    sConfigTimPump.Pulse = 0;
    sConfigTimPump.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigTimPump.OCFastMode  = TIM_OCFAST_ENABLE;
    sConfigTimPump.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigTimPump.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigTimPump.OCNIdleState= TIM_OCNIDLESTATE_SET;

    // PWM Mode
    HAL_TIM_PWM_ConfigChannel(&htimMotor, &sConfigTimMotor, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htimMotor, &sConfigTimMotor, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&htimMotor, &sConfigTimMotor, TIM_CHANNEL_3);
    HAL_TIM_PWM_ConfigChannel(&htimMotor, &sConfigTimMotor, TIM_CHANNEL_4);
    HAL_TIM_PWM_ConfigChannel(&htimPump, &sConfigTimPump, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htimPump, &sConfigTimPump, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htimMotor, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htimMotor, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htimMotor, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htimMotor, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htimPump, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htimPump, TIM_CHANNEL_2);

    MOTOR_SetVal(MOTOR_M1, 0, 0);
    MOTOR_SetVal(MOTOR_M2, 0 , 0);
    MOTOR_SetVal(MOTOR_PUMP, 0, 0);

    // Encoder ----------------------------------------------------------

	// Configure the hall encoder pins
	GPIO_InitStruct.Pin = MOTOR_HALL_M1A_ENC_PIN | MOTOR_HALL_M1B_ENC_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = MOTOR_HALL_ENC1_TIMER_AF;
	HAL_GPIO_Init(MOTOR_HALL_M1_ENC_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = MOTOR_HALL_M2A_ENC_PIN | MOTOR_HALL_M2B_ENC_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = MOTOR_HALL_ENC2_TIMER_AF;
	HAL_GPIO_Init(MOTOR_HALL_M2_ENC_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = MOTOR_HALL_M1A_SPEED_PIN | MOTOR_HALL_M1B_SPEED_PIN
						| MOTOR_HALL_M2A_SPEED_PIN | MOTOR_HALL_M2B_SPEED_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = MOTOR_HALL_SPEED_TIMER_AF;
	HAL_GPIO_Init(MOTOR_HALL_SPEED_PORT, &GPIO_InitStruct);

    // Timer configuration for hall encoder M1
	htimEncM1.Instance = MOTOR_HALL_ENC1_TIMER;
	htimEncM1.Init.Period = 0xFFFF;
	htimEncM1.Init.Prescaler = 0;
	htimEncM1.Init.ClockDivision = 0;
	htimEncM1.Init.CounterMode = TIM_COUNTERMODE_UP;

	sConfigEncM.EncoderMode = TIM_ENCODERMODE_TI2;
	sConfigEncM.IC1Filter = 0;
	sConfigEncM.IC1Polarity = TIM_ICPOLARITY_RISING;
	sConfigEncM.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfigEncM.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfigEncM.IC2Filter = 0;
	sConfigEncM.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfigEncM.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfigEncM.IC2Selection = TIM_ICSELECTION_DIRECTTI;

    // Encoder Mode
	HAL_TIM_Encoder_Init(&htimEncM1, &sConfigEncM);
    HAL_TIM_Encoder_Start(&htimEncM1, TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start(&htimEncM1, TIM_CHANNEL_2);

    // Timer configuration for hall encoder M2
	htimEncM2.Instance = MOTOR_HALL_ENC2_TIMER;
	htimEncM2.Init.Period = 0xFFFF;
	htimEncM2.Init.Prescaler = 0;
	htimEncM2.Init.ClockDivision = 0;
	htimEncM2.Init.CounterMode = TIM_COUNTERMODE_UP;

    // Encoder mode
	HAL_TIM_Encoder_Init(&htimEncM2, &sConfigEncM);
    HAL_TIM_Encoder_Start(&htimEncM2, TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start(&htimEncM2, TIM_CHANNEL_2);


    // Timer configuration for input capture
    htimEncSpeed.Instance = MOTOR_HALL_SPEED_TIMER;
    htimEncSpeed.Init.Period = 0xFFFF;
    htimEncSpeed.Init.Prescaler = 840-1; // 10us
    htimEncSpeed.Init.ClockDivision = 0;
    htimEncSpeed.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_IC_Init(&htimEncSpeed);


    sConfigEncSpeed.ICFilter = 0;
    sConfigEncSpeed.ICPolarity = TIM_ICPOLARITY_RISING;
    sConfigEncSpeed.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigEncSpeed.ICSelection = TIM_ICSELECTION_DIRECTTI;

    // Configure the NVIC
    HAL_NVIC_SetPriority(TIM_HALL_SPEED_IRQn, 0, 1);

    /* Enable the TIM8 global Interrupt */
    HAL_NVIC_EnableIRQ(TIM_HALL_SPEED_IRQn);

    // Input capture mode
    HAL_TIM_IC_ConfigChannel(&htimEncSpeed, &sConfigEncSpeed, TIM_CHANNEL_1);
    HAL_TIM_IC_ConfigChannel(&htimEncSpeed, &sConfigEncSpeed, TIM_CHANNEL_2);
    HAL_TIM_IC_ConfigChannel(&htimEncSpeed, &sConfigEncSpeed, TIM_CHANNEL_3);
    HAL_TIM_IC_ConfigChannel(&htimEncSpeed, &sConfigEncSpeed, TIM_CHANNEL_4);
    HAL_TIM_IC_Start_IT(&htimEncSpeed, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htimEncSpeed, TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(&htimEncSpeed, TIM_CHANNEL_3);
    HAL_TIM_IC_Start_IT(&htimEncSpeed, TIM_CHANNEL_4);

}

/**
 * @brief  Sets the motor with a new value
 * @param  motorNr number of the motor
 * @param  value motor value from -255 to +255
 * @retval None
 */
void MOTOR_SetVal(int motorNr, int value, uint8_t current) {
	uint32_t unsignedValue;
	uint32_t channelLeft, channelRight;
	TIM_HandleTypeDef htim;
	int sign;

	// get absolute value and sign
	if (value > 0) {
		unsignedValue = value;
		sign = 1;
	} else if (value < 0) {
		unsignedValue = -value;
		sign = -1;
	} else {
		unsignedValue = 0;
		sign = 0;
	}

	// Set current and the references
	if (motorNr == MOTOR_M1) {
		MOTOR_CURR_PORT->ODR = (MOTOR_CURR_PORT->ODR & (~MOTOR_M1_MASK)) |  (MOTOR_CURRENT_TAB[current] << MOTOR_M1_SHIFT);
		htim = htimMotor;
		channelLeft  = TIM_CHANNEL_3;
		channelRight = TIM_CHANNEL_4;
	} else if (motorNr == MOTOR_M2) {
		MOTOR_CURR_PORT->ODR = (MOTOR_CURR_PORT->ODR & (~MOTOR_M2_MASK)) |  (MOTOR_CURRENT_TAB[current] << MOTOR_M2_SHIFT);
		htim = htimMotor;
		channelLeft  = TIM_CHANNEL_1;
		channelRight = TIM_CHANNEL_2;
	} else if (motorNr == MOTOR_PUMP) {
		htim = htimPump;
		channelLeft  = TIM_CHANNEL_1;
		channelRight = TIM_CHANNEL_2;
	}

	// Set the PWM
	if (sign == 1) {
		__HAL_TIM_SetCompare(&htim, channelLeft, unsignedValue);
		__HAL_TIM_SetCompare(&htim, channelRight, 0);
	} else if (sign == -1) {
		__HAL_TIM_SetCompare(&htim, channelLeft, 0);
		__HAL_TIM_SetCompare(&htim, channelRight, unsignedValue);
	} else {
		__HAL_TIM_SetCompare(&htim, channelLeft, 0);
		__HAL_TIM_SetCompare(&htim, channelRight, 0);
	}


}


/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	uint16_t captureVal;
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
		captureVal = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		motorHallPeriode[0] = captureVal - motorHallLastCapVal[0];
		motorHallLastCapVal[0] = captureVal;
	}
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
		captureVal = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
		motorHallPeriode[1] = captureVal - motorHallLastCapVal[1];
		motorHallLastCapVal[1] = captureVal;
	}
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
		captureVal = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
		motorHallPeriode[2] = captureVal - motorHallLastCapVal[2];
		motorHallLastCapVal[2] = captureVal;
	}
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
		captureVal = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
		motorHallPeriode[3] = captureVal - motorHallLastCapVal[3];
		motorHallLastCapVal[3] = captureVal;
	}

}
