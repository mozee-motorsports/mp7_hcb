/******************************************************************************
 * can_bus.c
 *
 * Description:	API to interact with the vehicle CAN bus
 *
 * Needed Peripherals: FDCAN
 *
 *  Created on: Apr 17, 2025
 *      Author: nimsgernd
 *****************************************************************************/

/*=============================================================================
 |          Includes
 ============================================================================*/

#include "can_bus.h"

/*=============================================================================
 |          Defines
 ============================================================================*/

#define MAX_FDCAN_RETRIES 5

#define MAX_3BIT 0b111
#define MAX_4BIT 0b1111

#define FILTER 	0b00000010000										// Steering Wheel ID
#define MASK 	0b00011110000										// 11b id. Only look at Module bits [7..5] and direction (should be set to "to" = 0)

/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

HAL_StatusTypeDef fdcanWrite(FDCAN_HandleTypeDef* hfdcan, volatile FDCAN_TxHeaderTypeDef* tx_header, volatile uint8_t* txData, uint8_t len, MODULE module, DIRECTION direction, uint8_t priority, COMMAND command);
HAL_StatusTypeDef fdcanInit(FDCAN_HandleTypeDef* hfdcan1, FDCAN_HandleTypeDef* hfdcan2);
HAL_StatusTypeDef fdcanFilterInit(FDCAN_HandleTypeDef* hfdcan1, volatile FDCAN_TxHeaderTypeDef* tx_header1, FDCAN_HandleTypeDef* hfdcan2, volatile FDCAN_TxHeaderTypeDef* tx_header2);

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs);

/*=============================================================================
 |          File Scope Variables
 ============================================================================*/

static volatile bool can_init_done = false;

// FDCAN1 Variables
volatile FDCAN_RxHeaderTypeDef rx_header1;
volatile uint8_t rx_data1[8];

// FDCAN2 Variables
volatile FDCAN_RxHeaderTypeDef rx_header2;
volatile uint8_t rx_data2[8];

/*=============================================================================
 |          Function Definitions
 ============================================================================*/

/**
 * The write function for writing data to the CAN bus in the proper format
 */
HAL_StatusTypeDef fdcanWrite(FDCAN_HandleTypeDef* hfdcan, volatile FDCAN_TxHeaderTypeDef* tx_header, volatile uint8_t* txData, uint8_t len, MODULE module, DIRECTION direction, uint8_t priority, COMMAND command)
{
	// Verify that the actual payload (txData) is not greater than 8 bytes (max for classic CAN)
	if(len > FDCAN_DLC_BYTES_8)
		return HAL_ERROR;

	// Verify that priority, module, and direction are not greater than 3 bits, and command isn't greater than 4 bits
	if(priority > MAX_3BIT || module > MAX_3BIT || direction > MAX_3BIT || command > MAX_4BIT)
		return HAL_ERROR;

	// Set DLC
	tx_header -> DataLength = len;

	// Create 11b id
	tx_header -> Identifier = ((uint16_t)priority << 8) | (uint8_t)(module << 5) | (uint8_t)(direction << 4) | (uint8_t)command;

	// Transmit message by putting it into 10 element TxFIFO queue
	int retry = 0;
	while (retry < MAX_FDCAN_RETRIES)
	{
		uint32_t error = HAL_FDCAN_GetError(hfdcan);
		uint32_t tx_space = HAL_FDCAN_GetTxFifoFreeLevel(hfdcan); // Get the number of messages in the Tx FIFO queue
		uint32_t rx_space = HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0); // Get the number of messages in the Rx FIFO queue (for safety)

	  if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, tx_header, *(uint8_t*)txData) == HAL_OK)
	  {
		return HAL_OK; // Success
	  }
	  retry++;
	}

	return HAL_ERROR;
}

/**
 * The custom initialization function for other CAN peripheral settings
 */
HAL_StatusTypeDef fdcanInit(FDCAN_HandleTypeDef* hfdcan1, FDCAN_HandleTypeDef* hfdcan2)
{
	// FDCAN 1 - Rx FIFO0
	if (HAL_FDCAN_Start(hfdcan1) != HAL_OK)
		return HAL_ERROR;

	// FDCAN 2 - Rx FIFO1
	if (HAL_FDCAN_Start(hfdcan2) != HAL_OK)
		return HAL_ERROR;

	// Triggers interrupt when new message appears in RX_FIFO0
	if (HAL_FDCAN_ActivateNotification(hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0) != HAL_OK)
		return HAL_ERROR;

	// Triggers interrupt when new message appears in RX_FIFO1
	if (HAL_FDCAN_ActivateNotification(hfdcan2, FDCAN_IT_RX_FIFO1_NEW_MESSAGE,0) != HAL_OK)
		return HAL_ERROR;

	return HAL_OK;
}

/**
 * Custom CAN filter initializer
 */
HAL_StatusTypeDef fdcanFilterInit(FDCAN_HandleTypeDef* hfdcan1, volatile FDCAN_TxHeaderTypeDef* tx_header1, FDCAN_HandleTypeDef* hfdcan2, volatile FDCAN_TxHeaderTypeDef* tx_header2)
{
	static FDCAN_FilterTypeDef fdcan1_filter_config;
	static FDCAN_FilterTypeDef fdcan2_filter_config;

	// 11b register with 11b id. We only care about bits [7..5]
	// FDCAN1 Mask Filter
	fdcan1_filter_config.IdType = FDCAN_STANDARD_ID;					// Using standard IDs, not extended IDs
	fdcan1_filter_config.FilterIndex = 0;								// We are only using 1 filter so index = 0
	fdcan1_filter_config.FilterType = FDCAN_FILTER_MASK;				// Using mask filter
	fdcan1_filter_config.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;		// Messages that pass through the filter should be sent to RX FIFO 0
	fdcan1_filter_config.FilterID1 = FILTER;							// We only care about bits [7..5]
	fdcan1_filter_config.FilterID2 = MASK;								// Mask bits
	if (HAL_FDCAN_ConfigFilter(hfdcan1, &fdcan1_filter_config) != HAL_OK)
	  return HAL_ERROR; // Filter configuration Error

	// Explicitly reject non-matching messages
	HAL_StatusTypeDef ret = HAL_FDCAN_ConfigGlobalFilter(hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
	if (ret != HAL_OK)
	  return HAL_ERROR;

	// Configure TX Header for FDCAN1
//	txHeader1.Identifier = SW_MODULE;									// Recall header format - changed dynamically
	tx_header1->IdType = FDCAN_STANDARD_ID;								// Using standard IDs, not extended IDs
	tx_header1->TxFrameType = FDCAN_DATA_FRAME;							// Sending a Data frame not a Remote frame
//	txHeader1.DataLength = FDCAN_DLC_BYTES_8;							// Data length is classic CAN - 8 bytes
	tx_header1->ErrorStateIndicator = FDCAN_ESI_ACTIVE;					// Notify us if there is any error in transmission
	tx_header1->BitRateSwitch = FDCAN_BRS_OFF;							// Will use same bit rate for both Arbitration and Data fields
	tx_header1->FDFormat = FDCAN_CLASSIC_CAN;							// Using standard CAN not FDCAN
	tx_header1->TxEventFifoControl = FDCAN_NO_TX_EVENTS;				// Not using TxEvent
	tx_header1->MessageMarker = 0;										// Not using MessageMarker

	// FDCAN2 Mask Filter
	fdcan2_filter_config.IdType = FDCAN_STANDARD_ID;					// Using standard IDs, not extended IDs
	fdcan2_filter_config.FilterIndex = 0;								// We are only using 1 filter so index = 0
	fdcan2_filter_config.FilterType = FDCAN_FILTER_MASK;				// Using mask filter
	fdcan2_filter_config.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;		// Messages that pass through the filter should be sent to RX FIFO 0
	fdcan2_filter_config.FilterID1 = FILTER;							// ID to look for
	fdcan2_filter_config.FilterID2 = MASK;								// Mask bits
	if (HAL_FDCAN_ConfigFilter(hfdcan2, &fdcan2_filter_config) != HAL_OK)
	  return HAL_ERROR; // Filter configuration Error
		// Explicitly reject non-matching messages
	if (HAL_FDCAN_ConfigGlobalFilter(hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE) != HAL_OK)
	  return HAL_ERROR;

	// Configure TX Header for FDCAN2
//	txHeader2.Identifier = SW_MODULE;									// Recall header format - changed dynamically
	tx_header2->IdType = FDCAN_STANDARD_ID;								// Using standard IDs, not extended IDs
	tx_header2->TxFrameType = FDCAN_DATA_FRAME;							// Sending a Data frame not a Remote frame
//	txHeader2.DataLength = FDCAN_DLC_BYTES_8;							// Data length is classic CAN - 8 bytes
	tx_header2->ErrorStateIndicator = FDCAN_ESI_ACTIVE;					// Notify us if there is any error in transmission
	tx_header2->BitRateSwitch = FDCAN_BRS_OFF;							// Will use same bit rate for both Arbitration and Data fields
	tx_header2->FDFormat = FDCAN_CLASSIC_CAN;							// Using standard CAN not FDCAN
	tx_header2->TxEventFifoControl = FDCAN_NO_TX_EVENTS;				// Not using TxEvent
	tx_header2->MessageMarker = 0;										// Not using MessageMarker

	return HAL_OK;
}

/**
 * CAN Rx FIFO0 call back
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	if (!can_init_done)
		return;
	if (hfdcan->Instance == FDCAN1)
	{
		// If we are receiving a CAN signal and its NOT in reset, then get message
		if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
	    {
			// Get Rx messages from RX FIFO0
			if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header1, rx_data1) != HAL_OK)
			{
//				Error error = {fdcan_rx_failure, getErrorPriority(fdcan_rx_failure), false, false, false, HAL_GetTick(), false};
//				addErrorFromISR(error, internal_error_queueHandle);
			}

			// Activate notification again in case HAL deactivates interrupt
			if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
			{
//				Error error = {fdcan_rx_failure, getErrorPriority(fdcan_rx_failure), false, false, false, HAL_GetTick(), false};
//				addErrorFromISR(error, internal_error_queueHandle);
			}

			// Extract command
			//      Command command = (Command)(rx_header1.Identifier & 0x0F);

			// Queue non-critical commands
			CANMessage msg = {rx_header1, {0}};

			// Make deep copy of payload
			memcpy(msg.data, rx_data1, 8);

			// ISR-safe queue put with timeout = 0
			osStatus_t status = osMessageQueuePut(fdcan1_queueHandle, &msg, 0, 0);		// Timeout must be 0 to be ISR safe
			if (status != osOK)
			{
//				Error error = {fdcan_tx_failure, getErrorPriority(fdcan_tx_failure), false, false, false, HAL_GetTick(), false};
//				addErrorFromISR(error, internal_error_queueHandle);
			}
	    }
	}
}

/**
 * CAN Rx FIFO1 call back
 */
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
	if (!can_init_done)
		return;

	if (hfdcan->Instance == FDCAN2)
	{
		// If we are receiving a CAN signal and its NOT in reset, then get message
	    if ((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != RESET)
	    {
	    	// Get Rx messages from RX FIFO1
	    	if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &rx_header2, rx_data2) != HAL_OK)
	    	{
//	    		Error error = {fdcan_rx_failure, getErrorPriority(fdcan_rx_failure), false, false, false, HAL_GetTick(), false};
//	    		addErrorFromISR(error, internal_error_queueHandle);
	    	}

	    	// Activate notification again in case HAL deactivates interrupt
	    	if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0) != HAL_OK)
	    	{
//	    		Error error = {fdcan_rx_failure, getErrorPriority(fdcan_rx_failure), false, false, false, HAL_GetTick(), false};
//	    		addErrorFromISR(error, internal_error_queueHandle);

	    	}
	    	// Extract command
	    	//      Command command = (Command)(rx_header2.Identifier & 0x0F);

	    	// Queue non-critical commands
	    	CANMessage msg = {rx_header2, {0}};

	    	// Make deep copy of payload
	    	memcpy(msg.data, rx_data2, 8);

	    	// Add to queue
	    	// ISR-safe queue put with timeout = 0
	    	osStatus_t status = osMessageQueuePut(fdcan2_queueHandle, &msg, 0, 0);		// Timeout must be 0 to be ISR safe
	    	if (status != osOK)
	    	{
	    		// Handle queue full or error (optional)
//	    		Error error = {fdcan_tx_failure, getErrorPriority(fdcan_tx_failure), false, false, false, HAL_GetTick(), false};
//	    		addError(error, internal_error_queueHandle, mutex_internal_errorHandle);
	    	}
	    }
	}
}
