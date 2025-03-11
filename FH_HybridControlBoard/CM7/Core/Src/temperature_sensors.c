/******************************************************************************
 * temperature_sensors.c
 *
 *  Created on: Feb 9, 2025
 *      Author: nimsgernd
 *****************************************************************************/

/*=============================================================================
 |          Includes
 ============================================================================*/

#include "temperature_sensors.h"

/*=============================================================================
 |          Defines
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/



/*=============================================================================
 |          Function Definitions
 ============================================================================*/

// Function to read the ADC value (mock function, replace with actual ADC reading)
uint16_t read_adc(TEMP_PROBES probe)
{
	switch (probe) {
		case CORE:

			break;
		case AMBIENT:

			break;
		case IC_LOOP:

			break;
		case TS_LOOP:

			break;
		default:
			break;
	}
	return 0;
}

// Function to convert ADC value to resistance
float adc_to_resistance(TEMP_PROBES probe, uint16_t adcValue)
{
	float vOut = (adcValue / UINT16_MAX) * V_REF;
	float resistance = 0;
	switch (probe) {
		case CORE:
			resistance = (V_REF * R_FIXED / vOut) - R_FIXED;
			break;
		case AMBIENT:
			resistance = (V_REF * R_FIXED / vOut) - R_FIXED;
			break;
		case IC_LOOP:
			resistance = (V_REF * R_FIXED / vOut) - R_FIXED;
			break;
		case TS_LOOP:
			resistance = (V_REF * R_FIXED / vOut) - R_FIXED;
			break;
		default:
			break;
	}
    return resistance;
}

// Function to convert resistance to temperature (Steinhart-Hart equation)
float resistance_to_temperature(TEMP_PROBES probe, float resistance) {
    float steinhart;
    steinhart = resistance / R_25C;  // (R/R_25C)
    steinhart = log(steinhart);  // ln(R/R_25C)
    steinhart /= B_COEFFICIENT;  // 1/B * ln(R/R_25C)
    steinhart += 1.0 / (25.0 + 273.15);  // + (1/T_25)
    steinhart = 1.0 / steinhart;  // Invert
    steinhart -= 273.15;  // Convert to Celsius
    return steinhart;
}

float get_temperture(TEMP_PROBES probe)
{
	switch (probe) {
		case CORE:
			return resistance_to_temperature(probe, adc_to_resistance(probe, read_adc(probe)));
			break;
		case AMBIENT:
			return resistance_to_temperature(probe, adc_to_resistance(probe, read_adc(probe)));
			break;
		case IC_LOOP:
			return resistance_to_temperature(probe, adc_to_resistance(probe, read_adc(probe)));
			break;
		case TS_LOOP:
			return resistance_to_temperature(probe, adc_to_resistance(probe, read_adc(probe)));
			break;
		default:
			break;
	}
	return 0;
}
