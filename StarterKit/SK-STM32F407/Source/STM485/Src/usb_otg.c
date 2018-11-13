/**
  ******************************************************************************
  * File Name          : USB_OTG.c
  * Description        : This file provides code for the configuration
  *                      of the USB_OTG instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_otg.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

HCD_HandleTypeDef hhcd_USB_OTG_HS;

/* USB_OTG_FS init function */

void MX_USB_OTG_FS_USB_Init(void)
{

}
/* USB_OTG_HS init function */

void MX_USB_OTG_HS_HCD_Init(void)
{

  hhcd_USB_OTG_HS.Instance = USB_OTG_HS;
  hhcd_USB_OTG_HS.Init.dev_endpoints = 6;
  hhcd_USB_OTG_HS.Init.Host_channels = 12;
  hhcd_USB_OTG_HS.Init.speed = HCD_SPEED_FULL;
  hhcd_USB_OTG_HS.Init.dma_enable = DISABLE;
  hhcd_USB_OTG_HS.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
  hhcd_USB_OTG_HS.Init.Sof_enable = DISABLE;
  hhcd_USB_OTG_HS.Init.low_power_enable = DISABLE;
  hhcd_USB_OTG_HS.Init.vbus_sensing_enable = DISABLE;
  hhcd_USB_OTG_HS.Init.use_external_vbus = DISABLE;
  if (HAL_HCD_Init(&hhcd_USB_OTG_HS) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_HCD_MspInit(HCD_HandleTypeDef* hcdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hcdHandle->Instance==USB_OTG_HS)
  {
  /* USER CODE BEGIN USB_OTG_HS_MspInit 0 */

  /* USER CODE END USB_OTG_HS_MspInit 0 */
  
    /**USB_OTG_HS GPIO Configuration    
    PB14     ------> USB_OTG_HS_DM
    PB15     ------> USB_OTG_HS_DP 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_OTG_HS_FS;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
  /* USER CODE BEGIN USB_OTG_HS_MspInit 1 */

  /* USER CODE END USB_OTG_HS_MspInit 1 */
  }
}

void HAL_HCD_MspDeInit(HCD_HandleTypeDef* hcdHandle)
{

  if(hcdHandle->Instance==USB_OTG_HS)
  {
  /* USER CODE BEGIN USB_OTG_HS_MspDeInit 0 */

  /* USER CODE END USB_OTG_HS_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
  
    /**USB_OTG_HS GPIO Configuration    
    PB14     ------> USB_OTG_HS_DM
    PB15     ------> USB_OTG_HS_DP 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14|GPIO_PIN_15);

  }
  /* USER CODE BEGIN USB_OTG_HS_MspDeInit 1 */

  /* USER CODE END USB_OTG_HS_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
