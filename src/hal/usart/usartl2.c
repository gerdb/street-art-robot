/**
 *  Project     StreetArtRobot
 *  @file		usartl2.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		communication layer 2
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
#include "usartl2.h"
#include "controller.h"

/* local variables ----------------------------------------------------------*/
enDecodeState decodeState;
char decodeCmd;
uint32_t decodePos;
uint32_t decodeAddress;
uint32_t decodeData;
int debug_on;

/**
 * @brief  Initialize the module
 * @param  None
 * @retval None
 */
void USARTL2_Init(void) {
	USARTL1_Init();
	decodeState = DECODE_CMD;
	decodePos = 0;
	debug_on = 0;
	my_printf("\r\n>");
}

/**
 * @brief  convert an hex character to decimal value
 *
 * @param  c the character to convert
 * @retval hex value
 */
uint32_t hex2dec(char c) {
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'z')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'Z')
		return c - 'A' + 10;
	return -1;

}

/**
 * @brief  decode the received bytes
 *
 * @param  none
 * @retval none
 */
void USARTL2_Decode(char c) {
	int x,y;

	if (c == '\r' || c == '\n') {
		decodeState = DECODE_ENTER;

	} else	if (c == 27) {
		my_printf("\r\n>");
		decodeState = DECODE_CMD;
		return;
	}

	switch (decodeState) {
	case DECODE_CMD:
		// Write a I2C address with data
		decodeCmd = c;
		if ((c == 'w') || (c == 'r')|| (c == 'c'))  {
			decodeState = DECODE_ADDRESS;
			decodePos = 0;
			decodeAddress = 0;
			decodeData = 0;
		}
		if (c == 's') {
			my_printf("\r\n");
			for (y=0; y<120; y++) {
				for (x=0; x<120; x++) {
				//	my_printf("%03d ",pixels.zoomed[y][x]);
				}
				my_printf("\r\n");
			}
			my_printf("\r\n>");
		}
		if ((c == 'p') || (c == 'i'))  {
			decodeState = DECODE_DATA;
			decodePos = 0;
			decodeData = 0;
		}

		if (c == 'd') {
			debug_on = 0;
		}
		if (c == 'D') {
			debug_on = 1;
		}
//		if ((c >= '0') && (c <= '9')) {
//			controller_pos[0] = +(c-'0')*1000/9;
//			controller_pos[1] = -(c-'0')*1000/9;
//		}

		break;
	case DECODE_ADDRESS:
		if (c == ' ') {
			decodeState = DECODE_DATA;
		} else {
			decodeAddress *= 16;
			decodeAddress += hex2dec(c);
		}
		break;
	case DECODE_DATA:
		decodeData *= 16;
		decodeData += hex2dec(c);
		break;
	case DECODE_ENTER:
		my_printf("\n");
		if (decodeAddress < 65536) {
			if (decodeData < 65536) {
				switch (decodeCmd) {
				case 'p':
					controller_kp = decodeData;
					break;
				case 'i':
					controller_ki = decodeData;
					break;
				}
			}
			else {
				my_printf("Data out of range");
			}
		}
		else {
			my_printf("Address out of range");
		}
		my_printf("\r\n>");
		decodeState = DECODE_CMD;
		break;
	default:
		decodeState = DECODE_CMD;
	}

}
