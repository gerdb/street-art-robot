/**
 *  Project     StreetArtRobot
 *  @file		servo.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Controls the 2 servos
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

#include "servo.h"

/* local variables ----------------------------------------------------------*/
TIM_HandleTypeDef htimServo;
TIM_OC_InitTypeDef sConfigTimServo;


/* local functions ----------------------------------------------------------*/


/**
 * @brief  Configures the timer for generating servo pulses
 * @param  None
 * @retval None
 */
void SERVO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable the clock for timer 9
	__TIM9_CLK_ENABLE();

	// Enable the clock for both servo pins
	SERVO_GPIO_CLK_ENABLE();

	// Configure the 2 servo pins
	GPIO_InitStruct.Pin = SERVO_GPIO_1_PIN | SERVO_GPIO_2_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = SERVO_GPIO_TIMER_AF;
	HAL_GPIO_Init(SERVO_GPIO_PORT, &GPIO_InitStruct);

    // Timer configuration
	htimServo.Instance = SERVO_TIMER;
	htimServo.Init.Period = 20000 - 1; // = 50Hz = 20ms = 168MHz / 168 / 20000
	htimServo.Init.Prescaler = 168-1;
	htimServo.Init.ClockDivision = 1;
	htimServo.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init(&htimServo);

    // Configure Timer 9 channel 1 as PWM output
    sConfigTimServo.OCMode = TIM_OCMODE_PWM1;
    sConfigTimServo.OCIdleState = TIM_OUTPUTSTATE_ENABLE;
    sConfigTimServo.Pulse = 0;
    sConfigTimServo.OCPolarity = TIM_OCPOLARITY_HIGH;

    // PWM Mode
    HAL_TIM_PWM_ConfigChannel(&htimServo, &sConfigTimServo, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htimServo, &sConfigTimServo, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htimServo, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htimServo, TIM_CHANNEL_2);

    SERVO_SetVal(SERVO_1, 500);
    SERVO_SetVal(SERVO_2, 500);
}

/**
 * @brief  Sets the servo with a new value
 * @param  servoNr number of the servo
 * @param  value Servo value from 0 to 1000
 * @retval None
 */
void SERVO_SetVal(int servoNr, int value) {
	if (servoNr == SERVO_1) __HAL_TIM_SetCompare(&htimServo, TIM_CHANNEL_1, value + 500);
	if (servoNr == SERVO_2) __HAL_TIM_SetCompare(&htimServo, TIM_CHANNEL_2, value + 500);
}
