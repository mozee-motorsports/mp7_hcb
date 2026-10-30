#include "sevcon_messages.h"
#include <stddef.h>
#include <string.h>

/* These helpers only build or inspect frames in memory; they never send CAN.
 * NMT/SDO use their conventional CANopen IDs. The PDO helpers below use fixed
 * candidate layouts; sevcon_pdo.c provides the newer mapping-driven wrapper.
 */

/* Node 0 means broadcast for NMT. This API deliberately requires one target. */
static bool valid_node(uint8_t node) { return node >= 1U && node <= 127U; }

/* Accept the named controlword commands supported by this interface.
 * This validates the command value, not the motor's readiness or interlocks.
 */
static bool valid_control(SevconControlword control)
{
    switch (control) {
    case SEVCON_CONTROL_DISABLE_VOLTAGE: case SEVCON_CONTROL_QUICK_STOP:
    case SEVCON_CONTROL_SHUTDOWN: case SEVCON_CONTROL_SWITCH_ON:
    case SEVCON_CONTROL_ENABLE: case SEVCON_CONTROL_FAULT_RESET: return true;
    default: return false;
    }
}
/* CANopen integers are little endian: 0x1234 is sent as bytes 34 12.
 * Use shifts rather than casting the byte buffer to a potentially unaligned
 * integer pointer. These helpers work independently of the CPU's byte order.
 */
static void put16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value; p[1] = (uint8_t)(value >> 8);
}
static void put32(uint8_t *p, uint32_t value)
{
    for (unsigned i = 0; i < 4U; ++i) p[i] = (uint8_t)(value >> (8U * i));
}
static uint16_t get16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}
static uint32_t get32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
/* Avoid implementation-defined out-of-range unsigned-to-signed conversions. */
static int16_t signed16(uint16_t v)
{
    return (int16_t)(v <= INT16_MAX ? (int32_t)v : (int32_t)v - 65536);
}
static int32_t signed32(uint32_t v)
{
    return v <= INT32_MAX ? (int32_t)v : -1 - (int32_t)(UINT32_MAX - v);
}
static SevconCanFrame frame(uint16_t id, uint8_t length)
{
    /* Start with all bytes zero, including unused data/padding bytes. */
    SevconCanFrame result = {0}; 
    result.id = id; 
    result.length = length; 
    return result;
}

/* NMT controls NETWORK state. Starting a node permits PDO communication;
 * it does not establish that the motor drive is operation enabled.
 */
SevconMessageResult SevconMsg_Nmt(uint8_t node, SevconNmtCommand command,
                                SevconCanFrame *out)
{
    if (!valid_node(node) || out == NULL) {
        return SEVCON_MSG_INVALID;
    }
    switch (command) {
    case SEVCON_NMT_START: case SEVCON_NMT_STOP: case SEVCON_NMT_PREOP:
    case SEVCON_NMT_RESET: case SEVCON_NMT_RESET_COMMS: break;
    default: return SEVCON_MSG_INVALID;
    }
    /* NMT always uses ID 0 and two data bytes: command, then target node. */
    SevconCanFrame result = frame(0U, 2U);
    result.data[0] = (uint8_t)command; result.data[1] = node;
    *out = result; return SEVCON_MSG_OK;
}

/* SDO upload request: read one entry from the controller's object dictionary.
 * The caller must match a later response or abort to the pending transaction.
 */
SevconMessageResult SevconMsg_ReadObject(uint8_t node, uint16_t index,
                                       uint8_t subindex, SevconCanFrame *out)
{
    if (!valid_node(node) || out == NULL) return SEVCON_MSG_INVALID;
    SevconCanFrame result = frame((uint16_t)(0x600U + node), 8U);
    /* Default first SDO request channel is 0x600 + node. Byte 0 requests a
     * read; bytes 1-2 identify the object and byte 3 selects its subentry.
     */
    result.data[0] = 0x40U; put16(&result.data[1], index); result.data[3] = subindex;
    *out = result; return SEVCON_MSG_OK;
}
/* Expedited SDO download: write a raw value of 1-4 bytes in one request.
 * The value must fit its declared width; failed calls leave out unchanged.
 */
SevconMessageResult SevconMsg_WriteObject(uint8_t node, uint16_t index,
                                        uint8_t subindex, uint32_t value,
                                        uint8_t size, SevconCanFrame *out)
{
    if (!valid_node(node) || out == NULL || size < 1U || size > 4U ||
        (size < 4U && value >= (UINT32_C(1) << (8U * size))))
        return SEVCON_MSG_INVALID;
    SevconCanFrame result = frame((uint16_t)(0x600U + node), 8U);
    /* Encode the unused bytes in the four-byte value area: command specifiers
     * are 2F/2B/27/23 for 1/2/3/4 value bytes. The CAN frame is still 8 bytes.
     */
    result.data[0] = (uint8_t)(0x23U | ((4U - size) << 2));
    put16(&result.data[1], index); result.data[3] = subindex;
    put32(&result.data[4], value); *out = result; return SEVCON_MSG_OK;
}
/* 6040:00 is the proposed drive controlword. A write acknowledgment confirms
 * the object transaction; fresh statusword feedback confirms the drive state.
 */
SevconMessageResult SevconMsg_Control(uint8_t node, SevconControlword command,
                                    SevconCanFrame *out)
{
    if (!valid_control(command)) return SEVCON_MSG_INVALID;
    return SevconMsg_WriteObject(node, 0x6040U, 0U, (uint16_t)command, 2U, out);
}
/* Request proposed mode 4 at 6060:00; read 6061:00 to observe displayed mode. */
SevconMessageResult SevconMsg_TorqueMode(uint8_t node, SevconCanFrame *out)
{ return SevconMsg_WriteObject(node, 0x6060U, 0U, 4U, 1U, out); }

/* Request proposed mode 3. Supported modes depend on controller firmware. */
SevconMessageResult SevconMsg_VelocityMode(uint8_t node, SevconCanFrame *out)
{ return SevconMsg_WriteObject(node, 0x6060U, 0U, 3U, 1U, out); }

/* Read 6041:00. The reply contains drive-state/status bits, not NMT state. */
SevconMessageResult SevconMsg_ReadStatus(uint8_t node, SevconCanFrame *out)
{ return SevconMsg_ReadObject(node, 0x6041U, 0U, out); }

/* Read the displayed operating mode separately from the requested mode. */
SevconMessageResult SevconMsg_ReadMode(uint8_t node, SevconCanFrame *out)
{ return SevconMsg_ReadObject(node, 0x6061U, 0U, out); }

/* Write proposed 6080:00 using controller RAW units, not an assumed rpm scale. */
SevconMessageResult SevconMsg_MaxSpeed(uint8_t node, uint32_t speed_raw,
                                     SevconCanFrame *out)

{ return SevconMsg_WriteObject(node, 0x6080U, 0U, speed_raw, 4U, out); }

/* 1017:00 sets the node's heartbeat producer interval; zero disables it.
 * Consumer timeouts and communication-loss actions are configured separately.
 */
SevconMessageResult SevconMsg_HeartbeatPeriod(uint8_t node, uint16_t period_ms,
                                            SevconCanFrame *out)
{ return SevconMsg_WriteObject(node, 0x1017U, 0U, period_ms, 2U, out); }


/* confirmed is the caller's declaration that this exact layout is configured.
 * It does not inspect the DCF, validate physical units or supervise the drive.
 */
static SevconMessageResult check_pdo(const SevconPdoMapping *mapping,
                                    SevconControlword control, SevconCanFrame *out)
{
    if (mapping == NULL || out == NULL || mapping->cob_id == 0U ||
        mapping->cob_id > 0x7FFU || !valid_control(control)) return SEVCON_MSG_INVALID;
    return mapping->confirmed ? SEVCON_MSG_OK : SEVCON_MSG_MAPPING_UNCONFIRMED;
}

/* Fixed eight-byte RPDO: controlword U16 | target torque I16 | max speed U32.
 * RPDO is named from the inverter's perspective: the HCB transmits this frame.
 * Negative torque is encoded using its low 16 two's-complement bits.
 */
SevconMessageResult SevconMsg_Torque(const SevconPdoMapping *mapping,
                                   SevconControlword control, int16_t torque_raw,
                                   uint32_t max_speed_raw, SevconCanFrame *out)
{
    SevconMessageResult status = check_pdo(mapping, control, out);
    if (status != SEVCON_MSG_OK) return status;
    SevconCanFrame result = frame(mapping->cob_id, 8U);
    put16(result.data, (uint16_t)control); put16(&result.data[2], (uint16_t)torque_raw);
    put32(&result.data[4], max_speed_raw); *out = result; return SEVCON_MSG_OK;
}
/* Fixed candidate RPDO: controlword U16 | target velocity I32 | two zeros.
 * Those final bytes are only valid if the actual receiver permits that layout.
 */
SevconMessageResult SevconMsg_Velocity(const SevconPdoMapping *mapping,
                                     SevconControlword control, int32_t speed_raw,
                                     SevconCanFrame *out)
{
    SevconMessageResult status = check_pdo(mapping, control, out);
    if (status != SEVCON_MSG_OK) return status;
    SevconCanFrame result = frame(mapping->cob_id, 8U);
    put16(result.data, (uint16_t)control); put32(&result.data[2], (uint32_t)speed_raw);
    *out = result; return SEVCON_MSG_OK;
}

/* Decode the CiA 402 state while ignoring unrelated status flags. State masks
 * differ because some bits are don't-care for certain states. This is why
 * comparing the complete statusword to one constant would be unreliable.
 */
SevconDriveState SevconMsg_DriveState(uint16_t statusword)
{
    /* Bit 5 is don't-care in these four states. A single 0x006F mask is wrong. */
    switch (statusword & 0x004FU) {
    case 0x0000U: return SEVCON_DRIVE_NOT_READY;
    case 0x0040U: return SEVCON_DRIVE_DISABLED;
    case 0x000FU: return SEVCON_DRIVE_FAULT_REACTION;
    case 0x0008U: return SEVCON_DRIVE_FAULT;
    default: break;
    }
    switch (statusword & 0x006FU) {
    case 0x0021U: return SEVCON_DRIVE_READY;
    case 0x0023U: return SEVCON_DRIVE_SWITCHED_ON;
    case 0x0027U: return SEVCON_DRIVE_ENABLED;
    case 0x0007U: return SEVCON_DRIVE_QUICK_STOP;
    default: return SEVCON_DRIVE_UNKNOWN;
    }
}
/* Optional feedback PDOs must have distinct IDs and cannot claim this node's
 * heartbeat, SDO-response or emergency identifier. NULL disables PDO parsing.
 */
static bool feedback_mapping_valid(const SevconFeedbackMapping *m, uint8_t node)
{
    if (m == NULL) return true;
    const SevconPdoMapping maps[2] = {m->status_mode, m->speed_torque};
    for (unsigned i = 0; i < 2U; ++i) {
        if (maps[i].confirmed && (maps[i].cob_id == 0U || maps[i].cob_id > 0x7FFU ||
            maps[i].cob_id == 0x700U + node || maps[i].cob_id == 0x580U + node ||
            maps[i].cob_id == 0x080U + node)) return false;
    }
    return !(maps[0].confirmed && maps[1].confirmed && maps[0].cob_id == maps[1].cob_id);
}
/* Dispatch by full CANopen ID. Build a temporary result so ignored, malformed
 * or unsupported traffic cannot replace the caller's last valid observation.
 * This parser does not match SDO requests, track freshness or clear faults.
 */
SevconMessageResult SevconMsg_Decode(uint8_t node, const SevconCanFrame *f,
                                   const SevconFeedbackMapping *mapping,
                                   SevconRxMessage *out)
{
    if (!valid_node(node) || f == NULL || out == NULL || f->id > 0x7FFU ||
        f->length > 8U || !feedback_mapping_valid(mapping, node)) return SEVCON_MSG_INVALID;
    SevconRxMessage result = {0};
    if (f->id == 0x700U + node) {
        /* Boot-up is 00; heartbeat states are 04 stopped, 05 operational and
         * 7F pre-operational. Node-guard toggle-bit variants are unsupported.
         */
        if (f->length != 1U) return SEVCON_MSG_INVALID;
        switch (f->data[0]) {
        case 0U: case 4U: case 5U: case 127U: break;
        default: return SEVCON_MSG_UNSUPPORTED;
        }
        result.kind = f->data[0] == 0U ? SEVCON_RX_BOOTUP : SEVCON_RX_HEARTBEAT;
        result.nmt_state = f->data[0];
    } else if (f->id == 0x080U + node) {
        /* EMCY: two-byte error code, one error-register byte and five vendor
         * bytes. Preserve vendor bytes rather than guessing their meaning.
         */
        if (f->length != 8U) return SEVCON_MSG_INVALID;
        result.kind = SEVCON_RX_EMERGENCY; result.emergency_code = get16(f->data);
        result.error_register = f->data[2];
        memcpy(result.manufacturer_data, &f->data[3], 5U);
    } else if (f->id == 0x580U + node) {
        /* The default first SDO response channel carries the echoed object
         * address. The transaction owner must check it against its request.
         */
        if (f->length != 8U) return SEVCON_MSG_INVALID;
        result.index = get16(&f->data[1]); result.subindex = f->data[3];
        /* 60 acknowledges a write; 80 reports an abort code in bytes 4-7. */
        if (f->data[0] == 0x60U) result.kind = SEVCON_RX_SDO_WRITE_ACK;
        else if (f->data[0] == 0x80U) {
            result.kind = SEVCON_RX_SDO_ABORT; result.value = get32(&f->data[4]);
        } else {
            /* Size-indicated expedited read responses carry 1-4 value bytes.
             * Segmented/block transfers and unspecified sizes are unsupported.
             */
            switch (f->data[0]) {
            case 0x4FU: result.value_size = 1U; break;
            case 0x4BU: result.value_size = 2U; break;
            case 0x47U: result.value_size = 3U; break;
            case 0x43U: result.value_size = 4U; break;
            default: return SEVCON_MSG_UNSUPPORTED;
            }
            result.kind = SEVCON_RX_SDO_READ;
            /* Use only declared value bytes, excluding the response padding. */
            for (unsigned i = 0; i < result.value_size; ++i)
                result.value |= (uint32_t)f->data[4U + i] << (8U * i);
            /* Decode status only for the expected object, subindex and width. */
            if (result.index == 0x6041U && result.subindex == 0U && result.value_size == 2U) {
                result.statusword = (uint16_t)result.value;
                result.drive_state = SevconMsg_DriveState(result.statusword);
            }
        }
    } else if (mapping != NULL && mapping->status_mode.confirmed &&
               f->id == mapping->status_mode.cob_id) {
        /* Confirmed fixed TPDO layout: statusword U16 then displayed mode I8. */
        if (f->length != 3U) return SEVCON_MSG_INVALID;
        result.kind = SEVCON_RX_STATUS_MODE; result.statusword = get16(f->data);
        result.drive_state = SevconMsg_DriveState(result.statusword);
        result.mode = (int8_t)(f->data[2] <= INT8_MAX ? (int)f->data[2] : (int)f->data[2] - 256);
    } else if (mapping != NULL && mapping->speed_torque.confirmed &&
               f->id == mapping->speed_torque.cob_id) {
        /* Confirmed fixed TPDO layout: actual speed I32 then actual torque I16. */
        if (f->length != 6U) return SEVCON_MSG_INVALID;
        result.kind = SEVCON_RX_SPEED_TORQUE; result.speed_raw = signed32(get32(f->data));
        result.torque_raw = signed16(get16(&f->data[4]));
    } else return SEVCON_MSG_IGNORED;
    *out = result; return SEVCON_MSG_OK;
}
