/******************************************************************************
 * temperature_sensors.c
 *
 *  Created on: Feb 9, 2025
 *      Author: nimsgernd
 *****************************************************************************/

/*=============================================================================
 |          Includes
 ============================================================================*/

#include "temperature_sensor.h"

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
unsigned int read_adc() {
    // Replace this with your actual ADC reading code
    return 32768;  // Example value (mid-scale)
}

// Function to convert ADC value to resistance
float adc_to_resistance(unsigned int adc_value) {
    double v_out = (adc_value / ADC_MAX_VALUE) * V_REF;
    double resistance = (V_REF * R_FIXED / v_out) - R_FIXED;
    return resistance;
}

// Function to convert resistance to temperature (Steinhart-Hart equation)
float resistance_to_temperature(float resistance) {
    float steinhart;
    steinhart = resistance / R_25C;  // (R/R_25C)
    steinhart = log(steinhart);  // ln(R/R_25C)
    steinhart /= B_COEFFICIENT;  // 1/B * ln(R/R_25C)
    steinhart += 1.0 / (25.0 + 273.15);  // + (1/T_25)
    steinhart = 1.0 / steinhart;  // Invert
    steinhart -= 273.15;  // Convert to Celsius
    return steinhart;
}
