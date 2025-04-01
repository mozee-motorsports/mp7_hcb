/******************************************************************************
 * driver_io.c
 *
 *  Created on: Apr 1, 2025
 *      Author: nimsgernd
 *****************************************************************************/

/*=============================================================================
 |          Includes
 ============================================================================*/

#include "driver_io.h"

/*=============================================================================
 |          Defines
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

int set_led(LED led, bool state);
bool read_button(BUTTON button);

/*=============================================================================
 |          File Scope Variables
 ============================================================================*/



/*=============================================================================
 |          Function Definitions
 ============================================================================*/

int set_led(LED led, bool state)
{
	int returnCode = 0;
	switch (led) {
		case ReadyToDriveLED:
			if (state == LED_ON)
			{
				HAL_GPIO_WritePin(R2D_LED_GPIO_Port, R2D_LED_Pin, GPIO_PIN_SET);
			}
			else if (state == LED_OFF)
			{
				HAL_GPIO_WritePin(R2D_LED_GPIO_Port, R2D_LED_Pin, GPIO_PIN_RESET);
			}
			else
			{
				returnCode = -1;
			}
			break;
		case HC1LED:
			if (state == LED_ON)
			{
				HAL_GPIO_WritePin(HC1_LED_GPIO_Port, HC1_LED_Pin, GPIO_PIN_SET);
			}
			else if (state == LED_OFF)
			{
				HAL_GPIO_WritePin(HC1_LED_GPIO_Port, HC1_LED_Pin, GPIO_PIN_RESET);
			}
			else
			{
				returnCode = -1;
			}
			break;
		case HC2LED:
			if (state == LED_ON)
			{
				HAL_GPIO_WritePin(HC2_LED_GPIO_Port, R2D_LED_Pin, GPIO_PIN_SET);
			}
			else if (state == LED_OFF)
			{
				HAL_GPIO_WritePin(HC2_LED_GPIO_Port, R2D_LED_Pin, GPIO_PIN_RESET);
			}
			else
			{
				returnCode = -1;
			}
			break;
		case CANErrorLED:
			if (state == LED_ON)
			{
				HAL_GPIO_WritePin(CAN_Error_GPIO_Port, CAN_Error_Pin, GPIO_PIN_SET);
			}
			else if (state == LED_OFF)
			{
				HAL_GPIO_WritePin(CAN_Error_GPIO_Port, CAN_Error_Pin, GPIO_PIN_RESET);
			}
			else
			{
				returnCode = -1;
			}
			break;
		default:
			returnCode = 2;
			break;
	}

	return returnCode;
}

bool read_button(BUTTON button)
{
	switch (button) {
		case StarterButton:
			if (HAL_GPIO_ReadPin(Starter_Switch_GPIO_Port, Starter_Switch_Pin) == GPIO_PIN_SET)
			{
				return BUTTON_CLOSED;
			}
			else
			{
				return BUTTON_OPEN;
			}
			break;
		case FuelPumpButton:
			if (HAL_GPIO_ReadPin(FP_Switch_GPIO_Port, FP_Switch_Pin) == GPIO_PIN_SET)
			{
				return BUTTON_CLOSED;
			}
			else
			{
				return BUTTON_OPEN;
			}
			break;
		case IgnitionButton:
			if (HAL_GPIO_ReadPin(Ignition_Switch_GPIO_Port, Ignition_Switch_Pin) == GPIO_PIN_SET)
			{
				return BUTTON_CLOSED;
			}
			else
			{
				return BUTTON_OPEN;
			}
			break;
		case ReadyToDriveButton:
			if (HAL_GPIO_ReadPin(R2D_Switch_GPIO_Port, R2D_Switch_Pin) == GPIO_PIN_SET)
			{
				return BUTTON_CLOSED;
			}
			else
			{
				return BUTTON_OPEN;
			}
			break;
		case HybridControlButton:
			if (HAL_GPIO_ReadPin(HC_Switch_GPIO_Port, HC_Switch_Pin) == GPIO_PIN_SET)
			{
				return BUTTON_CLOSED;
			}
			else
			{
				return BUTTON_OPEN;
			}
			break;
		case UserButton:
			if (HAL_GPIO_ReadPin(User_Switch_GPIO_Port, User_Switch_Pin) == GPIO_PIN_SET)
			{
				return BUTTON_CLOSED;
			}
			else
			{
				return BUTTON_OPEN;
			}
			break;
		default:
			return BUTTON_OPEN;
			break;
	}

	return BUTTON_OPEN;
}
