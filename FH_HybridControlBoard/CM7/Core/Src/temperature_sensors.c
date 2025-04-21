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
	uint16_t adcRaw = 0;
	switch (probe) {
		case CORE:
			adcRaw = ADC_VAL3[5];
			break;
		case AMBIENT:
			adcRaw = ADC_VAL2[1];
			break;
		case IC_LOOP:
			adcRaw = ADC_VAL3[0];
			break;
		case TS_LOOP:
			adcRaw = ADC_VAL3[1];
			break;
		default:
			break;
	}
	return adcRaw;
}

// Function to convert ADC value to resistance
float adc_to_resistance(TEMP_PROBES probe, uint16_t adcValue)
{
	float vOut = ((float) adcValue / (float) UINT16_MAX) * V_REF;
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
float resistance_to_temperature(TEMP_PROBES probe, float resistance)
{
    float steinhart = 0;
    switch (probe) {
    	case CORE:
    		steinhart = resistance / R_25C;  // (R/R_25C)
    		steinhart = log(steinhart);  // ln(R/R_25C)
    		steinhart /= B_COEFFICIENT;  // 1/B * ln(R/R_25C)
    		break;
    	case AMBIENT:
    		steinhart = resistance / R_25C;  // (R/R_25C)
    		steinhart = log(steinhart);  // ln(R/R_25C)
    		steinhart /= B_COEFFICIENT;  // 1/B * ln(R/R_25C)
    		break;
    	case IC_LOOP:
    		steinhart = resistance / R_25C;  // (R/R_25C)
    		steinhart = log(steinhart);  // ln(R/R_25C)
    		steinhart /= B_COEFFICIENT;  // 1/B * ln(R/R_25C)
    		break;
    	case TS_LOOP:
    		steinhart = resistance / R_25C;  // (R/R_25C)
    		steinhart = log(steinhart);  // ln(R/R_25C)
    		steinhart /= B_COEFFICIENT;  // 1/B * ln(R/R_25C)
    		break;
    	default:
    		break;
    }
    steinhart += 1.0 / (25.0 + 273.15);  // + (1/T_25)
    steinhart = 1.0 / steinhart;  // Invert
    steinhart -= 273.15;  // Convert to Celsius
    return steinhart;
}

float read_temp(TEMP_PROBES probe)
{
	float temperature = 0;
	switch (probe) {
		case CORE:
			temperature = resistance_to_temperature(CORE, adc_to_resistance(CORE, read_adc(CORE)));
			break;
		case AMBIENT:
			temperature = resistance_to_temperature(AMBIENT, adc_to_resistance(AMBIENT, read_adc(AMBIENT)));
			break;
		case IC_LOOP:
			temperature = resistance_to_temperature(IC_LOOP, adc_to_resistance(IC_LOOP, read_adc(IC_LOOP)));
			break;
		case TS_LOOP:
			temperature = resistance_to_temperature(TS_LOOP, adc_to_resistance(TS_LOOP, read_adc(TS_LOOP)));
			break;
		default:
			break;
	}
	return temperature;
}
