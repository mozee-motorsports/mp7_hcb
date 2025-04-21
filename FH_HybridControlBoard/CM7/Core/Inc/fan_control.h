/******************************************************************************
 * fan_control.h
 *
 * Description:	PWM control for fans on the two cooling
 * 				loops.
 *
 * Needed Peripherals: TIM
 *
 *  Created on: Feb 9, 2025
 *      Author: nimsgernd
 *****************************************************************************/

// include guard
#ifndef INC_FAN_CONTROL_H_
#define INC_FAN_CONTROL_H_

// define this as extern for c++
#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 |          Includes
 ============================================================================*/

#include <stdint.h>

#include "tim.h"

/*=============================================================================
 |          Defines
 ============================================================================*/

#define PWM_RESOLUTION 100

extern uint32_t GPIO_PWM_VAL[PWM_RESOLUTION];

/*=============================================================================
 |          Enumerations
 ============================================================================*/

typedef enum
{
	FAN1 = TIM_CHANNEL_1,
	FAN2 = TIM_CHANNEL_2
} FANS;

/*=============================================================================
 |          typeDefs
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

extern int fan_output(FANS fan, uint8_t percent);

// end c++ guard
#ifdef __cplusplus
}
#endif

// end include guard
#endif /* INC_TEMPERTURE_SENSORS_H_ */
