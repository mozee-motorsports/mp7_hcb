/******************************************************************************
 * freq_sensors.h
 *
 * Description:	PWM input from the inductive RPM sensor and wheel speed sensor.
 *
 * Needed Peripherals: TIM (TIC)
 *
 *  Created on: Feb 9, 2025
 *      Author: nimsgernd
 *****************************************************************************/

// include guard
#ifndef INC_FREQ_SENSORS_H_
#define INC_FREQ_SENSORS_H_

// define this as extern for c++
#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 |          Includes
 ============================================================================*/

#include <stdint.h>
#include <stdbool.h>

//#include "stm32h7xx_hal_tim.h"
#include "tim.h"

/*=============================================================================
 |          Defines
 ============================================================================*/

#define TIMCLOCK   200000000
#define PRESCALAR  200

/*=============================================================================
 |          Enumerations
 ============================================================================*/



/*=============================================================================
 |          typeDefs
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

extern void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
extern uint16_t read_RPM();
extern uint16_t read_wheel_speed();

// end c++ guard
#ifdef __cplusplus
}
#endif

// end include guard
#endif /* INC_TEMPERTURE_SENSORS_H_ */
