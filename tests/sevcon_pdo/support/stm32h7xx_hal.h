/* TEST DOUBLE ONLY. Not a vendor HAL and not for firmware deployment.
 * Deliberately encoded DLC values catch accidental use as a byte count.
 */
#ifndef SEVCON_TEST_HAL_H
#define SEVCON_TEST_HAL_H
#include <stdint.h>
#include <stddef.h>
#ifdef __weak
#undef __weak
#endif
#define __weak __attribute__((weak))
typedef enum {HAL_OK = 0, HAL_ERROR = 1} HAL_StatusTypeDef;
typedef struct {unsigned test_id;} FDCAN_HandleTypeDef;
#define FDCAN_RX_FIFO0 0U
#define FDCAN_RX_FIFO1 1U
#define FDCAN_DLC_BYTES_0 (0U << 16)
#define FDCAN_DLC_BYTES_1 (1U << 16)
#define FDCAN_DLC_BYTES_2 (2U << 16)
#define FDCAN_DLC_BYTES_3 (3U << 16)
#define FDCAN_DLC_BYTES_4 (4U << 16)
#define FDCAN_DLC_BYTES_5 (5U << 16)
#define FDCAN_DLC_BYTES_6 (6U << 16)
#define FDCAN_DLC_BYTES_7 (7U << 16)
#define FDCAN_DLC_BYTES_8 (8U << 16)
#define FDCAN_STANDARD_ID 0U
#define FDCAN_DATA_FRAME 0U
#define FDCAN_CLASSIC_CAN 0U
#define FDCAN_ESI_ACTIVE 0U
#define FDCAN_BRS_OFF 0U
#define FDCAN_NO_TX_EVENTS 0U
typedef struct {
    uint32_t Identifier, IdType, TxFrameType, DataLength, ErrorStateIndicator;
    uint32_t BitRateSwitch, FDFormat, TxEventFifoControl, MessageMarker;
} FDCAN_TxHeaderTypeDef;
typedef struct {
    uint32_t Identifier, IdType, RxFrameType, DataLength, ErrorStateIndicator;
    uint32_t BitRateSwitch, FDFormat, RxTimestamp, FilterIndex, IsFilterMatchingFrame;
} FDCAN_RxHeaderTypeDef;
uint32_t HAL_FDCAN_GetRxFifoFillLevel(FDCAN_HandleTypeDef *, uint32_t);
HAL_StatusTypeDef HAL_FDCAN_GetRxMessage(FDCAN_HandleTypeDef *, uint32_t,
                                        FDCAN_RxHeaderTypeDef *, uint8_t *);
#endif
