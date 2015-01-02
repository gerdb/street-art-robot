/**
 *  Project     StreetArtRobot
 *  @file		motor.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for motor.c
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
#ifndef __MOTOR_H
#define __MOTOR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* defines ------------------------------------------------------------------*/
//#define MOTOR_MEASURE_SPEED
#define MOTOR_MAX 4200

// Clocks
#define MOTOR_PWM_CLK_ENABLE()			__GPIOA_CLK_ENABLE()
#define MOTOR_CURR_CLK_ENABLE()			__GPIOE_CLK_ENABLE()
#define MOTOR_HALL_M1_ENC_CLK_ENABLE()	__GPIOA_CLK_ENABLE()
#define MOTOR_HALL_M2_ENC_CLK_ENABLE()	__GPIOD_CLK_ENABLE()
#define MOTOR_HALL_SPEED_CLK_ENABLE()	__GPIOC_CLK_ENABLE()
// Pins
#define MOTOR_M2_IN1_PIN				GPIO_PIN_0
#define MOTOR_M2_IN2_PIN				GPIO_PIN_1
#define MOTOR_M1_IN1_PIN				GPIO_PIN_2
#define MOTOR_M1_IN2_PIN				GPIO_PIN_3
#define MOTOR_M3_IN1_PIN				GPIO_PIN_8
#define MOTOR_M3_IN2_PIN				GPIO_PIN_9
#define MOTOR_M2_I0_PIN					GPIO_PIN_0
#define MOTOR_M2_I1_PIN					GPIO_PIN_1
#define MOTOR_M2_I2_PIN					GPIO_PIN_2
#define MOTOR_M2_I3_PIN					GPIO_PIN_3
#define MOTOR_M2_I4_PIN					GPIO_PIN_4
#define MOTOR_M1_I0_PIN					GPIO_PIN_7
#define MOTOR_M1_I1_PIN					GPIO_PIN_8
#define MOTOR_M1_I2_PIN					GPIO_PIN_9
#define MOTOR_M1_I3_PIN					GPIO_PIN_10
#define MOTOR_M1_I4_PIN					GPIO_PIN_11
#define MOTOR_HALL_M1A_ENC_PIN			GPIO_PIN_6
#define MOTOR_HALL_M1B_ENC_PIN			GPIO_PIN_7
#define MOTOR_HALL_M2A_ENC_PIN			GPIO_PIN_12
#define MOTOR_HALL_M2B_ENC_PIN			GPIO_PIN_13
#define MOTOR_HALL_M1A_SPEED_PIN		GPIO_PIN_6
#define MOTOR_HALL_M1B_SPEED_PIN		GPIO_PIN_7
#define MOTOR_HALL_M2A_SPEED_PIN		GPIO_PIN_8
#define MOTOR_HALL_M2B_SPEED_PIN		GPIO_PIN_9
// Port
#define MOTOR_PWM_PORT					GPIOA
#define MOTOR_CURR_PORT					GPIOE
#define MOTOR_HALL_M1_ENC_PORT			GPIOA
#define MOTOR_HALL_M2_ENC_PORT			GPIOD
#define MOTOR_HALL_SPEED_PORT			GPIOC
// Bit shift
#define MOTOR_M2_SHIFT					0
#define MOTOR_M1_SHIFT					7
#define MOTOR_M2_MASK					(0x1F << MOTOR_M2_SHIFT)
#define MOTOR_M1_MASK					(0x1F << MOTOR_M1_SHIFT)
// Alternate functions
#define PUMP_PWM_TIMER_AF				GPIO_AF1_TIM1
#define MOTOR_PWM_TIMER_AF				GPIO_AF2_TIM5
#define MOTOR_HALL_ENC1_TIMER_AF		GPIO_AF2_TIM3
#define MOTOR_HALL_ENC2_TIMER_AF		GPIO_AF2_TIM4
#define MOTOR_HALL_SPEED_TIMER_AF		GPIO_AF3_TIM8
// Timer
#define PUMP_PWM_TIMER					TIM1
#define MOTOR_PWM_TIMER					TIM5
#define MOTOR_HALL_ENC1_TIMER			TIM3
#define MOTOR_HALL_ENC2_TIMER			TIM4
#define MOTOR_HALL_SPEED_TIMER			TIM8
//CLK
#define PUMP_PWM_TIMER_CLK_ENABLE()					__TIM1_CLK_ENABLE()
#define MOTOR_PWM_TIMER_CLK_ENABLE()				__TIM5_CLK_ENABLE()
#define MOTOR_HALL_ENC1_TIMER_CLK_ENABLE()			__TIM3_CLK_ENABLE()
#define MOTOR_HALL_ENC2_TIMER_CLK_ENABLE()			__TIM4_CLK_ENABLE()
#define MOTOR_HALL_SPEED_TIMER_CLK_ENABLE()			__TIM8_CLK_ENABLE()
//IRQ
#define TIM_HALL_SPEED_IRQn                      TIM8_CC_IRQn
#define TIM_HALL_SPEED_IRQHandler                TIM8_CC_IRQHandler

#define MOTOR_M1	1
#define MOTOR_M2 	2
#define MOTOR_M3 	3
#define MOTOR_PUMP 	MOTOR_M3

/* global variables --------------------------------------------------------*/
#ifdef MOTOR_MEASURE_SPEED
extern TIM_HandleTypeDef htimEncSpeed;
#endif
extern TIM_HandleTypeDef htimPump; // Timer1 is shared together with the IRLINK module

/* Function Prototypes --------------------------------------------------------*/
void MOTOR_Init(void);
void MOTOR_SetVal(int motorNr, int value, uint8_t current);
void MOTOR_1msTask(void);
#ifdef MOTOR_MEASURE_SPEED
int MOTOR_GetSpeed(int motorNr);
#endif

#endif // __MOTOR_H
