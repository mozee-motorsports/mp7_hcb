/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#define USER_BTN_Pin GPIO_PIN_13
#define USER_BTN_GPIO_Port GPIOC
#define CAN1_STBY_Pin GPIO_PIN_5
#define CAN1_STBY_GPIO_Port GPIOC
#define LED_USER_GRN_Pin GPIO_PIN_0
#define LED_USER_GRN_GPIO_Port GPIOB
#define LED_USER_RED_Pin GPIO_PIN_14
#define LED_USER_RED_GPIO_Port GPIOB
#define CAN2_STBY_Pin GPIO_PIN_6
#define CAN2_STBY_GPIO_Port GPIOC
#define R2D_SPK_Pin GPIO_PIN_9
#define R2D_SPK_GPIO_Port GPIOC
#define R2D_BTN_OUT_Pin GPIO_PIN_8
#define R2D_BTN_OUT_GPIO_Port GPIOB
#define R2D_BTN_IN_Pin GPIO_PIN_9
#define R2D_BTN_IN_GPIO_Port GPIOB
#define LED_USER_YLW_Pin GPIO_PIN_1
#define LED_USER_YLW_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
