/******************************************************************************
 * FET_control.h
 *
 * Description:	GPIO controller for turning on and off
 * 				external FETs.
 *
 * Needed Peripherals: NONE (GPIO)
 *
 *  Created on: Feb 9, 2025
 *      Author: nimsgernd
 *****************************************************************************/

// include guard
#ifndef INC_FET_CONTROL_H_
#define INC_FET_CONTROL_H_

// define this as extern for c++
#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 |          Includes
 ============================================================================*/

#include <stdbool.h>

/*=============================================================================
 |          Defines
 ============================================================================*/

#define FET_ON	(bool)	true
#define FET_OFF	(bool)	false

/*=============================================================================
 |          Enumerations
 ============================================================================*/

// Enumerations for which FET to set
typedef enum
{
	Starter,
	FuelPump,
	Ignition,
	FET1,
	FET2,
	FET3,
	FET4
}FET;

/*=============================================================================
 |          typeDefs
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

int set_fet(FET fet, bool state);

// end c++ guard
#ifdef __cplusplus
}
#endif

// end include guard
#endif /* INC_TEMPERTURE_SENSORS_H_ */
