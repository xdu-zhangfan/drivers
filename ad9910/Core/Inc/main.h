/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

#include "ad9910.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define AD9910_RESET_Pin GPIO_PIN_5
#define AD9910_RESET_GPIO_Port GPIOD
#define AD9910_IOUPDATE_Pin GPIO_PIN_7
#define AD9910_IOUPDATE_GPIO_Port GPIOD
#define AD9910_DRH_Pin GPIO_PIN_3
#define AD9910_DRH_GPIO_Port GPIOB
#define AD9910_DRC_Pin GPIO_PIN_4
#define AD9910_DRC_GPIO_Port GPIOB
#define AD9910_DRO_Pin GPIO_PIN_5
#define AD9910_DRO_GPIO_Port GPIOB
#define AD9910_OSK_Pin GPIO_PIN_6
#define AD9910_OSK_GPIO_Port GPIOB
#define AD9910_PF0_Pin GPIO_PIN_7
#define AD9910_PF0_GPIO_Port GPIOB
#define AD9910_PF1_Pin GPIO_PIN_8
#define AD9910_PF1_GPIO_Port GPIOB
#define AD9910_PF2_Pin GPIO_PIN_9
#define AD9910_PF2_GPIO_Port GPIOB
#define AD9910_SYNC_Pin GPIO_PIN_0
#define AD9910_SYNC_GPIO_Port GPIOE
#define AD9910_RSO_Pin GPIO_PIN_1
#define AD9910_RSO_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
