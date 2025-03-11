/******************************************************************************
 * low_voltage_battery_sensor.h
 *
 * Description:	Analog reading for the Low Voltage battery
 * 				voltage.
 *
 * Needed Peripherals: ADC
 *
 *  Created on: Feb 9, 2025
 *      Author: nimsgernd
 *****************************************************************************/

// include guard
#ifndef INC_LOW_VOLTAGE_BATTERY_SENSOR_H_
#define INC_LOW_VOLTAGE_BATTERY_SENSOR_H_

// define this as extern for c++
#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 |          Includes
 ============================================================================*/

#include <stdint.h>

/*=============================================================================
 |          Defines
 ============================================================================*/

// Low Voltage Battery defines
#define MAX_LVBAT_RAW	(uint16_t)	65535
#define MIN_LVBAT_RAW	(uint16_t)	0

#define MAX_LVBAT_V		(unsigned float)	14.8	// V
#define MIN_LVBAT_V		(unsigned float)	12.5	// V

/*=============================================================================
 |          Enumerations
 ============================================================================*/



/*=============================================================================
 |          typeDefs
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

unsigned float read_LVbatt();

// end c++ guard
#ifdef __cplusplus
}
#endif

// end include guard
#endif /* INC_TEMPERTURE_SENSORS_H_ */
