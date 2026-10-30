#ifndef SEVCON_RX_H
#define SEVCON_RX_H

#include "sevcon_pdo.h"
#include "stm32h7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Drain up to eight frames per call from the selected FIFO on the SEVCON bus.
 * Peripheral must have Classical CAN / 8-byte RX elements, as in this project.
 * Keeps the original CAN ID/DLC instead of applying MP7 custom-ID conversion.
 * Invalid frames never update feedback. Other CANopen IDs are currently ignored.
 * HAL_ERROR reports bad arguments/configuration, RX failure or rejected traffic;
 * previously accepted samples remain valid observations with their own ages.
 */
HAL_StatusTypeDef SevconRx_Process(FDCAN_HandleTypeDef *bus,
                                  uint32_t fifo,
                                  uint32_t now_ms,
                                  const SevconPdoLayouts *layouts,
                                  SevconPdoFeedback *feedback);

#ifdef __cplusplus
}
#endif
#endif
