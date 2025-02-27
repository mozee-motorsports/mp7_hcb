/******************************************************************************
 * brake_sensor.h
 *
 * Description:	Analog input from brake pressure sensor.
 *
 * Needed Peripherals: ADC
 *
 *  Created on: Feb 9, 2025
 *      Author: nimsgernd
 *****************************************************************************/

// include guard
#ifndef INC_BRAKE_SENSOR_H_
#define INC_BRAKE_SENSOR_H_

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

// Brake pressure sensor defines
#define MAX_BRAKE_RAW	(uint16_t)	65535
#define	MIN_BRAKE_RAW	(uint16_t)	0

#define MAX_BRAKE_PSI	(float)		10000
#define MIN_BRAKE_PSI	(float)		-14.5

/*=============================================================================
 |          Enumerations
 ============================================================================*/



/*=============================================================================
 |          typeDefs
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

float read_brake_sensor();

// end c++ guard
#ifdef __cplusplus
}
#endif

// end include guard
#endif /* INC_TEMPERTURE_SENSORS_H_ */
