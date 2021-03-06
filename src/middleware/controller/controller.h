/**
 *  Project     StreetArtRobot
 *  @file		controller.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for controller.c
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
#ifndef __CONTROLLER_H
#define __CONTROLLER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* defines ------------------------------------------------------------------*/
#define CONTROLLER_MAX_DIFF 100
#define CONTROLLER_YAW_MAX 100
//#define CONTROLLER_YAW_PID


/* global variables ------------------------------------------------------------------*/
extern int controller_angle_kp;
#ifdef CONTROLLER_YAW_PID
extern int controller_angle_ki;
extern int controller_angle_kd;
#endif

/* Function Prototypes --------------------------------------------------------*/
void CONTROLLER_Init(void);
void CONTROLLER_1msTask(void);
void CONTROLLER_Reset(void);
void CONTROLLER_Enable(int enabled);

#endif // CONTROLLER_H
