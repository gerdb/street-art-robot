/**
 *  Project     StreetArtRobot
 *  @file		main.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Projects main file
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
#include "main.h"
#include "usartl1.h"
#include "irlink.h"
#include "power.h"
#include "oled.h"
#include "servo.h"
#include "motor.h"
#include "switch.h"
#include "rc.h"
#include "irlink.h"
#include "gyro.h"
#include "sdcard.h"
#include "tm_stm32f4_fatfs.h"

/* function prototypes ------------------------------------------------------*/
void SystemClock_Config(void);
char txt[20]; // Temporary memory for strings
int blink = 0;
int mytick = 0;
/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void) {

    //Fatfs object
    FATFS FatFs;
    //File object
    FIL fil;
    //Free and total space
    uint32_t total, free;

	//int i = 12345;
	// Configure the system clock to 168 Mhz
	SystemClock_Config();

	// Initialize the hardware layer module
	HAL_Init();

	// Enable systick and configure 1ms tick
	HAL_SYSTICK_Config(168000000/ 1000);

	// Initialize the modules
	OLED_Init();	// OLED Display
	SERVO_Init();	// 2 servo motors
	MOTOR_Init();	// 3 driving motors
	SWITCH_Init();	// 5 button switches
	USARTL2_Init(); // debug port
	RC_Init(); 		// RC receiver
	POWER_Init();	// power module
	IRLINK_Init();	// IR link
	GYRO_Init();	// Gyro sensor
	SDCARD_Init();	// SD card
    //TM_DELAY_Init();


	OLED_Clr();






    //Initialize system
    //SystemInit();
    //Initialize delays


    //Mount drive
    if (f_mount(&FatFs, "", 1) == FR_OK) {


        //Try to open file
        if (f_open(&fil, "1stfile.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE) == FR_OK) {
            //File opened, turn off RED and turn on GREEN led

            //If we put more than 0 characters (everything OK)
            if (f_puts("First string in my file\n", &fil) > 0) {
                if (TM_FATFS_DriveSize(&total, &free) == FR_OK) {
                    //Data for drive size are valid
                }

            }

            //Close file, don't forget this!
            f_close(&fil);
        }

        //Unmount drive, don't forget this!
        f_mount(0, "", 1);
    }





	// Main loop
	 while (1) {

		// Generate a blink flag
		if (mytick > 200) {
			mytick = 0;
			blink = !blink;

			//200ms Task
			POWER_Task();
		}

		SWITCH_Task();

		if (SWITCH_GetClick(SWITCH_LEFT) || RC_GetKey(RC_KEY_GO)) {
			MOTOR_SetVal(MOTOR_M1, 400, 255);
		}
		if (SWITCH_GetClick(SWITCH_RIGHT) || RC_GetKey(RC_KEY_STOP)) {
			MOTOR_SetVal(MOTOR_M1, 0, 255);
		}
		if (SWITCH_GetClick(SWITCH_UP)) {
			MOTOR_SetVal(MOTOR_M1, -800, 255);
		}
		if (SWITCH_GetClick(SWITCH_DOWN) || RC_GetKey(RC_KEY_RED)) {
			MOTOR_SetVal(MOTOR_M1, 4200, 255);
		}
		// Debug ports
		USARTL1_RxBufferTask();

		//i = TIM8->CCR1;
		//sprintf (txt,"%5u", (unsigned int)POWER_vbat);
		//sprintf (txt,"%5u", (unsigned int)IRperiode);
		//sprintf (txt,"%5u", (unsigned int)GYRO_GetAngle());
		sprintf (txt,"%7u", (unsigned int)MOTOR_GetSpeed(MOTOR_M1));
		OLED_Print(0, 0, OLED_SIZE_SMALL, txt);

		sprintf (txt,"%7u", (unsigned int)MOTOR_GetSpeed(MOTOR_M2));
		OLED_Print(0, 1, OLED_SIZE_SMALL, txt);



		OLED_Display();


	}

}



