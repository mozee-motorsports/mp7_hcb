/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "resmgr_utility.h"

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
#define Wheel_Speed_Pin GPIO_PIN_5
#define Wheel_Speed_GPIO_Port GPIOE
#define Temp_2_Pin GPIO_PIN_6
#define Temp_2_GPIO_Port GPIOF
#define Oil_Pressure_Pin GPIO_PIN_7
#define Oil_Pressure_GPIO_Port GPIOF
#define CEL_Pin GPIO_PIN_8
#define CEL_GPIO_Port GPIOF
#define Temp_1_Pin GPIO_PIN_9
#define Temp_1_GPIO_Port GPIOF
#define Clutch_Sensor_Pin GPIO_PIN_0
#define Clutch_Sensor_GPIO_Port GPIOC
#define Brake_Sensor_Pin GPIO_PIN_1
#define Brake_Sensor_GPIO_Port GPIOC
#define GPS_UART_TX_Pin GPIO_PIN_0
#define GPS_UART_TX_GPIO_Port GPIOA
#define GPS_UART_RX_Pin GPIO_PIN_1
#define GPS_UART_RX_GPIO_Port GPIOA
#define RPM_sensor_Pin GPIO_PIN_2
#define RPM_sensor_GPIO_Port GPIOA
#define Amb_temp_Pin GPIO_PIN_7
#define Amb_temp_GPIO_Port GPIOA
#define LV_ADC_Pin GPIO_PIN_4
#define LV_ADC_GPIO_Port GPIOC
#define ADC5V1_Pin GPIO_PIN_5
#define ADC5V1_GPIO_Port GPIOC
#define ADC5V2_Pin GPIO_PIN_0
#define ADC5V2_GPIO_Port GPIOB
#define ADC5V3_Pin GPIO_PIN_1
#define ADC5V3_GPIO_Port GPIOB
#define GPS_Wake_Pin GPIO_PIN_2
#define GPS_Wake_GPIO_Port GPIOB
#define ADC12V_Pin GPIO_PIN_11
#define ADC12V_GPIO_Port GPIOF
#define GPS_Resetn_Pin GPIO_PIN_14
#define GPS_Resetn_GPIO_Port GPIOF
#define HC1_LED_Pin GPIO_PIN_15
#define HC1_LED_GPIO_Port GPIOF
#define HC_Switch_Pin GPIO_PIN_7
#define HC_Switch_GPIO_Port GPIOE
#define HC2_LED_Pin GPIO_PIN_8
#define HC2_LED_GPIO_Port GPIOE
#define R2D_Switch_Pin GPIO_PIN_9
#define R2D_Switch_GPIO_Port GPIOE
#define R2D_LED_Pin GPIO_PIN_10
#define R2D_LED_GPIO_Port GPIOE
#define Starter_Switch_Pin GPIO_PIN_11
#define Starter_Switch_GPIO_Port GPIOE
#define FP_Switch_Pin GPIO_PIN_12
#define FP_Switch_GPIO_Port GPIOE
#define Ignition_Switch_Pin GPIO_PIN_13
#define Ignition_Switch_GPIO_Port GPIOE
#define User_Switch_Pin GPIO_PIN_14
#define User_Switch_GPIO_Port GPIOE
#define Disp_EN_Pin GPIO_PIN_15
#define Disp_EN_GPIO_Port GPIOE
#define Disp_I2C_SCL_Pin GPIO_PIN_10
#define Disp_I2C_SCL_GPIO_Port GPIOB
#define Disp_I2C_SDA_Pin GPIO_PIN_11
#define Disp_I2C_SDA_GPIO_Port GPIOB
#define VCP_TX_Pin GPIO_PIN_8
#define VCP_TX_GPIO_Port GPIOD
#define VCP_RX_Pin GPIO_PIN_9
#define VCP_RX_GPIO_Port GPIOD
#define Shift_Up_Pin GPIO_PIN_13
#define Shift_Up_GPIO_Port GPIOD
#define Shift_Down_Pin GPIO_PIN_14
#define Shift_Down_GPIO_Port GPIOD
#define FP_Out_Pin GPIO_PIN_15
#define FP_Out_GPIO_Port GPIOD
#define Ignition_Out_Pin GPIO_PIN_6
#define Ignition_Out_GPIO_Port GPIOG
#define Detect_SDIO_Pin GPIO_PIN_10
#define Detect_SDIO_GPIO_Port GPIOA
#define FET4_Pin GPIO_PIN_4
#define FET4_GPIO_Port GPIOD
#define FAN1_Pin GPIO_PIN_5
#define FAN1_GPIO_Port GPIOD
#define FET2_Pin GPIO_PIN_6
#define FET2_GPIO_Port GPIOD
#define FAN2_Pin GPIO_PIN_7
#define FAN2_GPIO_Port GPIOD
#define FET1_Pin GPIO_PIN_10
#define FET1_GPIO_Port GPIOG
#define FET3_Pin GPIO_PIN_12
#define FET3_GPIO_Port GPIOG
#define CAN_Error_Pin GPIO_PIN_13
#define CAN_Error_GPIO_Port GPIOG
#define Acc_Interupt_Pin GPIO_PIN_5
#define Acc_Interupt_GPIO_Port GPIOB
#define Acc_I2C_SCL_Pin GPIO_PIN_6
#define Acc_I2C_SCL_GPIO_Port GPIOB
#define Acc_I2C_SDA_Pin GPIO_PIN_7
#define Acc_I2C_SDA_GPIO_Port GPIOB
#define Starter_Out_Pin GPIO_PIN_9
#define Starter_Out_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
