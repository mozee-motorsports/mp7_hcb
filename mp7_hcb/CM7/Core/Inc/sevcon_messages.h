#ifndef SEVCON_MESSAGES_H
#define SEVCON_MESSAGES_H

/* Message builders only. Nothing in this library transmits CAN or enables a drive.
 * Object support, units and PDO mappings must be checked against the actual EDS/DCF.
 * This is independent of the unfinished Sevcon_* API in the original firmware.
 */
#include <stdbool.h>
#include <stdint.h>
#include "can_frame.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CanFrame SevconCanFrame; /* Backward-compatible name for existing helpers. */

typedef enum {
    SEVCON_MSG_OK = 0,
    SEVCON_MSG_INVALID,
    SEVCON_MSG_MAPPING_UNCONFIRMED,
    SEVCON_MSG_IGNORED,
    SEVCON_MSG_UNSUPPORTED
} SevconMessageResult;

typedef enum {
    SEVCON_NMT_START = 0x01,
    SEVCON_NMT_STOP = 0x02,
    SEVCON_NMT_PREOP = 0x80,
    SEVCON_NMT_RESET = 0x81,
    SEVCON_NMT_RESET_COMMS = 0x82
} SevconNmtCommand;

typedef enum {
    SEVCON_CONTROL_DISABLE_VOLTAGE = 0x0000,
    SEVCON_CONTROL_QUICK_STOP = 0x0002,
    SEVCON_CONTROL_SHUTDOWN = 0x0006,
    SEVCON_CONTROL_SWITCH_ON = 0x0007,
    SEVCON_CONTROL_ENABLE = 0x000F,
    SEVCON_CONTROL_FAULT_RESET = 0x0080
} SevconControlword;

typedef enum {
    SEVCON_DRIVE_NOT_READY,
    SEVCON_DRIVE_DISABLED,
    SEVCON_DRIVE_READY,
    SEVCON_DRIVE_SWITCHED_ON,
    SEVCON_DRIVE_ENABLED,
    SEVCON_DRIVE_QUICK_STOP,
    SEVCON_DRIVE_FAULT_REACTION,
    SEVCON_DRIVE_FAULT,
    SEVCON_DRIVE_UNKNOWN
} SevconDriveState;

/* Caller attests that the EXACT byte layout documented for that function is
 * configured in the controller. This flag does not inspect or change the DCF.
 * Separate records are required for torque and velocity PDOs.
 */
typedef struct {
    uint16_t cob_id;
    bool confirmed;
} SevconPdoMapping;

typedef struct {
    SevconPdoMapping status_mode; /* u16 statusword, i8 mode; exactly 3 bytes. */
    SevconPdoMapping speed_torque;/* i32 speed, i16 torque; exactly 6 bytes. */
} SevconFeedbackMapping;

typedef enum {
    SEVCON_RX_HEARTBEAT,
    SEVCON_RX_BOOTUP,
    SEVCON_RX_EMERGENCY,
    SEVCON_RX_SDO_WRITE_ACK,
    SEVCON_RX_SDO_READ,
    SEVCON_RX_SDO_ABORT,
    SEVCON_RX_STATUS_MODE,
    SEVCON_RX_SPEED_TORQUE
} SevconRxKind;

/* Only fields belonging to kind are meaningful. All other fields are zero.
 * A parsed response is not automatically matched to an outstanding request.
 */
typedef struct {
    SevconRxKind kind;
    uint8_t nmt_state;
    uint16_t index;
    uint8_t subindex;
    uint8_t value_size;
    uint32_t value;              /* Raw unsigned bits; abort code for SDO_ABORT. */
    uint16_t statusword;
    SevconDriveState drive_state;
    int8_t mode;
    int32_t speed_raw;
    int16_t torque_raw;
    uint16_t emergency_code;
    uint8_t error_register;
    uint8_t manufacturer_data[5];
} SevconRxMessage;

/* Node IDs 1..127 only. Broadcast node 0 is deliberately not supported.
 * SDO helpers use the default first server channel (0x600/0x580 + node).
 * Only use output frames after SEVCON_MSG_OK. Errors leave output unchanged.
 */
SevconMessageResult SevconMsg_Nmt(uint8_t node, SevconNmtCommand command,
                                SevconCanFrame *out);
SevconMessageResult SevconMsg_ReadObject(uint8_t node, uint16_t index,
                                       uint8_t subindex, SevconCanFrame *out);
SevconMessageResult SevconMsg_WriteObject(uint8_t node, uint16_t index,
                                        uint8_t subindex, uint32_t value,
                                        uint8_t size, SevconCanFrame *out);
SevconMessageResult SevconMsg_Control(uint8_t node, SevconControlword command,
                                    SevconCanFrame *out);
SevconMessageResult SevconMsg_TorqueMode(uint8_t node, SevconCanFrame *out);
SevconMessageResult SevconMsg_VelocityMode(uint8_t node, SevconCanFrame *out);
SevconMessageResult SevconMsg_ReadStatus(uint8_t node, SevconCanFrame *out);
SevconMessageResult SevconMsg_ReadMode(uint8_t node, SevconCanFrame *out);
SevconMessageResult SevconMsg_MaxSpeed(uint8_t node, uint32_t speed_raw,
                                     SevconCanFrame *out);
SevconMessageResult SevconMsg_HeartbeatPeriod(uint8_t node, uint16_t period_ms,
                                            SevconCanFrame *out);

/* Candidate layouts from the ORIGINAL HEADER, not universal Sevcon defaults:
 * torque:   controlword u16 | target torque i16 | maximum speed u32
 * velocity: controlword u16 | target velocity i32 | two zero padding bytes
 * All fields are little endian. Raw units are deliberately not called Nm/rpm.
 * There are no limit clamps, watchdogs or state checks in these codecs.
 */
SevconMessageResult SevconMsg_Torque(const SevconPdoMapping *mapping,
                                   SevconControlword control, int16_t torque_raw,
                                   uint32_t max_speed_raw, SevconCanFrame *out);
SevconMessageResult SevconMsg_Velocity(const SevconPdoMapping *mapping,
                                     SevconControlword control, int32_t speed_raw,
                                     SevconCanFrame *out);

SevconDriveState SevconMsg_DriveState(uint16_t statusword);
/* Heartbeat/EMCY/default SDO channel decoded without any PDO assumptions.
 * Pass NULL for mapping until feedback layout is verified. A malformed,
 * ignored or unsupported message leaves out unchanged. Heartbeat node guarding
 * toggle-bit frames and segmented/block SDO transfers are not supported.
 */
SevconMessageResult SevconMsg_Decode(uint8_t node, const SevconCanFrame *frame,
                                   const SevconFeedbackMapping *mapping,
                                   SevconRxMessage *out);

#ifdef __cplusplus
}
#endif
#endif
