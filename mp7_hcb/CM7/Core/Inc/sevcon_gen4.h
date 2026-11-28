/*
 * sevcon_gen4.h
 *
 * Library for the Sevcon Gen4 Motor Controller (CANopen / CiA 402 Profile)
 *
 *  Last Modified: Oct 21, 2026
 *      By Author: montemayorm
 *
 *  Created on: Oct 21, 2026
 *      Author: montemayorm
 */

#ifndef CM7_CORE_INC_SEVCON_GEN4_H_
#define CM7_CORE_INC_SEVCON_GEN4_H_

#include <CO_app_STM32.h>
#include <stdint.h>
#include <stdbool.h>
#include "CANopen.h"

#if __has_include("stm32h7xx_hal.h")
#include "stm32h7xx_hal.h"
#elif __has_include("stm32g4xx_hal.h")
#include "stm32g4xx_hal.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/* CANopen Standard COB-ID Definitions (Base Offsets)                        */
/* ========================================================================== */
#define SEVCON_COBID_NMT 0x000U            /* Network Management */
#define SEVCON_COBID_SYNC 0x080U           /* Synchronization */
#define SEVCON_COBID_EMCY_BASE 0x080U      /* Emergency (0x080 + NodeID) */
#define SEVCON_COBID_TPDO1_BASE 0x180U     /* TPDO1 (0x180 + NodeID) */
#define SEVCON_COBID_RPDO1_BASE 0x200U     /* RPDO1 (0x200 + NodeID) */
#define SEVCON_COBID_TPDO2_BASE 0x280U     /* TPDO2 (0x280 + NodeID) */
#define SEVCON_COBID_RPDO2_BASE 0x300U     /* RPDO2 (0x300 + NodeID) */
#define SEVCON_COBID_TPDO3_BASE 0x380U     /* TPDO3 (0x380 + NodeID) */
#define SEVCON_COBID_RPDO3_BASE 0x400U     /* RPDO3 (0x400 + NodeID) */
#define SEVCON_COBID_TPDO4_BASE 0x480U     /* TPDO4 (0x480 + NodeID) */
#define SEVCON_COBID_RPDO4_BASE 0x500U     /* RPDO4 (0x500 + NodeID) */
#define SEVCON_COBID_TSDO_BASE 0x580U      /* SDO Response / Tx (0x580 + NodeID) */
#define SEVCON_COBID_RSDO_BASE 0x600U      /* SDO Request / Rx (0x600 + NodeID) */
#define SEVCON_COBID_HEARTBEAT_BASE 0x700U /* Heartbeat / Node Guard (0x700 + NodeID) */

/* Default Sevcon Node ID (Subject to customer .dcf / DVT configuration) */
#define SEVCON_NODE_ID 0x01U

/* ========================================================================== */
/* CiA 402 & Sevcon Object Dictionary Indices                                */
/* ========================================================================== */
#define SEVCON_OBJ_DEVICE_TYPE 0x1000U             /* Device Type (uint32) */
#define SEVCON_OBJ_ERROR_REGISTER 0x1001U          /* Error Register (uint8) */
#define SEVCON_OBJ_PRODUCER_HEARTBEAT_TIME 0x1017U /* Producer Heartbeat Time (uint16, ms) */
#define SEVCON_OBJ_CONTROLWORD 0x6040U             /* CiA 402 Controlword (uint16) */
#define SEVCON_OBJ_STATUSWORD 0x6041U              /* CiA 402 Statusword (uint16) */
#define SEVCON_OBJ_VL_TARGET_VELOCITY 0x6042U      /* Target Velocity (int16, rpm) */
#define SEVCON_OBJ_MODES_OF_OPERATION 0x6060U      /* Modes of Operation (int8) */
#define SEVCON_OBJ_MODES_OF_OPERATION_DISP 0x6061U /* Modes of Operation Display (int8) */
#define SEVCON_OBJ_VELOCITY_ACTUAL_VALUE 0x606CU   /* Actual Motor Velocity (int32, rpm) */
#define SEVCON_OBJ_TARGET_TORQUE 0x6071U           /* Target Torque (int16, 0.1% rated) */
#define SEVCON_OBJ_MAX_TORQUE 0x6072U              /* Max Torque (uint16, 0.1% rated) */
#define SEVCON_OBJ_TORQUE_ACTUAL_VALUE 0x6077U     /* Actual Motor Torque (int16, 0.1% rated) */
#define SEVCON_OBJ_MAX_MOTOR_SPEED 0x6080U         /* Max Motor Speed (uint32, rpm limit in torque mode) */
#define SEVCON_OBJ_QUICK_STOP_DECEL 0x6085U        /* Quick Stop Deceleration (uint32) */
#define SEVCON_OBJ_TARGET_VELOCITY 0x60FFU         /* Target Velocity (int32, counts/sec or rpm) */

/* ========================================================================== */
/* CiA 402 Controlword Commands (Object 0x6040)                               */
/* ========================================================================== */
#define SEVCON_CMD_SHUTDOWN 0x0006U         /* Transition 2, 6, 8: Ready to Switch On */
#define SEVCON_CMD_SWITCH_ON 0x0007U        /* Transition 3: Switched On */
#define SEVCON_CMD_ENABLE_OPERATION 0x000FU /* Transition 4: Operation Enabled */
#define SEVCON_CMD_DISABLE_VOLTAGE 0x0000U  /* Transition 7, 9, 10, 12: Switch On Disabled */
#define SEVCON_CMD_QUICK_STOP 0x0002U       /* Transition 7, 10, 11: Quick Stop Active */
#define SEVCON_CMD_FAULT_RESET 0x0080U      /* Bit 7 0->1 transition: Reset Fault */

/* ========================================================================== */
/* CiA 402 Statusword Bitmasks (Object 0x6041)                                */
/* ========================================================================== */
#define SEVCON_STATUS_READY_TO_SWITCH_ON 0x0001U
#define SEVCON_STATUS_SWITCHED_ON 0x0002U
#define SEVCON_STATUS_OPERATION_ENABLED 0x0004U
#define SEVCON_STATUS_FAULT 0x0008U
#define SEVCON_STATUS_VOLTAGE_ENABLED 0x0010U
#define SEVCON_STATUS_QUICK_STOP 0x0020U /* Active low in CiA 402: 0 = Quick Stop active */
#define SEVCON_STATUS_SWITCH_ON_DISABLED 0x0040U
#define SEVCON_STATUS_WARNING 0x0080U
#define SEVCON_STATUS_REMOTE 0x0200U
#define SEVCON_STATUS_TARGET_REACHED 0x0400U
#define SEVCON_STATUS_INTERNAL_LIMIT 0x0800U

/* Statusword State Decoding Mask */
#define SEVCON_STATUS_STATE_MASK 0x006FU

    /* ========================================================================== */
    /* Types and Enumerations                                                     */
    /* ========================================================================== */

    /**
     * @brief Sevcon Driver Status Return Codes
     */
    typedef enum
    {
        SEVCON_OK = 0x00U,
        SEVCON_ERROR = 0x01U,
        SEVCON_INVALID_PARAM = 0x02U,
        SEVCON_NOT_INITIALIZED = 0x03U,
        SEVCON_TIMEOUT = 0x04U,
        SEVCON_BUSY = 0x05U,
        SEVCON_FAULT_ACTIVE = 0x06U,
        SEVCON_STATE_REJECTED = 0x07U,
    } Sevcon_Status_t;

    /**
     * @brief CiA 402 Standard Drive States
     */
    typedef enum
    {
        SEVCON_STATE_NOT_READY_TO_SWITCH_ON = 0x00U,
        SEVCON_STATE_SWITCH_ON_DISABLED = 0x01U,
        SEVCON_STATE_READY_TO_SWITCH_ON = 0x02U,
        SEVCON_STATE_SWITCHED_ON = 0x03U,
        SEVCON_STATE_OPERATION_ENABLED = 0x04U,
        SEVCON_STATE_QUICK_STOP_ACTIVE = 0x05U,
        SEVCON_STATE_FAULT_REACTION_ACTIVE = 0x06U,
        SEVCON_STATE_FAULT = 0x07U,
        SEVCON_STATE_UNKNOWN = 0xFFU
    } Sevcon_State_t;

    /**
     * @brief CiA 402 Modes of Operation (Object 0x6060)
     */
    typedef enum
    {
        SEVCON_MODE_NO_MODE = 0,
        SEVCON_MODE_PROFILE_POSITION = 1,
        SEVCON_MODE_PROFILE_VELOCITY = 3,
        SEVCON_MODE_PROFILE_TORQUE = 4,
    } Sevcon_Mode_t;

    /**
     * @brief CANopen NMT Command Specifiers (Object COB-ID 0x000)
     */
    typedef enum
    {
        SEVCON_NMT_START_NODE = 0x01U,            /* Enter Operational state (enables PDOs) */
        SEVCON_NMT_STOP_NODE = 0x02U,             /* Enter Stopped state */
        SEVCON_NMT_ENTER_PRE_OPERATIONAL = 0x80U, /* Enter Pre-Operational state (SDO only) */
        SEVCON_NMT_RESET_NODE = 0x81U,            /* Software reset of node */
        SEVCON_NMT_RESET_COMMUNICATION = 0x82U,   /* Reset CAN communication stack */
    } Sevcon_NMT_Command_t;

    /**
     * @brief Telemetry Data parsed from Sevcon TPDOs
     */
    typedef struct
    {
        uint16_t statusword;           /* Raw CiA 402 Statusword (0x6041) */
        Sevcon_State_t state;          /* Decoded CiA 402 state */
        Sevcon_Mode_t mode_display;    /* Mode of operation display (0x6061) */
        int16_t actual_torque_0p1pct;  /* Actual torque (0x6077, 0.1% rated) */
        int32_t actual_speed_rpm;      /* Actual speed (0x606C, rpm) */
        uint16_t battery_voltage_0p1v; /* Capacitor/battery voltage (if mapped) */
        int16_t motor_temp_c;          /* Motor temperature in deg C (if mapped) */
        int16_t controller_temp_c;     /* Heatsink temperature in deg C (if mapped) */
        uint16_t last_emergency_code;  /* Last EMCY error code received */
        uint32_t last_rx_timestamp_ms; /* Timestamp of last valid message */
        bool is_responsive;            /* True if heartbeat/TPDO within timeout */
        bool fault_present;            /* True if statusword indicates fault */
    } Sevcon_Telemetry_t;

    /**
     * @brief Safety and Operational Limits Configuration
     */
    typedef struct
    {
        int16_t max_forward_torque_0p1pct; /* Forward torque clamp (e.g. +1000 = +100.0%) */
        int16_t max_reverse_torque_0p1pct; /* Reverse/regen torque clamp (e.g. -1000 = -100.0%) */
        uint32_t max_forward_speed_rpm;    /* Maximum motor speed clamp in RPM (0x6080) */
        uint32_t max_reverse_speed_rpm;    /* Reverse maximum motor speed clamp in RPM */
        uint32_t heartbeat_timeout_ms;     /* Maximum time before flagging comm loss */
        uint32_t command_rate_ms;          /* Expected rate for sending cyclic RPDOs */
        uint32_t quick_stop_decel_rpm_s;   /* Deceleration rate for quick-stop (0x6085) */
    } Sevcon_SafetyLimits_t;

    /**
     * @brief Sevcon Controller Handle Structure
     */
    typedef struct
    {
        CO_t *stack;                  /* STM32 FDCAN peripheral handle */
        uint8_t node_id;              /* Configured CANopen Node ID (1-127) */
        Sevcon_Mode_t active_mode;    /* Current operating mode */
        Sevcon_SafetyLimits_t limits; /* Configured limits and safety parameters */
        Sevcon_Telemetry_t telemetry; /* Latest feedback received from controller */
        uint16_t current_controlword; /* Current command word being sent */
        bool initialized;             /* True once init sequence has completed */
    } Sevcon_Handle_t;

    /* ========================================================================== */
    /* Function Prototypes & CAN Message Specifications                           */
    /* ========================================================================== */

    /**
     * @brief Initializes the Sevcon controller and brings it into Operational state.
     *
     * @param[in,out] handle   Pointer to Sevcon instance handle.
     * @param[in]     hfdcan   Pointer to initialized STM32 FDCAN handle.
     * @param[in]     node_id  CANopen Node ID of Sevcon (typically 0x01).
     * @param[in]     limits   Pointer to safety and operational limit settings.
     * @return Sevcon_Status_t SEVCON_OK on successful initialization sequence.
     *
     */
    Sevcon_Status_t Sevcon_Init(Sevcon_Handle_t *handle,
                                CO_t *stack,
                                uint8_t node_id,
                                const Sevcon_SafetyLimits_t *limits);

    /**
     * @brief Safely deinitializes the Sevcon controller and disables torque/voltage.
     *
     * @param[in,out] handle Pointer to Sevcon instance handle.
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * CAN Messages Sent:
     * 1. Zero Demands:
     *    - Sends 0 torque / 0 speed command via RPDO.
     * 2. CiA 402 Disable Voltage:
     *    - COB-ID:  0x200 + NodeID (RPDO1) or 0x600 + NodeID (SDO 0x6040)
     *    - DLC:     2 to 8
     *    - Payload: Controlword = 0x0000 (Disable Voltage / Switch On Disabled)
     * 3. NMT Enter Pre-Operational or Stop:
     *    - COB-ID:  0x000
     *    - DLC:     2
     *    - Payload: [0x80, NodeID] (Pre-Operational) or [0x02, NodeID] (Stopped)
     */
    Sevcon_Status_t Sevcon_Deinit(Sevcon_Handle_t *handle);

    // Taken from line 110 of co_sdoclient.h, sends SDO's easily
    /**
     * @brief Write one 1–4 byte value to a remote CANopen object and wait for completion.
     * @param[in,out] SDO_C    Initialized SDO client available for this transfer.
     * @param[in] nodeId       Remote node ID, from 1 through 127.
     * @param[in] index        Object dictionary index.
     * @param[in] subIndex     Object dictionary subindex.
     * @param[in] data         Value encoded in CANopen wire byte order.
     * @param[in] dataSize     Number of value bytes required by the object.
     * @return CO_SDO_AB_NONE on confirmed completion, or an abort code on failure.
     */
    CO_SDO_abortCode_t
    write_SDO(CO_SDOclient_t *SDO_C, uint8_t nodeId, uint16_t index,
              uint8_t subIndex, uint8_t *data, size_t dataSize);
    /**
     * @brief Selects the CiA 402 Mode of Operation (Torque vs Velocity).
     *
     * @param[in,out] handle Pointer to Sevcon instance handle.
     * @param[in]     mode   Desired mode (e.g. SEVCON_MODE_PROFILE_TORQUE or SEVCON_MODE_PROFILE_VELOCITY).
     * @return Sevcon_Status_t SEVCON_OK if command was dispatched.
     *
     * @details
     * CAN Message Sent (SDO Download Expedited to Object 0x6060:00):
     * - COB-ID:  0x600 + NodeID
     * - DLC:     8
     * - Payload:
     *   - Byte 0: 0x2F (SDO write expedited, 1 byte data)
     *   - Byte 1: 0x60 (Object Index LSB: 0x6060)
     *   - Byte 2: 0x60 (Object Index MSB)
     *   - Byte 3: 0x00 (Sub-index: 0x00)
     *   - Byte 4: Mode (4 = Profile Torque, 3 = Profile Velocity)
     *   - Bytes 5-7: 0x00, 0x00, 0x00 (Padding)
     */
    Sevcon_Status_t Sevcon_SetModeOfOperation(Sevcon_Handle_t *handle,
                                              Sevcon_Mode_t mode);

    /**
     * @brief Sets the Target Torque Demand (Object 0x6071) while in Torque Mode.
     *
     * @param[in,out] handle        Pointer to Sevcon instance handle.
     * @param[in]     torque_0p1pct Target torque in tenths of a percent of rated torque
     *                              (-1000 to +1000, where 1000 = 100.0%).
     * @return Sevcon_Status_t SEVCON_OK if within safety limits and sent.
     *
     * @details
     * Safety Checks:
     * - Value is clamped against handle->limits.max_forward_torque_0p1pct and max_reverse_torque_0p1pct.
     * - If drive is not in SEVCON_STATE_OPERATION_ENABLED, non-zero torque commands are rejected.
     *
     * CAN Message Sent (Cyclic RPDO or SDO):
     * - COB-ID:  0x200 + NodeID (Default RPDO1) or custom mapped RPDO
     * - DLC:     8 (or mapped length)
     * - Payload (Assuming standard 8-byte traction RPDO mapping):
     *   - Bytes 0-1: Controlword (0x6040, uint16, Little-Endian, e.g. 0x000F)
     *   - Bytes 2-3: Target Torque (0x6071, int16, Little-Endian, signed)
     *   - Bytes 4-7: Max Motor Speed / Padding
     */
    Sevcon_Status_t Sevcon_SetTorqueDemand(Sevcon_Handle_t *handle,
                                           int16_t torque_0p1pct);

    /**
     * @brief Sets the Maximum Motor Speed (Object 0x6080) for speed limiting while in Torque Mode.
     *
     * @param[in,out] handle    Pointer to Sevcon instance handle.
     * @param[in]     speed_rpm Maximum motor speed in RPM. Internal PI speed loop limits
     *                          runaway motor acceleration when torque is applied.
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * CAN Message Sent (SDO Download to Object 0x6080:00 or mapped in RPDO):
     * - If SDO Download:
     *   - COB-ID:  0x600 + NodeID
     *   - DLC:     8
     *   - Payload:
     *     - Byte 0:   0x23 (SDO write expedited, 4 bytes data)
     *     - Bytes 1-2: 0x6080 (LSB first: 0x80, 0x60)
     *     - Byte 3:   0x00 (Sub-index)
     *     - Bytes 4-7: Speed in RPM (uint32, Little-Endian)
     */
    Sevcon_Status_t Sevcon_SetSpeedLimit(Sevcon_Handle_t *handle,
                                         uint32_t speed_rpm);

    /**
     * @brief Combined Torque Demand and Speed Limiter transmitted in a single cyclic RPDO.
     *
     * @param[in,out] handle        Pointer to Sevcon instance handle.
     * @param[in]     torque_0p1pct Target torque (-1000 to +1000 = -100.0% to +100.0%).
     * @param[in]     speed_rpm     Maximum RPM limit clamp while in torque mode.
     * @return Sevcon_Status_t SEVCON_OK if transmitted.
     *
     * @details
     * Highly recommended for EV traction applications where torque and speed limit
     * must update synchronously in every control loop cycle.
     *
     * CAN Message Sent (RPDO1 or configured RPDO):
     * - COB-ID:  0x200 + NodeID
     * - DLC:     8
     * - Payload:
     *   - Byte 0: Controlword LSB (0x0F)
     *   - Byte 1: Controlword MSB (0x00)
     *   - Byte 2: Target Torque LSB (int16)
     *   - Byte 3: Target Torque MSB
     *   - Byte 4: Max Speed Limit LSB (uint32)
     *   - Byte 5: Max Speed Limit Byte 1
     *   - Byte 6: Max Speed Limit Byte 2
     *   - Byte 7: Max Speed Limit MSB
     */
    Sevcon_Status_t Sevcon_SetTorqueWithSpeedLimit(Sevcon_Handle_t *handle,
                                                   int16_t torque_0p1pct,
                                                   uint32_t speed_rpm);

    /* ========================================================================== */
    /* Safety & Supervisory Functions                                             */
    /* ========================================================================== */

    /**
     * @brief Executes CiA 402 Quick Stop (emergency controlled deceleration).
     *
     * @param[in,out] handle Pointer to Sevcon instance handle.
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * Transitions drive to SEVCON_STATE_QUICK_STOP_ACTIVE.
     * Motor ramps down at quick-stop deceleration rate (0x6085) without immediately
     * opening the high-voltage contactor.
     *
     * CAN Message Sent:
     * - COB-ID:  0x200 + NodeID (RPDO1) or 0x600 + NodeID
     * - DLC:     2 to 8
     * - Payload: Controlword = 0x0002 (Bit 2 cleared: Quick Stop active)
     */
    Sevcon_Status_t Sevcon_QuickStop(Sevcon_Handle_t *handle);

    /**
     * @brief Immediately disables drive voltage and inverter gating (freewheel/open contactor).
     *
     * @param[in,out] handle Pointer to Sevcon instance handle.
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * Transitions drive to SEVCON_STATE_SWITCH_ON_DISABLED.
     * Power bridge is disabled immediately.
     *
     * CAN Message Sent:
     * - COB-ID:  0x200 + NodeID (RPDO1)
     * - DLC:     2 to 8
     * - Payload: Controlword = 0x0000 (Disable Voltage)
     */
    Sevcon_Status_t Sevcon_EmergencyStop(Sevcon_Handle_t *handle);

    /**
     * @brief Resets a latched fault on the Sevcon controller.
     *
     * @param[in,out] handle Pointer to Sevcon instance handle.
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * Performs a 0 -> 1 transition on Bit 7 (Fault Reset) of CiA 402 Controlword (0x6040).
     *
     * CAN Message Sent:
     * - Step 1: Controlword with bit 7 = 0 (e.g. 0x0000)
     * - Step 2: Controlword with bit 7 = 1 (0x0080)
     * - Step 3: Controlword returned to 0x0000 or 0x0006 (Shutdown) to prepare for re-enable.
     */
    Sevcon_Status_t Sevcon_ResetFault(Sevcon_Handle_t *handle);

    /**
     * @brief Configures Sevcon Heartbeat producer time (Object 0x1017:00).
     *
     * @param[in,out] handle      Pointer to Sevcon instance handle.
     * @param[in]     interval_ms Heartbeat interval in ms (0 to disable).
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * CAN Message Sent (SDO Download Expedited):
     * - COB-ID:  0x600 + NodeID
     * - DLC:     8
     * - Payload:
     *   - Byte 0:   0x2B (write 2 bytes data)
     *   - Bytes 1-2: 0x1017 (Index: Heartbeat time)
     *   - Byte 3:   0x00 (Sub-index)
     *   - Bytes 4-5: interval_ms (uint16, Little-Endian)
     *   - Bytes 6-7: 0x00, 0x00
     */
    Sevcon_Status_t Sevcon_ConfigureHeartbeat(Sevcon_Handle_t *handle,
                                              uint16_t interval_ms);

    /**
     * @brief Evaluates watchdog timers and validates received Heartbeat / TPDO timing.
     *
     * @param[in,out] handle Pointer to Sevcon instance handle.
     * @param[in]     now_ms Current system timestamp in milliseconds.
     * @return Sevcon_Status_t SEVCON_OK if within timeout; SEVCON_TIMEOUT if communication lost.
     *
     * @details
     * If now_ms - handle->telemetry.last_rx_timestamp_ms > handle->limits.heartbeat_timeout_ms,
     * this function sets handle->telemetry.is_responsive = false and triggers safety action.
     */
    Sevcon_Status_t Sevcon_CheckCommunicationWatchdog(Sevcon_Handle_t *handle,
                                                      uint32_t now_ms);

    /**
     * @brief Main RX Parser: processes any CAN message received from the Sevcon controller.
     *
     * @param[in,out] handle     Pointer to Sevcon instance handle.
     * @param[in]     cob_id     Received CAN Identifier (standard 11-bit ID).
     * @param[in]     data       Pointer to payload data buffer.
     * @param[in]     dlc        Length of payload received.
     * @param[in]     now_ms     Timestamp in milliseconds.
     * @return Sevcon_Status_t SEVCON_OK if message belonged to this Sevcon and was parsed.
     *
     * @details
     * Handled Message Types:
     * 1. TPDO1 (0x180 + NodeID): Statusword (0x6041, Bytes 0-1) + Mode Display (0x6061, Byte 2)
     * 2. TPDO2 (0x280 + NodeID): Actual Velocity (0x606C, Bytes 0-3) + Actual Torque (0x6077, Bytes 4-5)
     * 3. Heartbeat (0x700 + NodeID): State byte (0x00=Bootup, 0x04=Stopped, 0x05=Operational, 0x7F=Pre-Op)
     * 4. Emergency (0x080 + NodeID): 8-byte CiA 301 EMCY frame:
     *    - Bytes 0-1: Emergency Error Code
     *    - Byte 2:    Error Register (0x1001)
     *    - Bytes 3-7: Manufacturer-specific error information
     */
    Sevcon_Status_t Sevcon_ProcessRxMessage(Sevcon_Handle_t *handle,
                                            uint32_t cob_id,
                                            const uint8_t *data,
                                            uint8_t dlc,
                                            uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif /* CM7_CORE_INC_SEVCON_GEN4_H_ */
#ifndef CM7_CORE_INC_SEVCON_GEN4_H_
#define CM7_CORE_INC_SEVCON_GEN4_H_

#include <stdint.h>
#include <stdbool.h>

#if __has_include("stm32h7xx_hal.h")
#include "stm32h7xx_hal.h"
#elif __has_include("stm32g4xx_hal.h")
#include "stm32g4xx_hal.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/* CANopen Standard COB-ID Definitions (Base Offsets)                        */
/* ========================================================================== */
#define SEVCON_COBID_NMT 0x000U            /* Network Management */
#define SEVCON_COBID_SYNC 0x080U           /* Synchronization */
#define SEVCON_COBID_EMCY_BASE 0x080U      /* Emergency (0x080 + NodeID) */
#define SEVCON_COBID_TPDO1_BASE 0x180U     /* TPDO1 (0x180 + NodeID) */
#define SEVCON_COBID_RPDO1_BASE 0x200U     /* RPDO1 (0x200 + NodeID) */
#define SEVCON_COBID_TPDO2_BASE 0x280U     /* TPDO2 (0x280 + NodeID) */
#define SEVCON_COBID_RPDO2_BASE 0x300U     /* RPDO2 (0x300 + NodeID) */
#define SEVCON_COBID_TPDO3_BASE 0x380U     /* TPDO3 (0x380 + NodeID) */
#define SEVCON_COBID_RPDO3_BASE 0x400U     /* RPDO3 (0x400 + NodeID) */
#define SEVCON_COBID_TPDO4_BASE 0x480U     /* TPDO4 (0x480 + NodeID) */
#define SEVCON_COBID_RPDO4_BASE 0x500U     /* RPDO4 (0x500 + NodeID) */
#define SEVCON_COBID_TSDO_BASE 0x580U      /* SDO Response / Tx (0x580 + NodeID) */
#define SEVCON_COBID_RSDO_BASE 0x600U      /* SDO Request / Rx (0x600 + NodeID) */
#define SEVCON_COBID_HEARTBEAT_BASE 0x700U /* Heartbeat / Node Guard (0x700 + NodeID) */

/* Default Sevcon Node ID (Subject to customer .dcf / DVT configuration) */
#define SEVCON_DEFAULT_NODE_ID 0x01U

/* ========================================================================== */
/* CiA 402 & Sevcon Object Dictionary Indices                                */
/* ========================================================================== */
#define SEVCON_OBJ_DEVICE_TYPE 0x1000U             /* Device Type (uint32) */
#define SEVCON_OBJ_ERROR_REGISTER 0x1001U          /* Error Register (uint8) */
#define SEVCON_OBJ_PRODUCER_HEARTBEAT_TIME 0x1017U /* Producer Heartbeat Time (uint16, ms) */
#define SEVCON_OBJ_CONTROLWORD 0x6040U             /* CiA 402 Controlword (uint16) */
#define SEVCON_OBJ_STATUSWORD 0x6041U              /* CiA 402 Statusword (uint16) */
#define SEVCON_OBJ_VL_TARGET_VELOCITY 0x6042U      /* Target Velocity (int16, rpm) */
#define SEVCON_OBJ_MODES_OF_OPERATION 0x6060U      /* Modes of Operation (int8) */
#define SEVCON_OBJ_MODES_OF_OPERATION_DISP 0x6061U /* Modes of Operation Display (int8) */
#define SEVCON_OBJ_VELOCITY_ACTUAL_VALUE 0x606CU   /* Actual Motor Velocity (int32, rpm) */
#define SEVCON_OBJ_TARGET_TORQUE 0x6071U           /* Target Torque (int16, 0.1% rated) */
#define SEVCON_OBJ_MAX_TORQUE 0x6072U              /* Max Torque (uint16, 0.1% rated) */
#define SEVCON_OBJ_TORQUE_ACTUAL_VALUE 0x6077U     /* Actual Motor Torque (int16, 0.1% rated) */
#define SEVCON_OBJ_MAX_MOTOR_SPEED 0x6080U         /* Max Motor Speed (uint32, rpm limit in torque mode) */
#define SEVCON_OBJ_QUICK_STOP_DECEL 0x6085U        /* Quick Stop Deceleration (uint32) */
#define SEVCON_OBJ_TARGET_VELOCITY 0x60FFU         /* Target Velocity (int32, counts/sec or rpm) */

/* ========================================================================== */
/* CiA 402 Controlword Commands (Object 0x6040)                               */
/* ========================================================================== */
#define SEVCON_CMD_SHUTDOWN 0x0006U         /* Transition 2, 6, 8: Ready to Switch On */
#define SEVCON_CMD_SWITCH_ON 0x0007U        /* Transition 3: Switched On */
#define SEVCON_CMD_ENABLE_OPERATION 0x000FU /* Transition 4: Operation Enabled */
#define SEVCON_CMD_DISABLE_VOLTAGE 0x0000U  /* Transition 7, 9, 10, 12: Switch On Disabled */
#define SEVCON_CMD_QUICK_STOP 0x0002U       /* Transition 7, 10, 11: Quick Stop Active */
#define SEVCON_CMD_FAULT_RESET 0x0080U      /* Bit 7 0->1 transition: Reset Fault */

/* ========================================================================== */
/* CiA 402 Statusword Bitmasks (Object 0x6041)                                */
/* ========================================================================== */
#define SEVCON_STATUS_READY_TO_SWITCH_ON 0x0001U
#define SEVCON_STATUS_SWITCHED_ON 0x0002U
#define SEVCON_STATUS_OPERATION_ENABLED 0x0004U
#define SEVCON_STATUS_FAULT 0x0008U
#define SEVCON_STATUS_VOLTAGE_ENABLED 0x0010U
#define SEVCON_STATUS_QUICK_STOP 0x0020U /* Active low in CiA 402: 0 = Quick Stop active */
#define SEVCON_STATUS_SWITCH_ON_DISABLED 0x0040U
#define SEVCON_STATUS_WARNING 0x0080U
#define SEVCON_STATUS_REMOTE 0x0200U
#define SEVCON_STATUS_TARGET_REACHED 0x0400U
#define SEVCON_STATUS_INTERNAL_LIMIT 0x0800U

/* Statusword State Decoding Mask */
#define SEVCON_STATUS_STATE_MASK 0x006FU

    /* ========================================================================== */
    /* Types and Enumerations                                                     */
    /* ========================================================================== */

    /**
     * @brief Sevcon Driver Status Return Codes
     */
    typedef enum
    {
        SEVCON_OK = 0x00U,
        SEVCON_ERROR = 0x01U,
        SEVCON_INVALID_PARAM = 0x02U,
        SEVCON_NOT_INITIALIZED = 0x03U,
        SEVCON_TIMEOUT = 0x04U,
        SEVCON_BUSY = 0x05U,
        SEVCON_FAULT_ACTIVE = 0x06U,
        SEVCON_STATE_REJECTED = 0x07U,
    } Sevcon_Status_t;

    /**
     * @brief CiA 402 Standard Drive States
     */
    typedef enum
    {
        SEVCON_STATE_NOT_READY_TO_SWITCH_ON = 0x00U,
        SEVCON_STATE_SWITCH_ON_DISABLED = 0x01U,
        SEVCON_STATE_READY_TO_SWITCH_ON = 0x02U,
        SEVCON_STATE_SWITCHED_ON = 0x03U,
        SEVCON_STATE_OPERATION_ENABLED = 0x04U,
        SEVCON_STATE_QUICK_STOP_ACTIVE = 0x05U,
        SEVCON_STATE_FAULT_REACTION_ACTIVE = 0x06U,
        SEVCON_STATE_FAULT = 0x07U,
        SEVCON_STATE_UNKNOWN = 0xFFU
    } Sevcon_State_t;

    /**
     * @brief CiA 402 Modes of Operation (Object 0x6060)
     */
    typedef enum
    {
        SEVCON_MODE_NO_MODE = 0,
        SEVCON_MODE_PROFILE_POSITION = 1,
        SEVCON_MODE_PROFILE_VELOCITY = 3,
        SEVCON_MODE_PROFILE_TORQUE = 4,
    } Sevcon_Mode_t;

    /**
     * @brief CANopen NMT Command Specifiers (Object COB-ID 0x000)
     */
    typedef enum
    {
        SEVCON_NMT_START_NODE = 0x01U,            /* Enter Operational state (enables PDOs) */
        SEVCON_NMT_STOP_NODE = 0x02U,             /* Enter Stopped state */
        SEVCON_NMT_ENTER_PRE_OPERATIONAL = 0x80U, /* Enter Pre-Operational state (SDO only) */
        SEVCON_NMT_RESET_NODE = 0x81U,            /* Software reset of node */
        SEVCON_NMT_RESET_COMMUNICATION = 0x82U,   /* Reset CAN communication stack */
    } Sevcon_NMT_Command_t;

    /**
     * @brief Telemetry Data parsed from Sevcon TPDOs
     */
    typedef struct
    {
        uint16_t statusword;           /* Raw CiA 402 Statusword (0x6041) */
        Sevcon_State_t state;          /* Decoded CiA 402 state */
        Sevcon_Mode_t mode_display;    /* Mode of operation display (0x6061) */
        int16_t actual_torque_0p1pct;  /* Actual torque (0x6077, 0.1% rated) */
        int32_t actual_speed_rpm;      /* Actual speed (0x606C, rpm) */
        uint16_t battery_voltage_0p1v; /* Capacitor/battery voltage (if mapped) */
        int16_t motor_temp_c;          /* Motor temperature in deg C (if mapped) */
        int16_t controller_temp_c;     /* Heatsink temperature in deg C (if mapped) */
        uint16_t last_emergency_code;  /* Last EMCY error code received */
        uint32_t last_rx_timestamp_ms; /* Timestamp of last valid message */
        bool is_responsive;            /* True if heartbeat/TPDO within timeout */
        bool fault_present;            /* True if statusword indicates fault */
    } Sevcon_Telemetry_t;

    /**
     * @brief Safety and Operational Limits Configuration
     */
    typedef struct
    {
        int16_t max_forward_torque_0p1pct; /* Forward torque clamp (e.g. +1000 = +100.0%) */
        int16_t max_reverse_torque_0p1pct; /* Reverse/regen torque clamp (e.g. -1000 = -100.0%) */
        uint32_t max_forward_speed_rpm;    /* Maximum motor speed clamp in RPM (0x6080) */
        uint32_t max_reverse_speed_rpm;    /* Reverse maximum motor speed clamp in RPM */
        uint32_t heartbeat_timeout_ms;     /* Maximum time before flagging comm loss */
        uint32_t command_rate_ms;          /* Expected rate for sending cyclic RPDOs */
        uint32_t quick_stop_decel_rpm_s;   /* Deceleration rate for quick-stop (0x6085) */
    } Sevcon_SafetyLimits_t;

    /**
     * @brief Sevcon Controller Handle Structure
     */
    typedef struct
    {
        FDCAN_HandleTypeDef *hfdcan;  /* STM32 FDCAN peripheral handle */
        uint8_t node_id;              /* Configured CANopen Node ID (1-127) */
        Sevcon_Mode_t active_mode;    /* Current operating mode */
        Sevcon_SafetyLimits_t limits; /* Configured limits and safety parameters */
        Sevcon_Telemetry_t telemetry; /* Latest feedback received from controller */
        uint16_t current_controlword; /* Current command word being sent */
        bool initialized;             /* True once init sequence has completed */
    } Sevcon_Handle_t;

    /* ========================================================================== */
    /* Function Prototypes & CAN Message Specifications                           */
    /* ========================================================================== */

    /**
     * @brief Initializes the Sevcon controller and brings it into Operational state.
     *
     * @param[in,out] handle   Pointer to Sevcon instance handle.
     * @param[in]     hfdcan   Pointer to initialized STM32 FDCAN handle.
     * @param[in]     node_id  CANopen Node ID of Sevcon (typically 0x01).
     * @param[in]     limits   Pointer to safety and operational limit settings.
     * @return Sevcon_Status_t SEVCON_OK on successful initialization sequence.
     *
     * @details
     * CAN Messages Sent:
     * 1. NMT Reset Communication / Node:
     *    - COB-ID:  0x000 (Standard 11-bit)
     *    - DLC:     2
     *    - Payload: [0x82, NodeID] or [0x81, NodeID]
     * 2. NMT Start Remote Node (Enter Operational mode, enables PDOs):
     *    - COB-ID:  0x000
     *    - DLC:     2
     *    - Payload: [0x01, NodeID]
     * 3. CiA 402 State Machine Bring-Up via RPDO1 / SDO (Object 0x6040):
     *    - Step A: Shutdown -> Controlword = 0x0006
     *      Payload: [0x06, 0x00, ...] -> transitions to "Ready to Switch On"
     *    - Step B: Switch On -> Controlword = 0x0007
     *      Payload: [0x07, 0x00, ...] -> transitions to "Switched On"
     *    - Step C: Enable Operation -> Controlword = 0x000F
     *      Payload: [0x0F, 0x00, ...] -> transitions to "Operation Enabled"
     */
    Sevcon_Status_t Sevcon_Init(Sevcon_Handle_t *handle,
                                FDCAN_HandleTypeDef *hfdcan,
                                uint8_t node_id,
                                const Sevcon_SafetyLimits_t *limits);

    /**
     * @brief Safely deinitializes the Sevcon controller and disables torque/voltage.
     *
     * @param[in,out] handle Pointer to Sevcon instance handle.
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * CAN Messages Sent:
     * 1. Zero Demands:
     *    - Sends 0 torque / 0 speed command via RPDO.
     * 2. CiA 402 Disable Voltage:
     *    - COB-ID:  0x200 + NodeID (RPDO1) or 0x600 + NodeID (SDO 0x6040)
     *    - DLC:     2 to 8
     *    - Payload: Controlword = 0x0000 (Disable Voltage / Switch On Disabled)
     * 3. NMT Enter Pre-Operational or Stop:
     *    - COB-ID:  0x000
     *    - DLC:     2
     *    - Payload: [0x80, NodeID] (Pre-Operational) or [0x02, NodeID] (Stopped)
     */
    Sevcon_Status_t Sevcon_Deinit(Sevcon_Handle_t *handle);

    /**
     * @brief Selects the CiA 402 Mode of Operation (Torque vs Velocity).
     *
     * @param[in,out] handle Pointer to Sevcon instance handle.
     * @param[in]     mode   Desired mode (e.g. SEVCON_MODE_PROFILE_TORQUE or SEVCON_MODE_PROFILE_VELOCITY).
     * @return Sevcon_Status_t SEVCON_OK if command was dispatched.
     *
     * @details
     * CAN Message Sent (SDO Download Expedited to Object 0x6060:00):
     * - COB-ID:  0x600 + NodeID
     * - DLC:     8
     * - Payload:
     *   - Byte 0: 0x2F (SDO write expedited, 1 byte data)
     *   - Byte 1: 0x60 (Object Index LSB: 0x6060)
     *   - Byte 2: 0x60 (Object Index MSB)
     *   - Byte 3: 0x00 (Sub-index: 0x00)
     *   - Byte 4: Mode (4 = Profile Torque, 3 = Profile Velocity)
     *   - Bytes 5-7: 0x00, 0x00, 0x00 (Padding)
     */
    Sevcon_Status_t Sevcon_SetModeOfOperation(Sevcon_Handle_t *handle,
                                              Sevcon_Mode_t mode);

    /**
     * @brief Sets the Target Torque Demand (Object 0x6071) while in Torque Mode.
     *
     * @param[in,out] handle        Pointer to Sevcon instance handle.
     * @param[in]     torque_0p1pct Target torque in tenths of a percent of rated torque
     *                              (-1000 to +1000, where 1000 = 100.0%).
     * @return Sevcon_Status_t SEVCON_OK if within safety limits and sent.
     *
     * @details
     * Safety Checks:
     * - Value is clamped against handle->limits.max_forward_torque_0p1pct and max_reverse_torque_0p1pct.
     * - If drive is not in SEVCON_STATE_OPERATION_ENABLED, non-zero torque commands are rejected.
     *
     * CAN Message Sent (Cyclic RPDO or SDO):
     * - COB-ID:  0x200 + NodeID (Default RPDO1) or custom mapped RPDO
     * - DLC:     8 (or mapped length)
     * - Payload (Assuming standard 8-byte traction RPDO mapping):
     *   - Bytes 0-1: Controlword (0x6040, uint16, Little-Endian, e.g. 0x000F)
     *   - Bytes 2-3: Target Torque (0x6071, int16, Little-Endian, signed)
     *   - Bytes 4-7: Max Motor Speed / Padding
     */
    Sevcon_Status_t Sevcon_SetTorqueDemand(Sevcon_Handle_t *handle,
                                           int16_t torque_0p1pct);

    /**
     * @brief Sets the Maximum Motor Speed (Object 0x6080) for speed limiting while in Torque Mode.
     *
     * @param[in,out] handle    Pointer to Sevcon instance handle.
     * @param[in]     speed_rpm Maximum motor speed in RPM. Internal PI speed loop limits
     *                          runaway motor acceleration when torque is applied.
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * CAN Message Sent (SDO Download to Object 0x6080:00 or mapped in RPDO):
     * - If SDO Download:
     *   - COB-ID:  0x600 + NodeID
     *   - DLC:     8
     *   - Payload:
     *     - Byte 0:   0x23 (SDO write expedited, 4 bytes data)
     *     - Bytes 1-2: 0x6080 (LSB first: 0x80, 0x60)
     *     - Byte 3:   0x00 (Sub-index)
     *     - Bytes 4-7: Speed in RPM (uint32, Little-Endian)
     */
    Sevcon_Status_t Sevcon_SetSpeedLimit(Sevcon_Handle_t *handle,
                                         uint32_t speed_rpm);

    /**
     * @brief Combined Torque Demand and Speed Limiter transmitted in a single cyclic RPDO.
     *
     * @param[in,out] handle        Pointer to Sevcon instance handle.
     * @param[in]     torque_0p1pct Target torque (-1000 to +1000 = -100.0% to +100.0%).
     * @param[in]     speed_rpm     Maximum RPM limit clamp while in torque mode.
     * @return Sevcon_Status_t SEVCON_OK if transmitted.
     *
     * @details
     * Highly recommended for EV traction applications where torque and speed limit
     * must update synchronously in every control loop cycle.
     *
     * CAN Message Sent (RPDO1 or configured RPDO):
     * - COB-ID:  0x200 + NodeID
     * - DLC:     8
     * - Payload:
     *   - Byte 0: Controlword LSB (0x0F)
     *   - Byte 1: Controlword MSB (0x00)
     *   - Byte 2: Target Torque LSB (int16)
     *   - Byte 3: Target Torque MSB
     *   - Byte 4: Max Speed Limit LSB (uint32)
     *   - Byte 5: Max Speed Limit Byte 1
     *   - Byte 6: Max Speed Limit Byte 2
     *   - Byte 7: Max Speed Limit MSB
     */
    Sevcon_Status_t Sevcon_SetTorqueWithSpeedLimit(Sevcon_Handle_t *handle,
                                                   int16_t torque_0p1pct,
                                                   uint32_t speed_rpm);

    /**
     * @brief Sets the Target Motor Speed in RPM while in Profile Velocity Mode (Object 0x60FF / 0x6042).
     *
     * @param[in,out] handle        Pointer to Sevcon instance handle.
     * @param[in]     velocity_rpm  Target motor speed in signed RPM (positive = forward, negative = reverse).
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * CAN Message Sent (RPDO or SDO to Object 0x60FF or 0x6042):
     * - If using Target Velocity (Object 0x60FF:00, int32):
     *   - COB-ID:  0x300 + NodeID (RPDO2) or 0x600 + NodeID (SDO)
     *   - DLC:     8
     *   - Bytes 0-1: Controlword (0x6040, uint16)
     *   - Bytes 2-5: Target Velocity (0x60FF, int32, Little-Endian, signed RPM)
     *   - Bytes 6-7: Padding / Acceleration limit
     */
    Sevcon_Status_t Sevcon_SetVelocityDemand(Sevcon_Handle_t *handle,
                                             int32_t velocity_rpm);

    /* ========================================================================== */
    /* Safety & Supervisory Functions                                             */
    /* ========================================================================== */

    /**
     * @brief Executes CiA 402 Quick Stop (emergency controlled deceleration).
     *
     * @param[in,out] handle Pointer to Sevcon instance handle.
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * Transitions drive to SEVCON_STATE_QUICK_STOP_ACTIVE.
     * Motor ramps down at quick-stop deceleration rate (0x6085) without immediately
     * opening the high-voltage contactor.
     *
     * CAN Message Sent:
     * - COB-ID:  0x200 + NodeID (RPDO1) or 0x600 + NodeID
     * - DLC:     2 to 8
     * - Payload: Controlword = 0x0002 (Bit 2 cleared: Quick Stop active)
     */
    Sevcon_Status_t Sevcon_QuickStop(Sevcon_Handle_t *handle);

    /**
     * @brief Immediately disables drive voltage and inverter gating (freewheel/open contactor).
     *
     * @param[in,out] handle Pointer to Sevcon instance handle.
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * Transitions drive to SEVCON_STATE_SWITCH_ON_DISABLED.
     * Power bridge is disabled immediately.
     *
     * CAN Message Sent:
     * - COB-ID:  0x200 + NodeID (RPDO1)
     * - DLC:     2 to 8
     * - Payload: Controlword = 0x0000 (Disable Voltage)
     */
    Sevcon_Status_t Sevcon_EmergencyStop(Sevcon_Handle_t *handle);

    /**
     * @brief Resets a latched fault on the Sevcon controller.
     *
     * @param[in,out] handle Pointer to Sevcon instance handle.
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * Performs a 0 -> 1 transition on Bit 7 (Fault Reset) of CiA 402 Controlword (0x6040).
     *
     * CAN Message Sent:
     * - Step 1: Controlword with bit 7 = 0 (e.g. 0x0000)
     * - Step 2: Controlword with bit 7 = 1 (0x0080)
     * - Step 3: Controlword returned to 0x0000 or 0x0006 (Shutdown) to prepare for re-enable.
     */
    Sevcon_Status_t Sevcon_ResetFault(Sevcon_Handle_t *handle);

    /**
     * @brief Configures Sevcon Heartbeat producer time (Object 0x1017:00).
     *
     * @param[in,out] handle      Pointer to Sevcon instance handle.
     * @param[in]     interval_ms Heartbeat interval in ms (0 to disable).
     * @return Sevcon_Status_t SEVCON_OK on success.
     *
     * @details
     * CAN Message Sent (SDO Download Expedited):
     * - COB-ID:  0x600 + NodeID
     * - DLC:     8
     * - Payload:
     *   - Byte 0:   0x2B (write 2 bytes data)
     *   - Bytes 1-2: 0x1017 (Index: Heartbeat time)
     *   - Byte 3:   0x00 (Sub-index)
     *   - Bytes 4-5: interval_ms (uint16, Little-Endian)
     *   - Bytes 6-7: 0x00, 0x00
     */
    Sevcon_Status_t Sevcon_ConfigureHeartbeat(Sevcon_Handle_t *handle,
                                              uint16_t interval_ms);

    /**
     * @brief Evaluates watchdog timers and validates received Heartbeat / TPDO timing.
     *
     * @param[in,out] handle Pointer to Sevcon instance handle.
     * @param[in]     now_ms Current system timestamp in milliseconds.
     * @return Sevcon_Status_t SEVCON_OK if within timeout; SEVCON_TIMEOUT if communication lost.
     *
     * @details
     * If now_ms - handle->telemetry.last_rx_timestamp_ms > handle->limits.heartbeat_timeout_ms,
     * this function sets handle->telemetry.is_responsive = false and triggers safety action.
     */
    Sevcon_Status_t Sevcon_CheckCommunicationWatchdog(Sevcon_Handle_t *handle,
                                                      uint32_t now_ms);

    /**
     * @brief Main RX Parser: processes any CAN message received from the Sevcon controller.
     *
     * @param[in,out] handle     Pointer to Sevcon instance handle.
     * @param[in]     cob_id     Received CAN Identifier (standard 11-bit ID).
     * @param[in]     data       Pointer to payload data buffer.
     * @param[in]     dlc        Length of payload received.
     * @param[in]     now_ms     Timestamp in milliseconds.
     * @return Sevcon_Status_t SEVCON_OK if message belonged to this Sevcon and was parsed.
     *
     * @details
     * Handled Message Types:
     * 1. TPDO1 (0x180 + NodeID): Statusword (0x6041, Bytes 0-1) + Mode Display (0x6061, Byte 2)
     * 2. TPDO2 (0x280 + NodeID): Actual Velocity (0x606C, Bytes 0-3) + Actual Torque (0x6077, Bytes 4-5)
     * 3. Heartbeat (0x700 + NodeID): State byte (0x00=Bootup, 0x04=Stopped, 0x05=Operational, 0x7F=Pre-Op)
     * 4. Emergency (0x080 + NodeID): 8-byte CiA 301 EMCY frame:
     *    - Bytes 0-1: Emergency Error Code
     *    - Byte 2:    Error Register (0x1001)
     *    - Bytes 3-7: Manufacturer-specific error information
     */
    Sevcon_Status_t Sevcon_ProcessRxMessage(Sevcon_Handle_t *handle,
                                            uint32_t cob_id,
                                            const uint8_t *data,
                                            uint8_t dlc,
                                            uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif /* CM7_CORE_INC_SEVCON_GEN4_H_ */
