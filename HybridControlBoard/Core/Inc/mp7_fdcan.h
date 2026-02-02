/*
 * mp7_fdcan.h
 *
 *  Library for setting up FDCAN for mp7 projects
 *
 *  Last Modified: Jan 21, 2026
 *      By Author: gundlachj
 *
 *  Created on: Jan 3, 2026
 *      Author: gundlachj
 */

#ifndef SRC_MP7_FDCAN_H_
#define SRC_MP7_FDCAN_H_

#include <stdint.h>

#if __has_include("stm32g4xx_hal.h")
#include "stm32g4xx_hal.h"
#elif __has_include("stm32h7xx_hal.h")
#include "stm32h7xx_hal.h"
#else
#error "HAL Library Files Not Found"
#endif

HAL_StatusTypeDef MP7_FDCAN_ConfigureGlobalFilter(FDCAN_HandleTypeDef *hfdcan);

HAL_StatusTypeDef MP7_FDCAN_ConfigureFilter(FDCAN_HandleTypeDef *hfdcan,
                                            uint32_t FilterIDRangeLow,
                                            uint32_t FilterIDRangeHigh,
                                            uint32_t FilterIndex,
                                            uint32_t FilterDestinationConfig);

uint8_t MP7_FDCAN_ConfigureTxHeader(FDCAN_TxHeaderTypeDef *TxHeader,
                                 uint32_t Identifier,
                                 uint32_t DataLength);


#endif /* SRC_MP7_FDCAN_H_ */
