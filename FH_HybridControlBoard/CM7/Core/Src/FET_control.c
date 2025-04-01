/******************************************************************************
 * FET_control.c
 *
 *  Created on: Apr 1, 2025
 *      Author: nimsgernd
 *****************************************************************************/

/*=============================================================================
 |          Includes
 ============================================================================*/

#include "FET_control.h"

/*=============================================================================
 |          Defines
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

int set_fet(FET fet, bool state);

/*=============================================================================
 |          File Scope Variables
 ============================================================================*/



/*=============================================================================
 |          Function Definitions
 ============================================================================*/

int set_fet(FET fet, bool state)
{
	int returnCode = 0;
	switch (fet) {
		case StarterFET:
			if (state == FET_ON)
			{
				HAL_GPIO_WritePin(Starter_Out_GPIO_Port, Starter_Out_Pin, GPIO_PIN_SET);
			}
			else if (state == FET_OFF)
			{
				HAL_GPIO_WritePin(Starter_Out_GPIO_Port, Starter_Out_Pin, GPIO_PIN_RESET);
			}
			else
			{
				returnCode = -1;
			}
			break;
		case FuelPumpFET:
			if (state == FET_ON)
			{
				HAL_GPIO_WritePin(FP_Out_GPIO_Port, FP_Out_Pin, GPIO_PIN_SET);
			}
			else if (state == FET_OFF)
			{
				HAL_GPIO_WritePin(FP_Out_GPIO_Port, FP_Out_Pin, GPIO_PIN_RESET);
			}
			else
			{
				returnCode = -1;
			}
			break;
		case IgnitionFET:
			if (state == FET_ON)
			{
				HAL_GPIO_WritePin(Ignition_Out_GPIO_Port, Ignition_Out_Pin, GPIO_PIN_SET);
			}
			else if (state == FET_OFF)
			{
				HAL_GPIO_WritePin(Ignition_Out_GPIO_Port, Ignition_Out_Pin, GPIO_PIN_RESET);
			}
			else
			{
				returnCode = -1;
			}
			break;
		case FET1:
			if (state == FET_ON)
			{
				HAL_GPIO_WritePin(FET1_GPIO_Port, FET1_Pin, GPIO_PIN_SET);
			}
			else if (state == FET_OFF)
			{
				HAL_GPIO_WritePin(FET1_GPIO_Port, FET1_Pin, GPIO_PIN_RESET);
			}
			else
			{
				returnCode = -1;
			}
			break;
		case FET2:
			if (state == FET_ON)
			{
				HAL_GPIO_WritePin(FET2_GPIO_Port, FET2_Pin, GPIO_PIN_SET);
			}
			else if (state == FET_OFF)
			{
				HAL_GPIO_WritePin(FET2_GPIO_Port, FET2_Pin, GPIO_PIN_RESET);
			}
			else
			{
				returnCode = -1;
			}
			break;
		case FET3:
			if (state == FET_ON)
			{
				HAL_GPIO_WritePin(FET3_GPIO_Port, FET3_Pin, GPIO_PIN_SET);
			}
			else if (state == FET_OFF)
			{
				HAL_GPIO_WritePin(FET3_GPIO_Port, FET3_Pin, GPIO_PIN_RESET);
			}
			else
			{
				returnCode = -1;
			}
			break;
		case FET4:
			if (state == FET_ON)
			{
				HAL_GPIO_WritePin(FET4_GPIO_Port, FET4_Pin, GPIO_PIN_SET);
			}
			else if (state == FET_OFF)
			{
				HAL_GPIO_WritePin(FET4_GPIO_Port, FET4_Pin, GPIO_PIN_RESET);
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
