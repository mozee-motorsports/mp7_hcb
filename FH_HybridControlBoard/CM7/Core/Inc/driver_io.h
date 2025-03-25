/******************************************************************************
 * driver_io.h
 *
 * Description:	GPIO controller reading driver inputs and
 * 				writing output signals to the dash board.
 *
 * Needed Peripherals: NONE (GPIO)
 *
 *  Created on: Feb 9, 2025
 *      Author: nimsgernd
 *****************************************************************************/

// include guard
#ifndef INC_DRIVER_IO_H_
#define INC_DRIVER_IO_H_

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

#define LED_ON			(bool)	true
#define LED_OFF			(bool)	false
#define BUTTON_CLOSED	(bool)	true
#define BUTTON_OPEN		(bool)	false

/*=============================================================================
 |          Enumerations
 ============================================================================*/

// Enumerations for which LED to set
typedef enum
{
	ReadyToDrive,
	HC1,
	HC2,
	CANError
}LED;

// Enumerations for which LED to set
typedef enum
{
	Starter,
	FuelPump,
	Ignition,
	ReadyToDrive,
	HybridControl,
	UserButton
}BUTTON;

/*=============================================================================
 |          typeDefs
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

int set_led(LED led, bool state);
bool read_button(BUTTON button);

// end c++ guard
#ifdef __cplusplus
}
#endif

// end include guard
#endif /* INC_TEMPERTURE_SENSORS_H_ */
