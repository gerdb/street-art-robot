/**
 *  Project     StreetArtRobot
 *  @file		stm32f4_board.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		board specific function
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
/* Includes ------------------------------------------------------------------*/
#include "stm32f4_board.h"


uint32_t SpixTimeout = SPIx_TIMEOUT_MAX;    /*<! Value of Timeout when SPI communication fails */

static SPI_HandleTypeDef    SpiHandle;


static void     SPIx_Init(void);
static void     SPIx_MspInit(void);
static uint8_t  SPIx_WriteRead(uint8_t Byte);
static  void    SPIx_Error(void);


/******************************************************************************
                            BUS OPERATIONS
*******************************************************************************/

/******************************* SPI Routines**********************************/
/**
  * @brief  Configures SPI interface.
  * @param  None
  * @retval None
  */
static void SPIx_Init(void)
{
  if(HAL_SPI_GetState(&SpiHandle) == HAL_SPI_STATE_RESET)
  {
    /* SPI configuration -------------------------------------------------------*/
    SpiHandle.Instance = OLED_SPIx;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
    SpiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    SpiHandle.Init.CRCPolynomial = 7;
    SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS = SPI_NSS_SOFT;
    SpiHandle.Init.TIMode = SPI_TIMODE_DISABLED;
    SpiHandle.Init.Mode = SPI_MODE_MASTER;

    SPIx_MspInit();
    HAL_SPI_Init(&SpiHandle);
  }
}

/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received 
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
static uint8_t SPIx_WriteRead(uint8_t Byte)
{
  uint8_t receivedbyte = 0;
  
  /* Send a Byte through the SPI peripheral */
  /* Read byte from the SPI bus */
  if(HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*) &Byte, (uint8_t*) &receivedbyte, 1, SpixTimeout) != HAL_OK)
  {
    SPIx_Error();
  }
  
  return receivedbyte;
}

/**
  * @brief Bus error user callback function
  * @param None
  * @retval None
  */
static  void SPIx_Error(void)
{
  /* De-initialize the SPI comunication bus */
  HAL_SPI_DeInit(&SpiHandle);
  
  /* Re-Initiaize the SPI comunication bus */
  SPIx_Init();
}


/**
  * @brief SPI MSP Init
  * @param hspi: SPI handle
  * @retval None
  */
static void SPIx_MspInit(void)
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable the SPI periph */
  OLED_SPIx_CLK_ENABLE();
  
  /* Enable SCK, MOSI and MISO GPIO clocks */
  OLED_SPIx_GPIO_CLK_ENABLE();
  
  /* SPI SCK, MOSI, MISO pin configuration */
  GPIO_InitStructure.Pin = (OLED_SPIx_SCK_PIN | OLED_SPIx_MISO_PIN | OLED_SPIx_MOSI_PIN);
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull  = GPIO_PULLDOWN;
  GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
  GPIO_InitStructure.Alternate = OLED_SPIx_AF;
  HAL_GPIO_Init(OLED_SPIx_GPIO_PORT, &GPIO_InitStructure);
}



