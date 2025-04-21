/******************************************************************************
 * freq_sensors.c
 *
 *  Created on: Mar 30, 2025
 *      Author: nimsgernd
 *****************************************************************************/

/*=============================================================================
 |          Includes
 ============================================================================*/

#include <freq_sensors.h>

/*=============================================================================
 |          Defines
 ============================================================================*/



/*=============================================================================
 |          Function Prototypes
 ============================================================================*/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
uint16_t read_RPM();
uint32_t read_wheel_speed();

/*=============================================================================
 |          File Scope Variables
 ============================================================================*/

// RPM Variables
uint32_t IC_RPM_Val1 = 0;
uint32_t IC_RPM_Val2 = 0;
uint32_t RPM_Difference = 0;
int RPM_Is_First_Captured = 0;
float RPM_frequency = 0;

// Wheel Speed Variables
uint32_t IC_WS_Val1 = 0;
uint32_t IC_WS_Val2 = 0;
uint32_t WS_Difference = 0;
int WS_Is_First_Captured = 0;
float WS_frequency = 0;

/*=============================================================================
 |          Function Definitions
 ============================================================================*/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2)
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
		{
			if (RPM_Is_First_Captured == false) // if the first rising edge is not captured
			{
				IC_RPM_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3); // read the first value
				RPM_Is_First_Captured = true;  // set the first captured as true
			}

			else   // If the first rising edge is captured, now we will capture the second edge
			{
				IC_RPM_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);  // read second value

				if (IC_RPM_Val2 > IC_RPM_Val1)
				{
					RPM_Difference = IC_RPM_Val2-IC_RPM_Val1;
				}

				else if (IC_RPM_Val1 > IC_RPM_Val2)
				{
					RPM_Difference = (0xffffffff - IC_RPM_Val1) + IC_RPM_Val2;
				}

				float refClock = TIMCLOCK/(PRESCALAR);

				RPM_frequency = refClock/RPM_Difference;

				__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
				RPM_Is_First_Captured = false; // set it back to false
			}
		}
	}
	else if (htim->Instance == TIM15)
	{
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
				{
					if (WS_Is_First_Captured == false) // if the first rising edge is not captured
					{
						IC_WS_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
						WS_Is_First_Captured = true;  // set the first captured as true
					}

					else   // If the first rising edge is captured, now we will capture the second edge
					{
						IC_WS_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value

						if (IC_WS_Val2 > IC_WS_Val1)
						{
							WS_Difference = IC_WS_Val2-IC_WS_Val1;
						}

						else if (IC_WS_Val1 > IC_WS_Val2)
						{
							WS_Difference = (0xffffffff - IC_WS_Val1) + IC_WS_Val2;
						}

						float refClock = TIMCLOCK/(PRESCALAR);

						WS_frequency = refClock/WS_Difference;

						__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
						WS_Is_First_Captured = 0; // set it back to false
					}
				}
	}
}

uint16_t read_RPM()
{
	return (uint16_t) RPM_frequency;
}

uint32_t read_wheel_speed()
{
	return (uint32_t) WS_frequency;
}
