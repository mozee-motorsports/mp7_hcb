#include "sevcon_rx.h"
#include "sevcon_stm32.h"

HAL_StatusTypeDef SevconRx_Process(FDCAN_HandleTypeDef *bus,
                                  uint32_t fifo,
                                  uint32_t now_ms,
                                  const SevconPdoLayouts *layouts,
                                  SevconPdoFeedback *feedback)
{
    if (bus == NULL || feedback == NULL ||
        (fifo != FDCAN_RX_FIFO0 && fifo != FDCAN_RX_FIFO1) ||
        SevconPdo_ValidateLayouts(layouts) != PDO_OK) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status = HAL_OK;
    for (unsigned processed = 0; processed < 8U; ++processed) {
        if (HAL_FDCAN_GetRxFifoFillLevel(bus, fifo) == 0U) {
            break;
        }

        FDCAN_RxHeaderTypeDef header;
        uint8_t data[8];
        if (HAL_FDCAN_GetRxMessage(bus, fifo, &header, data) != HAL_OK) {
            return HAL_ERROR;
        }

        CanFrame frame;
        if (SevconStm32_RxFrame(&header, data, &frame) != SEVCON_MSG_OK) {
            status = HAL_ERROR;
            continue;
        }

        SevconTpdoSlot updated_slot;
        PdoResult result = SevconPdo_Receive(layouts, &frame, now_ms,
                                            feedback, &updated_slot);
        if (result != PDO_OK && result != PDO_NOT_MINE) {
            status = HAL_ERROR;
        }
    }
    return status;
}
