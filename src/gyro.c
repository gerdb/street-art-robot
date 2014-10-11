/**
 *  Project     StreetArtRobot
 *  @file		gyro.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		gyro sensor
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
#include "gyro.h"


uint32_t SpixTimeout = SPIx_TIMEOUT_MAX;    /*<! Value of Timeout when SPI communication fails */

static SPI_HandleTypeDef    SpiHandle;



static void     SPIx_MspInit(void);
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
void GYRO_Init(void)
{
  if(HAL_SPI_GetState(&SpiHandle) == HAL_SPI_STATE_RESET)
  {
    /* SPI configuration -------------------------------------------------------*/
    SpiHandle.Instance = GYRO_SPIx;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase = SPI_PHASE_2EDGE;
    SpiHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;
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
uint16_t SPIx_WriteRead(uint8_t address, uint8_t data)
{
  uint8_t txbytes[2];
  uint8_t rxbytes[2];
  uint16_t retval;
  
  txbytes[0] = address;
  txbytes[1] = data;

  // Send the 2 bytes and receive 2 bytes
  HAL_GPIO_WritePin(GYRO_SPIx_CS_PORT, GYRO_SPIx_CS_PIN, RESET);
  if(HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*) &txbytes, (uint8_t*) rxbytes, 2, SpixTimeout) != HAL_OK)
  {
    SPIx_Error();
  }
  HAL_GPIO_WritePin(GYRO_SPIx_CS_PORT, GYRO_SPIx_CS_PIN, SET);
  retval = ((uint16_t)rxbytes[0]<<8) | rxbytes[1];
  return retval;
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
  //SPIx_Init();
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
  GYRO_SPIx_CLK_ENABLE();

  /* Enable CS, SCK, MOSI and MISO GPIO clocks */
  __GPIOA_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();

  /* SPI SCK, MOSI, MISO pin configuration */
  GPIO_InitStructure.Pin = (GYRO_SPIx_SCK_PIN | GYRO_SPIx_MISO_PIN | GYRO_SPIx_MOSI_PIN);
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull  = GPIO_PULLDOWN;
  GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
  GPIO_InitStructure.Alternate = GYRO_SPIx_AF;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = (GYRO_SPIx_CS_PIN);
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull  = GPIO_PULLDOWN;
  GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}



