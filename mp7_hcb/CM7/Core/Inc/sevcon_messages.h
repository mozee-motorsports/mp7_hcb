#ifndef SEVCON_MESSAGES_H
#define SEVCON_MESSAGES_H

// Message builders only. Nothing in this library transmits CAN or enables a drive.
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


// CAN FRAME DEFINITION!! GENERIC
typedef struct {
    uint16_t id;
    uint8_t length;
    uint8_t data[8];
} CanFrame;
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

SevconMessageResult SevconMsg_Decode(uint8_t node, const SevconCanFrame *frame,
                                   const SevconFeedbackMapping *mapping,
                                   SevconRxMessage *out);

#ifdef __cplusplus
}
#endif
#endif