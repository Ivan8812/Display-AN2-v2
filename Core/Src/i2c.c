/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SMBUS_HandleTypeDef hsmbus4;

/* I2C4 init function */

void MX_I2C4_SMBUS_Init(void)
{

  /* USER CODE BEGIN I2C4_Init 0 */

  /* USER CODE END I2C4_Init 0 */

  /* USER CODE BEGIN I2C4_Init 1 */

  /* USER CODE END I2C4_Init 1 */
  hsmbus4.Instance = I2C4;
  hsmbus4.Init.Timing = 0x307075B1;
  hsmbus4.Init.AnalogFilter = SMBUS_ANALOGFILTER_ENABLE;
  hsmbus4.Init.OwnAddress1 = 2;
  hsmbus4.Init.AddressingMode = SMBUS_ADDRESSINGMODE_7BIT;
  hsmbus4.Init.DualAddressMode = SMBUS_DUALADDRESS_DISABLE;
  hsmbus4.Init.OwnAddress2 = 0;
  hsmbus4.Init.OwnAddress2Masks = SMBUS_OA2_NOMASK;
  hsmbus4.Init.GeneralCallMode = SMBUS_GENERALCALL_DISABLE;
  hsmbus4.Init.NoStretchMode = SMBUS_NOSTRETCH_DISABLE;
  hsmbus4.Init.PacketErrorCheckMode = SMBUS_PEC_DISABLE;
  hsmbus4.Init.PeripheralMode = SMBUS_PERIPHERAL_MODE_SMBUS_SLAVE;
  hsmbus4.Init.SMBusTimeout = 0x000085B8;
  if (HAL_SMBUS_Init(&hsmbus4) != HAL_OK)
  {
    Error_Handler();
  }

  /** configuration Alert Mode
  */
  if (HAL_SMBUS_EnableAlert_IT(&hsmbus4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_SMBUS_ConfigDigitalFilter(&hsmbus4, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C4_Init 2 */

  /* USER CODE END I2C4_Init 2 */

}

void HAL_SMBUS_MspInit(SMBUS_HandleTypeDef* smbusHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(smbusHandle->Instance==I2C4)
  {
  /* USER CODE BEGIN I2C4_MspInit 0 */

  /* USER CODE END I2C4_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C4;
    PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_D3PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C4 GPIO Configuration
    PB5     ------> I2C4_SMBA
    PB6     ------> I2C4_SCL
    PB7     ------> I2C4_SDA
    */
    GPIO_InitStruct.Pin = I2C4_SMBA_Pin|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_I2C4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C4 clock enable */
    __HAL_RCC_I2C4_CLK_ENABLE();
  /* USER CODE BEGIN I2C4_MspInit 1 */

  /* USER CODE END I2C4_MspInit 1 */
  }
}

void HAL_SMBUS_MspDeInit(SMBUS_HandleTypeDef* smbusHandle)
{

  if(smbusHandle->Instance==I2C4)
  {
  /* USER CODE BEGIN I2C4_MspDeInit 0 */

  /* USER CODE END I2C4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C4_CLK_DISABLE();

    /**I2C4 GPIO Configuration
    PB5     ------> I2C4_SMBA
    PB6     ------> I2C4_SCL
    PB7     ------> I2C4_SDA
    */
    HAL_GPIO_DeInit(I2C4_SMBA_GPIO_Port, I2C4_SMBA_Pin);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C4_MspDeInit 1 */

  /* USER CODE END I2C4_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
