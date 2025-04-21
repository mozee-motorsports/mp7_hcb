/******************************************************************************
 * temperature_sensors.h
 *
 * Description:	API to interact with temperature sensors to read
 * 				micro-controller core temperature, ambient air temperature, and
 * 				both liquid cooling loops. (Steinhart-Hart equation)
 *
 * Needed Peripherals: ADC
 *
 *  Created on: Feb 9, 2025
 *      Author: nimsgernd
 *****************************************************************************/

// include guard
#ifndef INC_TEMPERATURE_SENSORS_H_
#define INC_TEMPERATURE_SENSORS_H_

// define this as extern for c++
#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 |          Includes
 ============================================================================*/

#include <stdint.h>
#include <math.h>

#include "adc.h"

/*=============================================================================
 |          Defines
 ============================================================================*/

/* Reference voltage */
#define V_REF	(float)	3.3

/* Core probe defines */
//#define VREFINT_CAL_ADDR	0x1FF1E860

#define MAX_CORE_RAW	(uint16_t)	UINT16_MAX
#define MIN_CORE_RAW	(uint16_t)	33760

#define MAX_CORE_TEMP	(float)		140		// C
#define MIN_CORE_TEMP	(float)		-40		// C

/* Ambient probe defines */
#define MAX_AMBI_RAW	(uint16_t)	UINT16_MAX
#define MIN_AMBI_RAW	(uint16_t)	0

#define MAX_AMBI_TEMP	(float)		125		// C
#define MIN_AMBI_TEMP	(float)		-40		// C

/* Cooling loop defines */
#define MAX_LOOP_RAW	(uint16_t)	UINT16_MAX
#define MIN_LOOP_RAW	(uint16_t)	0

#define MAX_LOOP_TEMP	(float)		120		// C
#define MIN_LOOP_TEMP	(float)		-40		// C

#define R_FIXED 		(float)		10000.0  // Fixed resistor value in ohms (10K ohms)

// Thermistor characteristics at specific temperatures
#define R_25C			(float)	10000.0  // Resistance at 25C in ohms
#define B_COEFFICIENT	(float)	3950.0  // Beta coefficient of the thermistor

/*=============================================================================
 |          Enumerations
 ============================================================================*/

// Enumerations for which probe to read
typedef enum
{
	CORE,
	AMBIENT,
	IC_LOOP,
	TS_LOOP
}TEMP_PROBES;

/*=============================================================================
 |          typeDefs
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

/**
 *
 */
extern float read_temp(TEMP_PROBES channel);

// end c++ guard
#ifdef __cplusplus
}
#endif

// end include guard
#endif /* INC_TEMPERATURE_SENSORS_H_ */
