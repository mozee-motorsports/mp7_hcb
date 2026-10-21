/*
 * mp7_fdcan.c
 *
 *  Library for setting up FDCAN for mp7 projects
 *
 *  Last Modified: Oct 7, 2026
 *      By Author: montemayorm
 *
 *  Created on: Jan 3, 2026
 *      Author: gundlachj
 */

#include "mp7_fdcan.h"
#include "stm32h7xx_hal_def.h"

#include <stdio.h>

//************************************************************************
// Static variables for periodic messages
//************************************************************************
// Periodic message array and callback function array
static sCAN_PeriodicMessage *periodic_messages[MP7_FDCAN_MAX_PERIODIC_MESSAGES];
static MP7_CAN_TxCallback periodic_callbacks[MP7_FDCAN_MAX_PERIODIC_MESSAGES];
static uint32_t periodic_message_count;

//************************************************************************
// Static variables for watchdogs
//************************************************************************
// Watchdog array and count
static sCAN_Watchdog *watchdogs[MP7_FDCAN_MAX_WATCHDOGS];
static uint32_t watchdog_count;


__weak eCAN_Error MP7_FDCAN_Init(void) {
  // Default implementation does nothing for now
  return CAN_OK;
}

/**
 * @brief Starts the FDCAN peripheral
 * @param hfdcan Pointer to the FDCAN handle
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef MP7_FDCAN_Start(FDCAN_HandleTypeDef *hfdcan) {
  if (hfdcan == NULL) {
    return HAL_ERROR;
  }
  if (HAL_FDCAN_Start(hfdcan) != HAL_OK) {
    MP7_HandleCANError(CAN_FDCAN_INIT_FAILURE);
    return HAL_ERROR;
  }
  return HAL_OK;
}

/**
 * @brief Configures default filter and starts the FDCAN peripheral
 * @param hfdcan Pointer to the FDCAN handle
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef MP7_FDCAN_InitAndStart(FDCAN_HandleTypeDef *hfdcan) {
  if (hfdcan == NULL) {
    return HAL_ERROR;
  }
  if (MP7_FDCAN_ConfigNoFilter(hfdcan) != HAL_OK) {
    MP7_HandleCANError(CAN_FDCAN_INIT_FAILURE);
    return HAL_ERROR;
  }
  return MP7_FDCAN_Start(hfdcan);
}

//************************************************************************
// Functions for sending and receiving CAN messages over FDCAN
//************************************************************************
/**
 * @brief Send a CAN message over FDCAN
 * @param hfdcan Pointer to the FDCAN handle
 * @param message Pointer to the CAN message
 * @return eCAN_Error
 */
eCAN_Error MP7_FDCAN_SendMessage(FDCAN_HandleTypeDef *hfdcan,
                                 const sCAN_Message *message)
{
  FDCAN_TxHeaderTypeDef txHeader;
  FDCAN_RxHeaderTypeDef convertedHeader;

  if ((hfdcan == NULL) || (message == NULL)) {
    return CAN_GENERIC;
  }

  convertedHeader = MP7_ConvertCANHeaderToFDCANHeader(&message->header);
  txHeader.Identifier = convertedHeader.Identifier;
  txHeader.IdType = convertedHeader.IdType;
  txHeader.TxFrameType = FDCAN_DATA_FRAME;
  txHeader.DataLength = convertedHeader.DataLength;
  txHeader.ErrorStateIndicator = convertedHeader.ErrorStateIndicator;
  txHeader.BitRateSwitch = convertedHeader.BitRateSwitch;
  txHeader.FDFormat = convertedHeader.FDFormat;
  txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  txHeader.MessageMarker = 0U;

  if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &txHeader, (uint8_t *)message->data) != HAL_OK) {
    return CAN_FDCAN_TX_FAILURE;
  }

  return CAN_OK;
}

/**
 * @brief Receive a CAN message over FDCAN
 * @param hfdcan Pointer to the FDCAN handle
 * @param message Pointer to the CAN message
 * @return eCAN_Error
 */
eCAN_Error MP7_FDCAN_ReceiveMessage(FDCAN_HandleTypeDef *hfdcan,
                                    sCAN_Message *message){
  FDCAN_RxHeaderTypeDef rxHeader;

  if ((hfdcan == NULL) || (message == NULL)) {
    return CAN_GENERIC;
  }

  if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader, message->data) != HAL_OK) {
    return CAN_FDCAN_RX_FAILURE;
  }

  message->header = MP7_ConvertFDCANHeaderToCANHeader(&rxHeader);
  return CAN_OK;
}

/**
 * @brief Process all pending received messages from an FDCAN RX FIFO
 * @param hfdcan Pointer to the FDCAN handle
 * @param RxFifo FDCAN_RX_FIFO0 or FDCAN_RX_FIFO1
 * @param rx_callback Optional callback to process each received message
 * @param now_ms Current system timestamp in milliseconds
 * @return eCAN_Error
 */
eCAN_Error MP7_FDCAN_ProcessRxMessages(FDCAN_HandleTypeDef *hfdcan,
                                       uint32_t RxFifo,
                                       MP7_CAN_RxCallback rx_callback,
                                       uint32_t now_ms){
  sCAN_Message message;
  eCAN_Error status = CAN_OK;

  if (hfdcan == NULL) {
    return CAN_GENERIC;
  }

  while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, RxFifo) > 0U) {
    if (MP7_FDCAN_ReceiveMessage(hfdcan, &message) != CAN_OK) {
      MP7_HandleCANError(CAN_FDCAN_RX_FAILURE);
      status = CAN_FDCAN_RX_FAILURE;
      break;
    }

    MP7_FDCAN_NotifyCANMessageReceived(&message, now_ms);

    if (rx_callback != NULL) {
      eCAN_Error cb_res = rx_callback(&message);
      if (cb_res != CAN_OK) {
        MP7_HandleCANError(cb_res);
        status = cb_res;
      }
    }
  }

  return status;
}

//************************************************************************
// Filters
//************************************************************************

/**
 * @brief Configures the FDCAN filter to accept all messages
 * @param hfdcan Pointer to the FDCAN handle
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef MP7_FDCAN_ConfigNoFilter(FDCAN_HandleTypeDef *hfdcan){
  FDCAN_FilterTypeDef filterConfig;
  filterConfig.IdType = FDCAN_STANDARD_ID;
  filterConfig.FilterIndex = 0U;
  filterConfig.FilterType = FDCAN_FILTER_MASK;
  filterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  filterConfig.FilterID1 = 0U;
  filterConfig.FilterID2 = 0U;

  return HAL_FDCAN_ConfigFilter(hfdcan, &filterConfig);
}

HAL_StatusTypeDef MP7_FDCAN_ConfigFilterRange(FDCAN_HandleTypeDef *hfdcan,
                                         uint32_t FilterIDRangeLow,
                                         uint32_t FilterIDRangeHigh,
                                         uint32_t FilterIndex,
                                         uint32_t FilterDestinationConfig){
  if (FilterIDRangeLow > FilterIDRangeHigh) {
    hfdcan->ErrorCode = HAL_FDCAN_ERROR_PARAM;
    return HAL_ERROR;
  }
  if (FilterIndex >= hfdcan->Init.StdFiltersNbr) {
    hfdcan->ErrorCode = HAL_FDCAN_ERROR_PARAM;
    return HAL_ERROR;
  }

  FDCAN_FilterTypeDef filterConfig;
  filterConfig.IdType = FDCAN_STANDARD_ID;
  filterConfig.FilterIndex = FilterIndex;
  filterConfig.FilterType = FDCAN_FILTER_RANGE;
  filterConfig.FilterConfig = FilterDestinationConfig;
  filterConfig.FilterID1 = FilterIDRangeLow;
  filterConfig.FilterID2 = FilterIDRangeHigh;

  return HAL_FDCAN_ConfigFilter(hfdcan, &filterConfig);
}

//************************************************************************
// MP7 CAN to FDCAN conversions
//************************************************************************

/**
 * @brief Convert an FDCAN header to an mp7 CAN header
 * @param fdcanHeader Pointer to the FDCAN header
 * @return sCAN_Header
 */
sCAN_Header MP7_ConvertFDCANHeaderToCANHeader(const FDCAN_RxHeaderTypeDef *fdcanHeader){
  sCAN_Header canHeader;

  canHeader.priority = (uint8_t)((fdcanHeader->Identifier >> 8) & 0x07U);
  canHeader.module = BROADCAST;
  canHeader.direction = FROM;
  canHeader.command = (uint8_t)(fdcanHeader->Identifier & 0xFFU);

  return canHeader;
}

/** 
 * @brief Convert an mp7 CAN header to an FDCAN header
 * @param header Pointer to the CAN header
 * @return FDCAN_RxHeaderTypeDef
 */
FDCAN_RxHeaderTypeDef MP7_ConvertCANHeaderToFDCANHeader(const sCAN_Header *header){
  
  FDCAN_RxHeaderTypeDef fdcanHeader;

  fdcanHeader.Identifier = ((uint32_t)(header->priority & 0x07U) << 8) |
                           header->command;
  fdcanHeader.IdType = FDCAN_STANDARD_ID;
  fdcanHeader.RxFrameType = FDCAN_DATA_FRAME;
  fdcanHeader.DataLength = FDCAN_DLC_BYTES_8;
  fdcanHeader.FDFormat = FDCAN_CLASSIC_CAN;
  fdcanHeader.BitRateSwitch = FDCAN_BRS_OFF;
  fdcanHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  fdcanHeader.RxTimestamp = 0U;
  fdcanHeader.FilterIndex = 0U;
  fdcanHeader.IsFilterMatchingFrame = 0U;

  return fdcanHeader;
}

//************************************************************************
// Message Construction Helpers
//************************************************************************

/**
 * @brief Constructs an sCAN_Message
 */
sCAN_Message MP7_CreateCANMessage(uint8_t priority,
                                  eModule module,
                                  eDirection direction,
                                  uint8_t command,
                                  const uint8_t *data,
                                  uint8_t length)
{
  sCAN_Message msg;
  msg.header.priority = priority;
  msg.header.module = module;
  msg.header.direction = direction;
  msg.header.command = command;
  for (uint8_t i = 0U; i < 8U; i++) {
    msg.data[i] = (data != NULL && i < length) ? data[i] : 0U;
  }
  return msg;
}

/**
 * @brief Constructs an sCAN_PeriodicMessage
 */
sCAN_PeriodicMessage MP7_CreatePeriodicMessage(uint8_t priority,
                                               eModule module,
                                               eDirection direction,
                                               uint8_t command,
                                               const uint8_t *data,
                                               uint8_t length,
                                               uint32_t period_ms)
{
  sCAN_PeriodicMessage pmsg;
  pmsg.message = MP7_CreateCANMessage(priority, module, direction, command, data, length);
  pmsg.period_ms = period_ms;
  pmsg.next_due_ms = 0U;
  pmsg.enabled = 1U;
  return pmsg;
}

/**
 * @brief Constructs an sCAN_Watchdog
 */
sCAN_Watchdog MP7_CreateWatchdog(eModule module,
                                 eDirection direction,
                                 uint8_t command,
                                 uint32_t timeout_ms,
                                 MP7_CAN_WatchdogCallback callback)
{
  sCAN_Watchdog wd;
  wd.module = module;
  wd.direction = direction;
  wd.command = command;
  wd.timeout_ms = timeout_ms;
  wd.deadline_ms = 0U;
  wd.timeout_callback = callback;
  wd.enabled = 1U;
  wd.expired = 0U;
  return wd;
}

//************************************************************************
// Periodic messages
//************************************************************************
eCAN_Error MP7_FDCAN_RegisterPeriodicMessage(sCAN_PeriodicMessage *periodic_message,
                                            MP7_CAN_TxCallback tx_callback){
  if ((periodic_message == NULL) || (tx_callback == NULL) ||
      (periodic_message->period_ms == 0U)) {
    return CAN_GENERIC;
  }
  if (periodic_message_count >= MP7_FDCAN_MAX_PERIODIC_MESSAGES) {
    return CAN_GENERIC;
  }

  periodic_messages[periodic_message_count] = periodic_message;
  periodic_callbacks[periodic_message_count] = tx_callback;
  periodic_message->enabled = 1U;
  periodic_message->next_due_ms = 0U;
  periodic_message_count++;
  return CAN_OK;
}

eCAN_Error MP7_FDCAN_ProcessPeriodicMessages(FDCAN_HandleTypeDef *hfdcan,
                                             uint32_t now_ms){
  uint32_t index;

  if (hfdcan == NULL) {
    return CAN_GENERIC;
  }

  for (index = 0U; index < periodic_message_count; index++) {
    sCAN_PeriodicMessage *periodic_message = periodic_messages[index];

    if ((periodic_message == NULL) || (periodic_message->enabled == 0U) ||
        ((int32_t)(now_ms - periodic_message->next_due_ms) < 0)) {
      continue;
    }

    if (periodic_callbacks[index] != NULL) {
      if (periodic_callbacks[index](hfdcan, &periodic_message->message) != CAN_OK) {
        MP7_HandleCANError(CAN_FDCAN_TX_FAILURE);
      }
    }
    periodic_message->next_due_ms += periodic_message->period_ms;
  }
  return CAN_OK;
}

//************************************************************************
// Watchdog and error handling
//************************************************************************
eCAN_Error MP7_FDCAN_RegisterWatchdog(sCAN_Watchdog *watchdog){
  if ((watchdog == NULL) || (watchdog->timeout_ms == 0U) ||
      (watchdog->timeout_callback == NULL)) {
    return CAN_GENERIC;
  }
  if (watchdog_count >= MP7_FDCAN_MAX_WATCHDOGS) {
    return CAN_GENERIC;
  }

  watchdog->enabled = 1U;
  watchdog->expired = 0U;
  watchdog->deadline_ms = 0U;
  watchdogs[watchdog_count] = watchdog;
  watchdog_count++;
  return CAN_OK;
}

eCAN_Error MP7_FDCAN_CheckWatchdogs(uint32_t now_ms){
  for (uint32_t i = 0U; i < watchdog_count; i++) {
    sCAN_Watchdog *watchdog = watchdogs[i];

    if ((watchdog == NULL) || (watchdog->enabled == 0U) || (watchdog->expired == 1U)) {
      continue;
    }

    if (((int32_t)(now_ms - watchdog->deadline_ms) >= 0) &&
        (watchdog->deadline_ms != 0U)) {
      watchdog->expired = 1U;
      if (watchdog->timeout_callback != NULL) {
        watchdog->timeout_callback();
      }
    }
  }
  return CAN_OK;
}

void MP7_FDCAN_NotifyCANMessageReceived(const sCAN_Message *message,
                                        uint32_t now_ms){
  uint32_t index;

  if (message == NULL) {
    return;
  }

  for (index = 0U; index < watchdog_count; index++) {
    sCAN_Watchdog *watchdog = watchdogs[index];

    if ((watchdog != NULL) && (watchdog->enabled != 0U) &&
        (watchdog->module == message->header.module) &&
        (watchdog->direction == message->header.direction) &&
        (watchdog->command == message->header.command)) {
      watchdog->deadline_ms = now_ms + watchdog->timeout_ms;
      watchdog->expired = 0U;
    }
  }
}

void MP7_HandleCANError(eCAN_Error error_code)
{
  (void)error_code;
  return; // TODO: Implement error handling
}

// existing functions from Jacob

/**
 * @brief   Configures the global FDCAN filter to reject all messages
 *          with IDs that do not match any applied filters.
 * @param   hfdcan address of FDCAN_HandleTypeDef used for
 *          configuring the FDCAN peripheral.
 * @retval  HAL_StatusTypeDef based on the completion
 *          of the filter configuration.
 */
HAL_StatusTypeDef MP7_FDCAN_ConfigureGlobalFilter(FDCAN_HandleTypeDef *hfdcan) {
  return HAL_FDCAN_ConfigGlobalFilter(  hfdcan,
                        FDCAN_REJECT,
                        FDCAN_REJECT,
                        FDCAN_REJECT_REMOTE,
                        FDCAN_REJECT_REMOTE);
}

/**
 *  @brief  Configures a new standard filter to send/reject messages based on an ID range.
 *          Note: MUST be called BEFORE the FDCAN peripheral has been started.
 *  @param  hfdcan address of FDCAN_HandleTypeDef used for configuring the FDCAN peripheral.
 *  @param  FilterIDRangeLow lowest ID value that will be filtered.
 *  @param  FilterIDRangeHigh highest ID value that will be filtered.
 *  @param  FilterIndex index for the new filter for the list of standard filters.
 *          Note: FilterIndex must be less than StdFiltersNbr from the FDCAN Handler.
 *  @param  FilterDestinationConfig value of FDCAN_FILTER_xxx for what to do with the message.
 *  @retval HAL_StatusTypeDef based on the success of configuring the filter.
 *          If the filter ranges or index are invalid, then hfdcan.ErrorCode is set to
 *          HAL_FDCAN_ERROR_PARAM and a HAL_ERROR is returned.
 */
HAL_StatusTypeDef MP7_FDCAN_ConfigureFilter(FDCAN_HandleTypeDef *hfdcan, uint32_t FilterIDRangeLow,
                                            uint32_t FilterIDRangeHigh, uint32_t FilterIndex,
                                            uint32_t FilterDestinationConfig) {

  if (FilterIDRangeLow > FilterIDRangeHigh) {
    hfdcan->ErrorCode = HAL_FDCAN_ERROR_PARAM;
    return HAL_ERROR;
  }
  if (FilterIndex > hfdcan->Init.StdFiltersNbr) {
    hfdcan->ErrorCode = HAL_FDCAN_ERROR_PARAM;
    return HAL_ERROR;
  }

  FDCAN_FilterTypeDef filterConfig;
  filterConfig.IdType = FDCAN_STANDARD_ID;
  filterConfig.FilterConfig = FilterDestinationConfig;
  filterConfig.FilterType = FDCAN_FILTER_RANGE;
  filterConfig.FilterIndex = FilterIndex;
  filterConfig.FilterID1 = FilterIDRangeLow;
  filterConfig.FilterID2 = FilterIDRangeHigh;

  return HAL_FDCAN_ConfigFilter(hfdcan, &filterConfig);
}

/**
 * @brief   Configures a given FDCAN_TxHeaderTypeDef with the default MP7 configuration.
 * @param   TxHeader address of an FDCAN_TxHeaderTypeDef for configuring.
 * @param   Identifier specifies the ID to attach to the message.
 * @param   DataLength specifies the size of the message data.
 * @retval  1 if dataLength is greater than the max size for messages; otherwise, 0.
 */
uint8_t MP7_FDCAN_ConfigureTxHeader(FDCAN_TxHeaderTypeDef *TxHeader, uint32_t Identifier,
                                    uint32_t DataLength) {

  if (DataLength > FDCAN_DLC_BYTES_8) {
    return 1;
  }

  TxHeader->Identifier = Identifier;
  TxHeader->DataLength = DataLength;
  TxHeader->IdType = FDCAN_STANDARD_ID;
  TxHeader->FDFormat = FDCAN_CLASSIC_CAN;
  TxHeader->BitRateSwitch = FDCAN_BRS_OFF;
  TxHeader->TxFrameType = FDCAN_DATA_FRAME;
  TxHeader->ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader->TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  TxHeader->MessageMarker = 0;

  return 0;
}
