#include "mp7_hcb.h"
#include "hcb_can_config.h"
#include "sevcon_rx.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

/* These fixtures exercise software only, not a real HAL or inverter. */
FDCAN_HandleTypeDef hfdcan1 = {1}, hfdcan2 = {2};
static unsigned queue_count, queue_pos, fail_read, read_calls;
static FDCAN_HandleTypeDef *expected_bus;
static uint32_t expected_fifo;
static unsigned vehicle_calls;
static FDCAN_RxHeaderTypeDef headers[12];
static uint8_t payloads[12][8];

uint32_t HAL_FDCAN_GetRxFifoFillLevel(FDCAN_HandleTypeDef *bus, uint32_t fifo)
{
    assert(bus == expected_bus && fifo == expected_fifo);
    return queue_count - queue_pos;
}

HAL_StatusTypeDef HAL_FDCAN_GetRxMessage(FDCAN_HandleTypeDef *bus, uint32_t fifo,
                                        FDCAN_RxHeaderTypeDef *header, uint8_t *data)
{
    assert(bus == expected_bus && fifo == expected_fifo);
    ++read_calls;
    if (fail_read) return HAL_ERROR;
    assert(queue_pos < queue_count);
    *header = headers[queue_pos];
    memcpy(data, payloads[queue_pos++], 8);
    return HAL_OK;
}

HAL_StatusTypeDef MP7_FDCAN_InitAndStart(FDCAN_HandleTypeDef *bus)
{
    assert(bus == &hfdcan1 || bus == &hfdcan2);
    return HAL_OK;
}

eCAN_Error MP7_FDCAN_ProcessRxMessages(FDCAN_HandleTypeDef *bus, uint32_t fifo,
                                       MP7_CAN_RxCallback callback, uint32_t now_ms)
{
    (void)now_ms;
    assert(bus == HCB_GetVehicleCan() && fifo == FDCAN_RX_FIFO0);
    assert(callback == HCB_HandleCANRxMessage);
    ++vehicle_calls;
    return CAN_OK;
}

eCAN_Error MP7_FDCAN_RegisterPeriodicMessage(sCAN_PeriodicMessage *message,
                                            MP7_CAN_TxCallback callback)
{
    (void)message; (void)callback;
    return CAN_OK;
}

eCAN_Error MP7_FDCAN_SendMessage(FDCAN_HandleTypeDef *bus, const sCAN_Message *message)
{
    (void)bus; (void)message;
    assert(!"Receive tests must not send a frame");
    return CAN_GENERIC;
}

void MP7_HandleCANError(eCAN_Error error) { (void)error; }
void Error_Handler(void) { assert(!"Unexpected initialization error"); }

static CanFrame fixture(void)
{
    /* 0xFF60 = -160 -> -10 in the documented engineering unit. */
    CanFrame frame = {0x410, 8, {0x60, 0xFF, 0xFF, 0xFF, 0x00, 0x80, 0x34, 0x12}};
    return frame;
}

static void same_feedback(const SevconPdoFeedback *a, const SevconPdoFeedback *b)
{
    for (unsigned i = 0; i < SEVCON_TPDO_COUNT; ++i) {
        assert(a->tpdo[i].seen == b->tpdo[i].seen);
        assert(a->tpdo[i].updated_ms == b->tpdo[i].updated_ms);
        assert(a->tpdo[i].value_count == b->tpdo[i].value_count);
        for (size_t j = 0; j < a->tpdo[i].value_count; ++j) {
            assert(a->tpdo[i].values[j].index == b->tpdo[i].values[j].index);
            assert(a->tpdo[i].values[j].subindex == b->tpdo[i].values[j].subindex);
            assert(a->tpdo[i].values[j].raw == b->tpdo[i].values[j].raw);
        }
    }
}

static void codec_checks(void)
{
    SevconPdoLayouts maps;
    assert(SevconPdo_DefaultLayouts(&maps) == PDO_OK);
    assert(SevconPdo_ValidateLayouts(&maps) == PDO_OK);
    assert(maps.tpdo[0].enabled && maps.tpdo[0].map.cob_id == 0x410);
    for (unsigned i = 1; i < SEVCON_TPDO_COUNT; ++i) assert(!maps.tpdo[i].enabled);
    SevconPdoFeedback feedback;
    SevconPdo_ClearFeedback(&feedback);
    SevconCurrentFeedback currents = {0};
    assert(!SevconPdo_GetCurrents(&feedback, &currents));
    assert(!SevconPdo_IsFresh(&feedback, SEVCON_TPDO1, 0, 100));

    CanFrame frame = fixture();
    SevconTpdoSlot updated = SEVCON_TPDO5;
    assert(SevconPdo_Receive(&maps, &frame, 100, &feedback, &updated) == PDO_OK);
    assert(updated == SEVCON_TPDO1);
    assert(SevconPdo_GetCurrents(&feedback, &currents));
    assert(currents.target_id_raw == -160 && currents.target_id_value == -10.0f);
    assert(currents.target_iq_bits == 0xFFFF && currents.id_bits == 0x8000);
    assert(currents.iq_bits == 0x1234 && currents.updated_ms == 100);
    assert(SevconPdo_IsFresh(&feedback, SEVCON_TPDO1, 120, 20));
    assert(!SevconPdo_IsFresh(&feedback, SEVCON_TPDO1, 121, 20));
    assert(!SevconPdo_IsFresh(&feedback, (SevconTpdoSlot)-1, 100, 20));

    /* Generic placeholders can receive different object sets and byte lengths.
     * Each of the four added slots must keep its own independent sample age.
     */
    static const PdoField extra[] = {{0x2000, 1, PDO_U16}};
    for (unsigned i = 1; i < SEVCON_TPDO_COUNT; ++i) {
        maps.tpdo[i] = (SevconTpdoConfig){true, {(uint16_t)(0x500 + i), extra, 1, true}};
        CanFrame other = {(uint16_t)(0x500 + i), 2, {(uint8_t)i, 0}};
        assert(SevconPdo_Receive(&maps, &other, 200 + i, &feedback, &updated) == PDO_OK);
        assert((unsigned)updated == i && feedback.tpdo[i].updated_ms == 200 + i);
        assert(feedback.tpdo[0].updated_ms == 100);
        assert(SevconPdo_GetValue(&feedback, (SevconTpdoSlot)i, 0x2000, 1)->raw == i);
    }

    SevconPdoFeedback saved = feedback;
    updated = SEVCON_TPDO5;
    frame.length = 7;
    assert(SevconPdo_Receive(&maps, &frame, 999, &feedback, &updated) == PDO_INVALID);
    assert(updated == SEVCON_TPDO5);
    same_feedback(&feedback, &saved);
    frame.length = 8; frame.id = 0x411;
    assert(SevconPdo_Receive(&maps, &frame, 999, &feedback, &updated) == PDO_NOT_MINE);
    same_feedback(&feedback, &saved);
    frame.id = 0x410;
    maps.tpdo[0].map.confirmed = false;
    assert(SevconPdo_Receive(&maps, &frame, 999, &feedback, &updated) == PDO_UNCONFIRMED);
    same_feedback(&feedback, &saved);
    maps.tpdo[0].map.confirmed = true;
    maps.tpdo[1].map.cob_id = 0x410;
    assert(SevconPdo_ValidateLayouts(&maps) == PDO_INVALID);
    maps.tpdo[1].enabled = false;
    assert(SevconPdo_ValidateLayouts(&maps) == PDO_OK);

    /* Changing wire order keeps named lookup correct. */
    static const PdoField reversed[] = {
        {0x4600, 8, PDO_U16}, {0x4600, 7, PDO_U16},
        {0x4600, 6, PDO_U16}, {0x4600, 5, PDO_I16}
    };
    maps.tpdo[0].map.fields = reversed;
    frame = (CanFrame){0x410, 8, {0x34, 0x12, 0x00, 0x80, 0xFF, 0xFF, 0x60, 0xFF}};
    assert(SevconPdo_Receive(&maps, &frame, UINT32_MAX - 5U, &feedback, &updated) == PDO_OK);
    assert(SevconPdo_GetCurrents(&feedback, &currents) && currents.target_id_value == -10);
    assert(SevconPdo_IsFresh(&feedback, SEVCON_TPDO1, 4, 10));
    assert(!SevconPdo_IsFresh(&feedback, SEVCON_TPDO1, 5, 10));

    assert(SevconPdo_DefaultLayouts(&maps) == PDO_OK);
    frame = fixture();
    for (int value = INT16_MIN; value <= INT16_MAX; ++value) {
        uint16_t bits = (uint16_t)value;
        frame.data[0] = (uint8_t)bits; frame.data[1] = (uint8_t)(bits >> 8);
        assert(SevconPdo_Receive(&maps, &frame, 1, &feedback, &updated) == PDO_OK);
        assert(SevconPdo_GetCurrents(&feedback, &currents));
        assert(currents.target_id_raw == value);
        assert(currents.target_id_value == (float)value * 0.0625f);
    }
    maps.tpdo[1].enabled = true; /* Empty enabled slot must fail validation. */
    assert(SevconPdo_ValidateLayouts(&maps) == PDO_INVALID);
}

static void enqueue(unsigned count)
{
    queue_count = count; queue_pos = 0; fail_read = 0; read_calls = 0;
    expected_bus = &hfdcan2; expected_fifo = FDCAN_RX_FIFO0;
    CanFrame frame = fixture();
    for (unsigned i = 0; i < count; ++i) {
        headers[i] = (FDCAN_RxHeaderTypeDef){0};
        headers[i].Identifier = 0x410;
        headers[i].DataLength = FDCAN_DLC_BYTES_8;
        memcpy(payloads[i], frame.data, 8);
    }
}

static void receive_checks(void)
{
    SevconPdoLayouts maps;
    SevconPdoFeedback feedback;
    assert(SevconPdo_DefaultLayouts(&maps) == PDO_OK);
    SevconPdo_ClearFeedback(&feedback);
    enqueue(1); expected_fifo = FDCAN_RX_FIFO1;
    assert(SevconRx_Process(&hfdcan2, FDCAN_RX_FIFO1, 10, &maps, &feedback) == HAL_OK);
    assert(queue_pos == 1 && feedback.tpdo[0].updated_ms == 10);
    SevconPdoFeedback saved = feedback;
    enqueue(3);
    headers[0].Identifier = 0x701; /* Other CANopen service: not current telemetry. */
    headers[1].DataLength = FDCAN_DLC_BYTES_7;
    headers[2].IdType = 1; /* Extended ID, despite matching numeric identifier. */
    assert(SevconRx_Process(&hfdcan2, FDCAN_RX_FIFO0, 20, &maps, &feedback) == HAL_ERROR);
    same_feedback(&feedback, &saved);
    enqueue(1); fail_read = 1;
    assert(SevconRx_Process(&hfdcan2, FDCAN_RX_FIFO0, 30, &maps, &feedback) == HAL_ERROR);
    assert(read_calls == 1);
    same_feedback(&feedback, &saved);
    enqueue(10);
    assert(SevconRx_Process(&hfdcan2, FDCAN_RX_FIFO0, 40, &maps, &feedback) == HAL_OK);
    assert(queue_pos == 8); /* Bounded work prevents receive-loop starvation. */
    assert(SevconRx_Process(&hfdcan2, FDCAN_RX_FIFO0, 41, &maps, &feedback) == HAL_OK);
    assert(queue_pos == 10 && feedback.tpdo[0].updated_ms == 41);
    assert(SevconRx_Process(NULL, FDCAN_RX_FIFO0, 50, &maps, &feedback) == HAL_ERROR);
    assert(SevconRx_Process(&hfdcan2, 9, 50, &maps, &feedback) == HAL_ERROR);
}

static void routing_checks(void)
{
    HCB_Init();
    enqueue(1);
#if HCB_VEHICLE_FDCAN_INDEX == 1
    assert(HCB_GetVehicleCan() == &hfdcan1);
#else
    assert(HCB_GetVehicleCan() == &hfdcan2);
#endif
#if HCB_SEVCON_FDCAN_INDEX == 1
    expected_bus = &hfdcan1;
#endif
    unsigned calls_before = vehicle_calls;
    assert(HCB_ProcessCANRx(123) == CAN_OK);
    assert(vehicle_calls == calls_before + 1);
#if HCB_SEVCON_FDCAN_INDEX == 0
    assert(queue_pos == 0 && !HCB_GetSevconFeedback()->tpdo[0].seen);
#else
    assert(queue_pos == 1 && HCB_GetSevconFeedback()->tpdo[0].updated_ms == 123);
#endif
}

int main(void)
{
    codec_checks(); receive_checks(); routing_checks();
    puts("PASS: five slots, signed/scaled Target Id, rejection/age preservation, raw HAL RX and bus routing");
    return 0;
}
