#include "sevcon_stm32.h"
#include <stddef.h>
#include <string.h>

/* Lookup, rather than treating HAL's encoded DLC as an ordinary byte count. */
static const uint32_t dlc[9] = {
    FDCAN_DLC_BYTES_0, FDCAN_DLC_BYTES_1, FDCAN_DLC_BYTES_2,
    FDCAN_DLC_BYTES_3, FDCAN_DLC_BYTES_4, FDCAN_DLC_BYTES_5,
    FDCAN_DLC_BYTES_6, FDCAN_DLC_BYTES_7, FDCAN_DLC_BYTES_8
};
SevconMessageResult SevconStm32_TxHeader(const SevconCanFrame *f,
                                       FDCAN_TxHeaderTypeDef *out)
{
    if (f == NULL || out == NULL || f->id > 0x7FFU || f->length > 8U)
        return SEVCON_MSG_INVALID;
    FDCAN_TxHeaderTypeDef result = {0};
    result.Identifier = f->id; result.IdType = FDCAN_STANDARD_ID;
    result.TxFrameType = FDCAN_DATA_FRAME; result.DataLength = dlc[f->length];
    result.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    result.BitRateSwitch = FDCAN_BRS_OFF; result.FDFormat = FDCAN_CLASSIC_CAN;
    result.TxEventFifoControl = FDCAN_NO_TX_EVENTS; result.MessageMarker = 0U;
    *out = result; return SEVCON_MSG_OK;
}
SevconMessageResult SevconStm32_RxFrame(const FDCAN_RxHeaderTypeDef *header,
                                      const uint8_t data[8], SevconCanFrame *out)
{
    if (header == NULL || data == NULL || out == NULL ||
        header->IdType != FDCAN_STANDARD_ID || header->RxFrameType != FDCAN_DATA_FRAME ||
        header->FDFormat != FDCAN_CLASSIC_CAN || header->Identifier > 0x7FFU)
        return SEVCON_MSG_INVALID;
    for (uint8_t length = 0U; length <= 8U; ++length) {
        if (header->DataLength == dlc[length]) {
            SevconCanFrame result = {0};
            result.id = (uint16_t)header->Identifier; result.length = length;
            memcpy(result.data, data, length); *out = result; return SEVCON_MSG_OK;
        }
    }
    return SEVCON_MSG_INVALID;
}
