/******************************************************************************
 * clutch_sensor.h
 *
 * Description:	Analog input from clutch pressure sensor.
 *
 * Needed Peripherals: ADC
 *
 *  Created on: Feb 9, 2025
 *      Author: nimsgernd
 *****************************************************************************/

// include guard
#ifndef INC_CLUTCH_SENSOR_H_
#define INC_CLUTCH_SENSOR_H_

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

// Clutch pressure sensor defines
#define MAX_CLUTCH_RAW	(uint16_t)	65535
#define MIN_CLUTCH_RAW	(uint16_t)	0

#define MAX_CLUTCH_PSI	(float)		3000
#define MIN_CLUTCH_PSI	(float)		0

/*=============================================================================
 |          Enumerations
 ============================================================================*/



/*=============================================================================
 |          typeDefs
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

float read_clutch_sensor();

// end c++ guard
#ifdef __cplusplus
}
#endif

// end include guard
#endif /* INC_TEMPERTURE_SENSORS_H_ */
