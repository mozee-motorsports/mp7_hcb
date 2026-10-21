/*
 * mp7_fdcan.h
 *
 *  Library for setting up FDCAN for mp7 projects
 *
 *  Last Modified: Oct 18, 2026
 *      By Author: montemayorm
 *
 *  Created on: Jan 3, 2026
 *      Author: gundlachj
 */

#ifndef SRC_MP7_FDCAN_H_
#define SRC_MP7_FDCAN_H_

#include <stdint.h>

#if __has_include("stm32g4xx_hal.h")
#include "stm32g4xx_hal.h"
#elif __has_include("stm32h7xx_hal.h")
#include "stm32h7xx_hal.h"
#else
#error "HAL Library Files Not Found"
#endif
//************************************************************************
//* MP7 CAN Library
//* This library provides a set of functions for configuring and using the 
//* FDCAN peripheral on MP7 projects. It includes functions for setting up 
//* global filters, configuring individual filters, and handling received
//* CAN messages. The library also defines structures for CAN message 
//* headers and payloads, allowing for easy manipulation of CAN messages 
//* in the application code.
//************************************************************************

//************************************************************************
// Defines
//************************************************************************
#define MP7_FDCAN_MAX_PERIODIC_MESSAGES 8U
#define MP7_FDCAN_MAX_WATCHDOGS 8U
#define HCB_MAX_PERIODIC_MESSAGES MP7_FDCAN_MAX_PERIODIC_MESSAGES
#define HCB_MAX_WATCHDOGS MP7_FDCAN_MAX_WATCHDOGS

/* 
 * Typedefs and Enums
 * define the structures and enumerations used in the MP7 CAN library.
 */

/*
 * MP7 Modules
 * Module ID should typically match the primary consumer of the message,
 * unless the message is for data logging, then it should be the producer
 * of the message.
 */
typedef enum {
    SAFETY_SYSTEM = 0,          // Shutdown Circuit Board
    BROADCAST = 1,              // broadcast message, should be consumed by all modules
    HYBRID_CONTROL_BOARD = 2,   // Hybrid Control Board
    THROTTLE_CONTROL_BOARD = 3, // Throttle Body controller
    PEDAL_BOX = 4,              // Pedal Box Board
    SEVCON = 5,                 // SEVCON Inverter
} eModule;

/*
 * Message Direction
 * TO: Message is being sent to the module
 * FROM: Message is being sent from the module
 */
typedef enum {
    TO = 0,
    FROM = 1,
} eDirection;

typedef enum {
    CAN_OK                  = 0x00U,
    CAN_INIT_FAILURE        = 0x01U,
    CAN_GENERIC             = 0x02U,
    CAN_MISMATCH_DLC        = 0x03U,
    CAN_INVALID_COMMAND     = 0x04U,
    CAN_FDCAN_INIT_FAILURE  = 0x05U,
    CAN_FDCAN_RX_FAILURE    = 0x06U,
    CAN_FDCAN_TX_FAILURE    = 0x07U,
    CAN_INVALID_MODULE      = 0x08U,
} eCAN_Error;

/*
 * CAN Header
 * This structure defines the header for each CAN message.
 */
typedef struct {
    uint8_t priority;       // 0-7, 0 is highest priority
    eModule module;         // Module ID
    eDirection direction;   // Message Direction
    uint8_t command;        // Command ID, specific to the module
} sCAN_Header; 

/*
 * CAN Message
 * This structure defines a complete CAN message, including the header and data.
 */
typedef struct {
    sCAN_Header header;
    uint8_t data[8];        // Data payload, up to 8 bytes
} sCAN_Message;

/*
 * Periodic CAN Messages
 * These are messages that are sent at regular intervals.
 */
typedef struct {
    sCAN_Message message;
    uint32_t period_ms;     // Period in milliseconds
    uint32_t next_due_ms;   // Timestamp when the message is next due
    uint8_t enabled;
} sCAN_PeriodicMessage;

/*
 * CAN TX Callback
 * This is a function pointer type for a callback function that will be called 
 * when a CAN message is transmitted.
 */
typedef eCAN_Error (*MP7_CAN_TxCallback)(FDCAN_HandleTypeDef *hfdcan,
                                         const sCAN_Message *message);

/*
 * CAN RX Callback
 * This is a function pointer type for a callback function that will be called 
 * when a CAN message is received.
 */
typedef eCAN_Error (*MP7_CAN_RxCallback)(const sCAN_Message *message);

/*
 * CAN Watchdog Callback
 * This is a function pointer type for a callback function that will be called 
 * when a CAN message times out.
 */
typedef void (*MP7_CAN_WatchdogCallback)(void);

/*
 * CAN Watchdog
 * This structure defines a watchdog for a CAN message.
 */
typedef struct {
    eModule module;
    eDirection direction;
    uint8_t command;
    uint32_t timeout_ms;
    uint32_t deadline_ms;
    MP7_CAN_WatchdogCallback timeout_callback;
    uint8_t enabled;
    uint8_t expired;
} sCAN_Watchdog;

/*
 * Function Prototypes
 */
__weak eCAN_Error MP7_FDCAN_Init(void);

// Peripheral Control & Startup
HAL_StatusTypeDef MP7_FDCAN_Start(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef MP7_FDCAN_InitAndStart(FDCAN_HandleTypeDef *hfdcan);

// Functions for sending and receiving CAN messages over FDCAN
eCAN_Error MP7_FDCAN_SendMessage(FDCAN_HandleTypeDef *hfdcan,
                                 const sCAN_Message *message);
eCAN_Error MP7_FDCAN_ReceiveMessage(FDCAN_HandleTypeDef *hfdcan,
                                    sCAN_Message *message);
eCAN_Error MP7_FDCAN_ProcessRxMessages(FDCAN_HandleTypeDef *hfdcan,
                                       uint32_t RxFifo,
                                       MP7_CAN_RxCallback rx_callback,
                                       uint32_t now_ms);

// Filters
HAL_StatusTypeDef MP7_FDCAN_ConfigNoFilter(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef MP7_FDCAN_ConfigFilterRange(FDCAN_HandleTypeDef *hfdcan,
                                              uint32_t FilterIDRangeLow,
                                              uint32_t FilterIDRangeHigh,
                                              uint32_t FilterIndex,
                                              uint32_t FilterDestinationConfig);

// MP7 CAN to FDCAN conversions
sCAN_Header MP7_ConvertFDCANHeaderToCANHeader(const FDCAN_RxHeaderTypeDef *fdcanHeader);
FDCAN_RxHeaderTypeDef MP7_ConvertCANHeaderToFDCANHeader(const sCAN_Header *header);

// Message Construction Helpers
sCAN_Message MP7_CreateCANMessage(uint8_t priority,
                                  eModule module,
                                  eDirection direction,
                                  uint8_t command,
                                  const uint8_t *data,
                                  uint8_t length);
sCAN_PeriodicMessage MP7_CreatePeriodicMessage(uint8_t priority,
                                               eModule module,
                                               eDirection direction,
                                               uint8_t command,
                                               const uint8_t *data,
                                               uint8_t length,
                                               uint32_t period_ms);
sCAN_Watchdog MP7_CreateWatchdog(eModule module,
                                 eDirection direction,
                                 uint8_t command,
                                 uint32_t timeout_ms,
                                 MP7_CAN_WatchdogCallback callback);

//Periodic messages
eCAN_Error MP7_FDCAN_RegisterPeriodicMessage(sCAN_PeriodicMessage *periodic_message,
                                            MP7_CAN_TxCallback tx_callback);
eCAN_Error MP7_FDCAN_ProcessPeriodicMessages(FDCAN_HandleTypeDef *hfdcan,
                                             uint32_t now_ms);

//Watchdog and error handling
eCAN_Error MP7_FDCAN_RegisterWatchdog(sCAN_Watchdog *watchdog);
eCAN_Error MP7_FDCAN_CheckWatchdogs(uint32_t now_ms);
void MP7_FDCAN_NotifyCANMessageReceived(const sCAN_Message *message,
                                        uint32_t now_ms);

void MP7_HandleCANError(eCAN_Error error_code);


// existing functions made by jacob
HAL_StatusTypeDef MP7_FDCAN_ConfigureGlobalFilter(FDCAN_HandleTypeDef *hfdcan);

HAL_StatusTypeDef MP7_FDCAN_ConfigureFilter(FDCAN_HandleTypeDef *hfdcan,
                                            uint32_t FilterIDRangeLow,
                                            uint32_t FilterIDRangeHigh,
                                            uint32_t FilterIndex,
                                            uint32_t FilterDestinationConfig);

uint8_t MP7_FDCAN_ConfigureTxHeader(FDCAN_TxHeaderTypeDef *TxHeader,
                                 uint32_t Identifier,
                                 uint32_t DataLength);


#endif /* SRC_MP7_FDCAN_H_ */
