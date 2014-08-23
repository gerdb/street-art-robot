/**
 *  Project     StreetArtRobot
 *  @file		oled.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for oled.c
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
#ifndef __OLED_H
#define __OLED_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* defines ------------------------------------------------------------------*/
#define OLED_GPIO_RES_CLK_ENABLE()  __GPIOD_CLK_ENABLE()
#define OLED_RES_PIN      		GPIO_PIN_8
#define OLED_GPIO_RES_PORT      GPIOD

#define OLED_GPIO_7VEN_CLK_ENABLE()  __GPIOB_CLK_ENABLE()
#define OLED_7VEN_PIN      		GPIO_PIN_0
#define OLED_GPIO_7VEN_PORT     GPIOB

#define SSD1306_SETLOWCOLUMN 		0x00
#define SSD1306_EXTERNALVCC 		0x01
#define SSD1306_SWITCHCAPVCC 		0x02
#define SSD1306_SETHIGHCOLUMN 		0x10
#define SSD1306_MEMORYMODE 			0x20
#define SSD1306_COLUMNADDR 			0x21
#define SSD1306_PAGEADDR 			0x22
#define SSD1306_SETSTARTLINE 		0x40
#define SSD1306_SETCONTRAST 		0x81
#define SSD1306_CHARGEPUMP 			0x8D
#define SSD1306_SEGREMAP 			0xA0
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 		0xA5
#define SSD1306_NORMALDISPLAY 		0xA6
#define SSD1306_INVERTDISPLAY 		0xA7
#define SSD1306_SETMULTIPLEX 		0xA8
#define SSD1306_DISPLAYOFF 			0xAE
#define SSD1306_DISPLAYON 			0xAF
#define SSD1306_COMSCANINC 			0xC0
#define SSD1306_COMSCANDEC 			0xC8
#define SSD1306_SETDISPLAYOFFSET 	0xD3
#define SSD1306_SETCOMPINS 			0xDA
#define SSD1306_SETVCOMDETECT 		0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 	0xD5
#define SSD1306_SETPRECHARGE 		0xD9

#define SSD1306_I2C_ADDRESS 		0b01111000

#define OLED_FLIPPED

typedef enum {
	OLED_SIZE_SMALL,
	OLED_SIZE_LARGE
} en_oled_size;

/* Function Prototypes --------------------------------------------------------*/
void OLED_Init(void);
void OLED_Command(uint8_t c);
void OLED_Data(uint8_t d);
void OLED_Clr(void);
void OLED_Display(void);
void OLED_Print(int x, int y, en_oled_size oled_size, char* text);

#endif /* __OLED_H */
