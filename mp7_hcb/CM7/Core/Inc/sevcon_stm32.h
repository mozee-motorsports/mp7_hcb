#ifndef SEVCON_STM32_H
#define SEVCON_STM32_H
#include "sevcon_messages.h"
#include "stm32h7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Header conversions only. These functions do not access the peripheral.
 * Receive into an 8-byte buffer only on a peripheral configured for Classical
 * CAN / 8-byte RX elements, as in this project. Reject other frame types here.
 */
SevconMessageResult SevconStm32_TxHeader(const SevconCanFrame *frame,
                                       FDCAN_TxHeaderTypeDef *out);
SevconMessageResult SevconStm32_RxFrame(const FDCAN_RxHeaderTypeDef *header,
                                      const uint8_t data[8], SevconCanFrame *out);
#ifdef __cplusplus
}
#endif
#endif
