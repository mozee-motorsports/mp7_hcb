/******************************************************************************
 * can_bus.h
 *
 * Description:	API to interact with the vehicle CAN bus
 *
 * Needed Peripherals: FDCAN1 and FDCAN2
 *
 *  Created on: Apr 13, 2025
 *      Author: nimsgernd
 *****************************************************************************/

#ifndef INC_CAN_BUS_H_
#define INC_CAN_BUS_H_

// define this as extern for c++
#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 |          Includes
 ============================================================================*/

#include <stdint.h>
#include <stdbool.h>

#include "fdcan.h"
#include "cmsis_os2.h"

/*=============================================================================
 |          Defines
 ============================================================================*/



/*=============================================================================
 |          Enumerations
 ============================================================================*/

typedef enum
{
	safety_system		= (uint8_t) 0,
	broadcast			= (uint8_t) 1,
	ic_throttle_control	= (uint8_t) 2,
	peddal_box			= (uint8_t) 3,
	steering_wheel		= (uint8_t) 4,
}MODULE;

typedef enum
{
	to 		= (uint8_t) 0,
	from	= (uint8_t) 1
}DIRECTION;

typedef enum
{
	safety_system_shutdown 					= (uint8_t) 0,
	safety_system_status_report 			= (uint8_t) 1,
	safety_system_error_report 				= (uint8_t) 2,
	safety_system_accelerometer_data 		= (uint8_t) 3,
	safety_system_voltage_stage_report 		= (uint8_t) 4,
	ic_throttle_control_shutdown 			= (uint8_t) 0,
	ic_throttle_control_status_report 		= (uint8_t) 1,
	ic_throttle_control_error_report 		= (uint8_t) 2,
	ic_throttle_control_throttle_percent	= (uint8_t) 3,
	peddal_box_shutdown 					= (uint8_t) 0,
	peddal_box_status_report 				= (uint8_t) 1,
	peddal_box_error_report 				= (uint8_t) 2,
	peddal_box_accelerometer_data 			= (uint8_t) 3,
	peddal_box_accelerator_pedal_percent 	= (uint8_t) 4,
	steering_wheel_shutdown 				= (uint8_t) 0,
	steering_wheel_status_report 			= (uint8_t) 1,
	steering_wheel_error_report 			= (uint8_t) 2,
	steering_wheel_send_vehicle_error 		= (uint8_t) 3,
	steering_wheel_set_time 				= (uint8_t) 4,
	steering_wheel_reverse_lockout 			= (uint8_t) 5,
	steering_wheel_gear_shift 				= (uint8_t) 6,
	steering_wheel_primary_data 			= (uint8_t) 7,
	steering_wheel_fuel_level 				= (uint8_t) 8,
	steering_wheel_hybrid_power_ratio 		= (uint8_t) 9,
	steering_wheel_battery_percentages 		= (uint8_t) 10,
	steering_wheel_temperatures 			= (uint8_t) 11,
	steering_wheel_GPS_position 			= (uint8_t) 12,
	steering_wheel_altitude_and_heading 	= (uint8_t) 13,
	steering_wheel_accelerometer_data 		= (uint8_t) 14,
	steering_wheel_distance 				= (uint8_t)	15
}COMMAND;

typedef enum
{
	shutdown 				= (uint8_t) 0,
	status_report 			= (uint8_t) 3,
	error_report 			= (uint8_t) 1,
	throttle_percentage 	= (uint8_t) 1,
	shutdown_loop_report	= (uint8_t) 1,
	vehicle_error_code 		= (uint8_t) 2,
	set_time 				= (uint8_t) 8,
	get_reverse_lockout 	= (uint8_t) 1,
	gear_shift 				= (uint8_t) 1,
	primary_data 			= (uint8_t) 5,
	fuel_level 				= (uint8_t) 1,
	hybrid_power_ratio 		= (uint8_t) 1,
	battery_percentages 	= (uint8_t) 2,
	temperatures 			= (uint8_t) 6,
	GPS_position 			= (uint8_t) 8,
	altitude_and_headding 	= (uint8_t) 6,
	accelerometer_data 		= (uint8_t) 6,
	distance 				= (uint8_t) 4
}DLC;

/*=============================================================================
 |          Struct
 ============================================================================*/

typedef struct{
	FDCAN_RxHeaderTypeDef rx_header;
	uint8_t data[8];
}CANMessage;

/*=============================================================================
 |          Public Variables
 ============================================================================*/

/* Definitions for fdcan1_queue */
extern osMessageQueueId_t fdcan1_queueHandle;

/* Definitions for fdcan2_queue */
extern osMessageQueueId_t fdcan2_queueHandle;

/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

HAL_StatusTypeDef fdcanWrite(FDCAN_HandleTypeDef* hfdcan, volatile FDCAN_TxHeaderTypeDef* tx_header, volatile uint8_t* txData, uint8_t len, MODULE module, DIRECTION direction, uint8_t priority, COMMAND command);
HAL_StatusTypeDef fdcanInit(FDCAN_HandleTypeDef* hfdcan1, FDCAN_HandleTypeDef* hfdcan2);
HAL_StatusTypeDef fdcanFilterInit(FDCAN_HandleTypeDef* hfdcan1, volatile FDCAN_TxHeaderTypeDef* tx_header1, FDCAN_HandleTypeDef* hfdcan2, volatile FDCAN_TxHeaderTypeDef* tx_header2);

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs);

// end c++ guard
#ifdef __cplusplus
}
#endif

#endif /* INC_CAN_BUS_H_ */
