/*
 * hcb.c
 *
 * Library for the hybrid control board
 * 
 *  Last Modified: Oct 19, 2026
 *      By Author: montemayorm
 *
 *  Created on: Oct 10, 2023
 *      Author: montemayorm
 */

#include "mp7_hcb.h"
#include "fdcan.h"
#include "main.h"

#include <stdio.h>
#include <stdbool.h>

/*
 * RX Handler Prototypes
 */
static eCAN_Error handleSafetySystemMessage(const sCAN_Message *message);
static eCAN_Error handleBroadcastMessage(const sCAN_Message *message);
static eCAN_Error handleHybridControlBoardMessage(const sCAN_Message *message);
static eCAN_Error handleThrottleControlBoardMessage(const sCAN_Message *message);
static eCAN_Error handlePedalBoxMessage(const sCAN_Message *message);
static eCAN_Error handleSevconMessage(const sCAN_Message *message);

//************************************************************************
// Initialization
//************************************************************************

/**
 * @brief Initializes the FDCAN peripheral and registers periodic messages
 * @return eCAN_Error
 */
void FDCAN_Init(void){
  if (MP7_FDCAN_InitAndStart(&hfdcan1) != HAL_OK ||
      MP7_FDCAN_InitAndStart(&hfdcan2) != HAL_OK){
    printf("FDCAN init/start failed\r\n");
    Error_Handler();
  }
}

/**
  * @brief Initializes the HCB module, including FDCAN and periodic messages
  * @retval None
  */
void HCB_Init(void){
  FDCAN_Init();
}

/**
 * @brief Handle a received CAN message
 * @param message Pointer to the received CAN message
 * @return eCAN_Error
 */
eCAN_Error HCB_HandleCANRxMessage(const sCAN_Message *message) {
  // Check if the message is valid
  if (message == NULL) {
    return CAN_GENERIC;
  }

  // process CAN messages
  switch (message->header.module) {
    case SAFETY_SYSTEM:
      return handleSafetySystemMessage(message);
    case BROADCAST:
      return handleBroadcastMessage(message);
    case HYBRID_CONTROL_BOARD:
      return handleHybridControlBoardMessage(message);
    case THROTTLE_CONTROL_BOARD:
      return handleThrottleControlBoardMessage(message);
    case PEDAL_BOX:
      return handlePedalBoxMessage(message);
    case SEVCON:
      return handleSevconMessage(message);
    default:
      return CAN_INVALID_MODULE; // Unknown module
  }

  return CAN_OK;
}

/*
 * Message RX Handlers
 */
static eCAN_Error handleSafetySystemMessage(const sCAN_Message *message) {
  // ignore safety system messages for now
  (void)message; // Suppress unused variable warning
  return CAN_OK;
}

static eCAN_Error handleBroadcastMessage(const sCAN_Message *message) {
  // ignore broadcast messages for now
  (void)message; // Suppress unused variable warning
  return CAN_OK;
}

static eCAN_Error handleHybridControlBoardMessage(const sCAN_Message *message) {
  // ignore hybrid control board messages for now
  (void)message; // Suppress unused variable warning
  return CAN_OK;
}

static eCAN_Error handleThrottleControlBoardMessage(const sCAN_Message *message) {
  // ignore throttle control board messages for now
  (void)message; // Suppress unused variable warning
  return CAN_OK;
}

static eCAN_Error handlePedalBoxMessage(const sCAN_Message *message) {
  // ignore pedal box messages for now
  (void)message; // Suppress unused variable warning
  return CAN_OK;
}

static eCAN_Error handleSevconMessage(const sCAN_Message *message) {
  // ignore SEVCON messages for now
  (void)message; // Suppress unused variable warning
  return CAN_OK;
}

/*
 * Transmit Things
 */

//************************************************************************
// Message Definitions
//************************************************************************

// r2d broadcast message
// sends out r2d message every 500ms
sCAN_PeriodicMessage r2d_broadcast_message = {
  .message = {
    .header = {
      .priority = 0,
      .module = BROADCAST,
      .direction = FROM,
      .command = 0x01, // R2D broadcast command
    },
    .data = {0}, // No data for this message
  },
  .period_ms = 500, // Send every 500ms
  .next_due_ms = 0,
  .enabled = 1U,
};

//************************************************************************
// Transmit Functions
//************************************************************************

/**
 * @brief Registers the R2D broadcast message for periodic transmission
 * @return eCAN_Error
 */
eCAN_Error HCB_RegisterR2DPeriodicTransmit(void) {
  // Register the R2D broadcast message
  return MP7_FDCAN_RegisterPeriodicMessage(&r2d_broadcast_message,
                                           MP7_FDCAN_SendMessage);
}




