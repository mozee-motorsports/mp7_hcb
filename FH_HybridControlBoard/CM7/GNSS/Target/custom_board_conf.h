/**
  ******************************************************************************
  * @file    custom_board_conf.h
  * @author  SRA Application Team
  * @brief   This file contains definitions for the GNSS components bus interfaces
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CUSTOM_BOARD_CONF_H
#define CUSTOM_BOARD_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "custom_bus.h"
#include "stm32_bus_ex.h"
#include "custom_errno.h"

#define USE_I2C 0U

#define USE_CUSTOM_GNSS_TESEO_LIV3F	1U

#define CUSTOM_GNSS_UART_Init        BSP_UART4_Init
#define CUSTOM_GNSS_UART_DeInit      BSP_UART4_DeInit
#define CUSTOM_GNSS_UART_Transmit_IT BSP_UART4_Send_IT
#define CUSTOM_GNSS_UART_Receive_IT  BSP_UART4_Recv_IT
#define CUSTOM_GNSS_GetTick         BSP_GetTick

#define CUSTOM_GNSS_UART_ClearOREF   BSP_UART4_ClearOREF

#define CUSTOM_RST_PORT                        GPIOF
#define CUSTOM_RST_PIN                         GPIO_PIN_14

#define CUSTOM_WAKEUP_PORT                     GPIOB
#define CUSTOM_WAKEUP_PIN                      GPIO_PIN_2

//#define CUSTOM_RegisterDefaultMspCallbacks     BSP_UART4_RegisterDefaultMspCallbacks
#define CUSTOM_RegisterRxCb                    BSP_UART4_RegisterRxCallback
#define CUSTOM_RegisterErrorCb                 BSP_UART4_RegisterErrorCallback

/* To be checked */
#define CUSTOM_UART_IRQHanlder                 BSP_UART4_IRQHanlder

#ifdef __cplusplus
}
#endif

#endif /* CUSTOM_BOARD_CONF_H */

