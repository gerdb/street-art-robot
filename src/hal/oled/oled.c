/**
 *  Project     StreetArtRobot
 *  @file		oled.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		oled functionality
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
#include "oled.h"
#include "i2c.h"
#include "font.h"

/* local variables ----------------------------------------------------------*/
uint8_t oled_buffer[192];

/* local functions ----------------------------------------------------------*/

/**
 * @brief  Configures the oled module interface.
 * @param  None
 * @retval None
 */
void OLED_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable the clock for the RES pin on port D8
	OLED_GPIO_RES_CLK_ENABLE();
	// Enable the clock for the enable 7volt pin on port B7
	OLED_GPIO_7VEN_CLK_ENABLE();

	// Configure the enable 7V pin
	GPIO_InitStruct.Pin = OLED_7VEN_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(OLED_GPIO_7VEN_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(OLED_GPIO_7VEN_PORT, OLED_7VEN_PIN, GPIO_PIN_RESET);

	// Configure the RES pin
	GPIO_InitStruct.Pin = OLED_RES_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(OLED_GPIO_RES_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(OLED_GPIO_RES_PORT, OLED_RES_PIN, GPIO_PIN_RESET);

	// Initialize the I2C bus
	I2C_Init();

	// Release Reset
	HAL_GPIO_WritePin(OLED_GPIO_RES_PORT, OLED_RES_PIN, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(OLED_GPIO_RES_PORT, OLED_RES_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(OLED_GPIO_RES_PORT, OLED_RES_PIN, GPIO_PIN_SET);

	// Initialize the MIDAS 96x16 display
	OLED_Command(SSD1306_DISPLAYOFF);
	OLED_Command(SSD1306_SETDISPLAYCLOCKDIV);
	OLED_Command(0x80);
	OLED_Command(SSD1306_SETMULTIPLEX);
	OLED_Command(0x0F);
	OLED_Command(SSD1306_SETDISPLAYOFFSET);
	OLED_Command(0x00);
	OLED_Command(SSD1306_SETSTARTLINE | 0x0);
	OLED_Command(SSD1306_CHARGEPUMP);
	OLED_Command(0x10);
	OLED_Command(SSD1306_MEMORYMODE);
	OLED_Command(0x00);
	OLED_Command(SSD1306_SEGREMAP | 0x1);
#ifdef OLED_FLIPPED
	OLED_Command(SSD1306_COMSCANINC);
#else
	OLED_Command(SSD1306_COMSCANDEC);
#endif
	OLED_Command(SSD1306_SETCOMPINS);
	OLED_Command(0x02);
	OLED_Command(SSD1306_SETCONTRAST);
	OLED_Command(0x10);
	OLED_Command(SSD1306_SETPRECHARGE);
	OLED_Command(0x22);
	OLED_Command(SSD1306_SETVCOMDETECT);
	OLED_Command(0x40);
	OLED_Command(SSD1306_DISPLAYALLON_RESUME);
	OLED_Command(SSD1306_NORMALDISPLAY);

	// Switch on OLED VCC
	HAL_GPIO_WritePin(OLED_GPIO_7VEN_PORT, OLED_7VEN_PIN, GPIO_PIN_SET);
	HAL_Delay(100);

	// Switch on OLED
	OLED_Command(SSD1306_DISPLAYON);

	// Draw the buffer
	OLED_Clr();
	OLED_Print( 0, 0, OLED_SIZE_SMALL, "Street");
	OLED_Print(18, 1, OLED_SIZE_SMALL, "Art");
	OLED_Print(40, 0, OLED_SIZE_LARGE, "Robot");
	OLED_Display();
}

/**
 * @brief  Clear the display buffer
 * @param  None
 * @retval None
 */
void OLED_Clr(void) {
	int i;
	// Clear the display buffer
	for (i = 0; i < 192; i++) {
		oled_buffer[i] = 0;
	}
}

/**
 * @brief  Draws a string on the OLED
 * @param  x position
 * @param  y position ( 1 = +8 pixels)
 * @param  oled_size size (8 pixel or 16 pixel height)
 * @param  text, a null terminated string
 * @retval None
 */
void OLED_Print(int x, int y, en_oled_size oled_size, char* text) {

	int charnr = 0;
	uint8_t c;
	int buffix;
	int xx;
	int yy=0;
	while (text[charnr] != '\0' && (charnr <16)) {
		c = (uint8_t)text[charnr];
		charnr ++;

		if (oled_size == OLED_SIZE_LARGE) {
			for (yy=0; yy<2; yy++) {
				for (xx=0; xx<10; xx++) {

#ifdef OLED_FLIPPED
					buffix = 95-(x + xx) + (y + yy)*96;
#else
					buffix = x + xx + (y + yy)*96;
#endif
					if (buffix<192 && buffix>=0) {
						oled_buffer[buffix] = font10x16[c][xx*2+1-yy];
					}
				}
			}
			x+=10;
		} else {
			for (xx=0; xx<6; xx++) {
#ifdef OLED_FLIPPED
				buffix = 95-(x + xx) + y*96;
#else
				buffix = x + xx + y*96;
#endif
				if (buffix<192 && buffix>=0) {
					oled_buffer[buffix] = font6x8[c][xx];
				}
			}
			x+=6;
		}


	}


}

/**
 * @brief  Send a command byte via I2C.
 * @param  c the command
 * @retval None
 */
void OLED_Command(uint8_t c) {
	I2C_WriteData(SSD1306_I2C_ADDRESS, 0x00, c);
}

/**
 * @brief  Send a data byte via I2C.
 * @param  d the data byte to send
 * @retval None
 */
void OLED_Data(uint8_t d) {
	I2C_WriteData(SSD1306_I2C_ADDRESS, 0x40, d);
}

/**
 * @brief  Send a data byte via I2C.
 * @param  d the data byte to send
 * @retval None
 */
void OLED_Display(void) {

	OLED_Command(SSD1306_COLUMNADDR);
	OLED_Command(0); // Column start address (0 = reset)
	OLED_Command(95); // Column end address (127 = reset)
	OLED_Command(SSD1306_PAGEADDR);
	OLED_Command(0); // Page start address (0 = reset)
	OLED_Command(1); // Page end address

	// Write all 192 bytes of the buffer via I2C to the OLED
	I2C_WriteDataBytes(SSD1306_I2C_ADDRESS, 0x40, oled_buffer, 192);
}
