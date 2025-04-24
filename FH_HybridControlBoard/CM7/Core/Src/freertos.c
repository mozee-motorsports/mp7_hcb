/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "driver_io.h"
#include "FET_control.h"
#include "freq_sensors.h"
#include "fan_control.h"
#include "temperature_sensors.h"
#include "can_bus.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint16_t RPM = 0;
uint32_t wheelSpeed = 0;
float coreTemp = 0.0;
float ambientTemp = 0.0;
float ICTemp = 0.0;
float TSTemp = 0.0;
uint16_t accelerationPedalPos = 0;

volatile bool can_init_done = false;

// FDCAN1 Variables
volatile FDCAN_TxHeaderTypeDef tx_header1;
volatile uint8_t tx_data1[8];

// FDCAN2 Variables
volatile FDCAN_TxHeaderTypeDef tx_header2;
volatile uint8_t tx_data2[8];

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for CAN1Read */
osThreadId_t CAN1ReadHandle;
const osThreadAttr_t CAN1Read_attributes = {
  .name = "CAN1Read",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for VCOM */
osThreadId_t VCOMHandle;
const osThreadAttr_t VCOM_attributes = {
  .name = "VCOM",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DashButtons */
osThreadId_t DashButtonsHandle;
const osThreadAttr_t DashButtons_attributes = {
  .name = "DashButtons",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for FanControl */
osThreadId_t FanControlHandle;
const osThreadAttr_t FanControl_attributes = {
  .name = "FanControl",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ReadSensors */
osThreadId_t ReadSensorsHandle;
const osThreadAttr_t ReadSensors_attributes = {
  .name = "ReadSensors",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for CAN2Read */
osThreadId_t CAN2ReadHandle;
const osThreadAttr_t CAN2Read_attributes = {
  .name = "CAN2Read",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for PollPeddalBox */
osThreadId_t PollPeddalBoxHandle;
const osThreadAttr_t PollPeddalBox_attributes = {
  .name = "PollPeddalBox",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for PollThrottleBox */
osThreadId_t PollThrottleBoxHandle;
const osThreadAttr_t PollThrottleBox_attributes = {
  .name = "PollThrottleBox",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for PollShutdownBox */
osThreadId_t PollShutdownBoxHandle;
const osThreadAttr_t PollShutdownBox_attributes = {
  .name = "PollShutdownBox",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for PollWheel */
osThreadId_t PollWheelHandle;
const osThreadAttr_t PollWheel_attributes = {
  .name = "PollWheel",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for KeyStats */
osMessageQueueId_t KeyStatsHandle;
const osMessageQueueAttr_t KeyStats_attributes = {
  .name = "KeyStats"
};
/* Definitions for UartBuf */
osMessageQueueId_t UartBufHandle;
const osMessageQueueAttr_t UartBuf_attributes = {
  .name = "UartBuf"
};
/* Definitions for SemaphoreRx */
osMessageQueueId_t SemaphoreRxHandle;
const osMessageQueueAttr_t SemaphoreRx_attributes = {
  .name = "SemaphoreRx"
};
/* Definitions for fdcan1_queue */
osMessageQueueId_t fdcan1_queueHandle;
const osMessageQueueAttr_t fdcan1_queue_attributes = {
  .name = "fdcan1_queue"
};
/* Definitions for fdcan2_queue */
osMessageQueueId_t fdcan2_queueHandle;
const osMessageQueueAttr_t fdcan2_queue_attributes = {
  .name = "fdcan2_queue"
};
/* Definitions for CAN1Write */
osMutexId_t CAN1WriteHandle;
const osMutexAttr_t CAN1Write_attributes = {
  .name = "CAN1Write"
};
/* Definitions for CAN2Write */
osMutexId_t CAN2WriteHandle;
const osMutexAttr_t CAN2Write_attributes = {
  .name = "CAN2Write"
};
/* Definitions for CANRead */
osMutexId_t CANReadHandle;
const osMutexAttr_t CANRead_attributes = {
  .name = "CANRead"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void processCANMessage(FDCAN_HandleTypeDef *hfdcan, volatile FDCAN_TxHeaderTypeDef *tx_header, CANMessage *msg, MODULE module, COMMAND command);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartCAN1Read(void *argument);
void StartVCOM(void *argument);
void StartDashButtons(void *argument);
void StartFanControl(void *argument);
void StartReadSensors(void *argument);
void StartCAN2Read(void *argument);
void StartPollPeddalBox(void *argument);
void StartPollThrottleControl(void *argument);
void StartPollShutdownCircuit(void *argument);
void StartPollSteeringWheel(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{
	HAL_TIM_Base_Start_IT(&htim12);
}

extern volatile unsigned long ulHighFrequencyTimerTicks;
__weak unsigned long getRunTimeCounterValue(void)
{
	return ulHighFrequencyTimerTicks;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of CAN1Write */
  CAN1WriteHandle = osMutexNew(&CAN1Write_attributes);

  /* creation of CAN2Write */
  CAN2WriteHandle = osMutexNew(&CAN2Write_attributes);

  /* creation of CANRead */
  CANReadHandle = osMutexNew(&CANRead_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of KeyStats */
  KeyStatsHandle = osMessageQueueNew (16, sizeof(uint16_t), &KeyStats_attributes);

  /* creation of UartBuf */
  UartBufHandle = osMessageQueueNew (16, sizeof(uint16_t), &UartBuf_attributes);

  /* creation of SemaphoreRx */
  SemaphoreRxHandle = osMessageQueueNew (16, sizeof(uint16_t), &SemaphoreRx_attributes);

  /* creation of fdcan1_queue */
  fdcan1_queueHandle = osMessageQueueNew (50, sizeof(CANMessage), &fdcan1_queue_attributes);

  /* creation of fdcan2_queue */
  fdcan2_queueHandle = osMessageQueueNew (50, sizeof(CANMessage), &fdcan2_queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  vQueueAddToRegistry(KeyStatsHandle, "KeyStats");
  vQueueAddToRegistry(UartBufHandle, "UartBuf");
  vQueueAddToRegistry(SemaphoreRxHandle, "SemaphoreRx");
  vQueueAddToRegistry(fdcan1_queue_attributes, "fdcan1_queue_attributes");
  vQueueAddToRegistry(fdcan2_queue_attributes, "fdcan2_queue_attributes");
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of CAN1Read */
  CAN1ReadHandle = osThreadNew(StartCAN1Read, NULL, &CAN1Read_attributes);

  /* creation of VCOM */
  VCOMHandle = osThreadNew(StartVCOM, NULL, &VCOM_attributes);

  /* creation of DashButtons */
  DashButtonsHandle = osThreadNew(StartDashButtons, NULL, &DashButtons_attributes);

  /* creation of FanControl */
  FanControlHandle = osThreadNew(StartFanControl, NULL, &FanControl_attributes);

  /* creation of ReadSensors */
  ReadSensorsHandle = osThreadNew(StartReadSensors, NULL, &ReadSensors_attributes);

  /* creation of CAN2Read */
  CAN2ReadHandle = osThreadNew(StartCAN2Read, NULL, &CAN2Read_attributes);

  /* creation of PollPeddalBox */
  PollPeddalBoxHandle = osThreadNew(StartPollPeddalBox, NULL, &PollPeddalBox_attributes);

  /* creation of PollThrottleBox */
  PollThrottleBoxHandle = osThreadNew(StartPollThrottleControl, NULL, &PollThrottleBox_attributes);

  /* creation of PollShutdownBox */
  PollShutdownBoxHandle = osThreadNew(StartPollShutdownCircuit, NULL, &PollShutdownBox_attributes);

  /* creation of PollWheel */
  PollWheelHandle = osThreadNew(StartPollSteeringWheel, NULL, &PollWheel_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	/* Infinite loop */
	for(;;)
	{
		HAL_GPIO_TogglePin(User_LED2_GPIO_Port, User_LED2_Pin);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartCAN1Read */
/**
* @brief Function implementing the CAN1Read thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCAN1Read */
void StartCAN1Read(void *argument)
{
  /* USER CODE BEGIN StartCAN1Read */
	while(!can_init_done) vTaskDelay(pdMS_TO_TICKS(5));

	CANMessage msg;
	/* Infinite loop */
	for(;;)
	{
		// If the queue is not empty
		if (osMessageQueueGetCount(fdcan1_queueHandle) > 0)
		{
			osStatus_t status = osMessageQueueGet(fdcan1_queueHandle, &msg, NULL, 0U);
			// Get current message in queue and remove it. Wait up to 100MS in ticks
			if (status == osOK)
			{
				MODULE module = (MODULE)((msg.rx_header.Identifier & 0b00011100000) >> 5);
			    COMMAND command = (COMMAND)(msg.rx_header.Identifier & 0b00000001111);
			    osMutexAcquire(CANReadHandle, osWaitForever); // Ensure thread safety when accessing the file scope variables
			    processCANMessage(&hfdcan1, &tx_header1, &msg, module, command);
			    osMutexRelease(CANReadHandle); // Release mutex after accessing the file scope variables
			}
		}
		vTaskDelay(pdMS_TO_TICKS(5));
	}
  /* USER CODE END StartCAN1Read */
}

/* USER CODE BEGIN Header_StartVCOM */
/**
* @brief Function implementing the VCOM thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartVCOM */
void StartVCOM(void *argument)
{
  /* USER CODE BEGIN StartVCOM */
	/* Infinite loop */
	for(;;)
	{
		vTaskDelay(pdMS_TO_TICKS(100));
	}
  /* USER CODE END StartVCOM */
}

/* USER CODE BEGIN Header_StartDashButtons */
/**
* @brief Function implementing the DashButtons thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDashButtons */
void StartDashButtons(void *argument)
{
  /* USER CODE BEGIN StartDashButtons */
	/* Infinite loop */
	bool fuelPumpLastState = false;
	bool fuelPumpState = false;
	bool ignitionLastState = false;
	bool ignitionState = false;
	bool readyToDriveLastState = false;
	bool readyToDriveState = false;
	for(;;)
	{
		// Momentary Starter (On when button pressed)
		if (read_button(StarterButton))
		{
			set_fet(StarterFET, FET_ON);
		}
		else
		{
			set_fet(StarterFET, FET_ON);
		}

		// Toggle Fuel Pump (Toggles on button press)
		if (read_button(FuelPumpButton))
		{
			if (!fuelPumpLastState)
			{
				if (!fuelPumpState)
				{
					set_fet(FuelPumpFET, FET_ON);
					fuelPumpState = true;
				}
				else
				{
					set_fet(FuelPumpFET, FET_OFF);
					fuelPumpState = false;
				}
			}
			fuelPumpLastState = true;
		}
		else
		{
			fuelPumpLastState = false;
		}

		// Toggle Ignition (Toggles on button press)
		if (read_button(IgnitionButton))
		{
			if (!ignitionLastState)
			{
				if (!ignitionState)
				{
					set_fet(IgnitionFET, FET_ON);
					ignitionState = true;
				}
				else
				{
					set_fet(IgnitionFET, FET_OFF);
					ignitionState = false;
				}
			}
			ignitionLastState = true;
		}
		else
		{
			ignitionLastState = false;
		}

		// Toggle Ready To Drive (Toggles on button press)
		if (read_button(ReadyToDriveButton))
		{
			if (!readyToDriveLastState)
			{
				if(!readyToDriveState)
				{
					HAL_GPIO_WritePin(R2D_LED_GPIO_Port, R2D_LED_Pin, GPIO_PIN_SET);
					readyToDriveState = true;
				}
				else
				{
					HAL_GPIO_WritePin(R2D_LED_GPIO_Port, R2D_LED_Pin, GPIO_PIN_RESET);
					readyToDriveState = false;
				}
			}
			readyToDriveLastState = true;
		}
		else
		{
			readyToDriveLastState = false;
		}

		vTaskDelay(pdMS_TO_TICKS(100));
	}
  /* USER CODE END StartDashButtons */
}

/* USER CODE BEGIN Header_StartFanControl */
/**
* @brief Function implementing the FanControl thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartFanControl */
void StartFanControl(void *argument)
{
  /* USER CODE BEGIN StartFanControl */
	/* Infinite loop */
	for(;;)
	{
		// Fan 1 set based on IC temp
		if (ICTemp < 70)
		{
			fan_output(FAN1, 0);
		}
		else if (ICTemp < 80)
		{
			fan_output(FAN1, 50);
		}
		else if (ICTemp < 90)
		{
			fan_output(FAN1, 75);
		}
		else
		{
			fan_output(FAN1, 100);
		}

		// Fan 2 set based on TS temp
		if (TSTemp < 70)
		{
			fan_output(FAN2, 0);
		}
		else if (TSTemp < 80)
		{
			fan_output(FAN2, 50);
		}
		else if (TSTemp < 90)
		{
			fan_output(FAN2, 75);
		}
		else
		{
			fan_output(FAN2, 100);
		}

		vTaskDelay(pdMS_TO_TICKS(500));
	}
  /* USER CODE END StartFanControl */
}

/* USER CODE BEGIN Header_StartReadSensors */
/**
* @brief Function implementing the ReadSensors thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartReadSensors */
void StartReadSensors(void *argument)
{
  /* USER CODE BEGIN StartReadSensors */
	/* Infinite loop */
	for(;;)
	{
		RPM = read_RPM();
		wheelSpeed = read_wheel_speed();
		coreTemp = read_temp(CORE);
		ambientTemp = read_temp(AMBIENT);
		ICTemp = read_temp(IC_LOOP);
		TSTemp = read_temp(TS_LOOP);
		vTaskDelay(pdMS_TO_TICKS(125));
	}
  /* USER CODE END StartReadSensors */
}

/* USER CODE BEGIN Header_StartCAN2Read */
/**
* @brief Function implementing the CAN2Read thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCAN2Read */
void StartCAN2Read(void *argument)
{
  /* USER CODE BEGIN StartCAN2Read */
	while(!can_init_done) vTaskDelay(pdMS_TO_TICKS(5));

	CANMessage msg;
	/* Infinite loop */
	for(;;)
	{
		// If the queue is not empty
		if (osMessageQueueGetCount(fdcan2_queueHandle) > 0)
		{
			osStatus_t status = osMessageQueueGet(fdcan2_queueHandle, &msg, NULL, 0U);
			// Get current message in queue and remove it. Wait up to 100MS in ticks
			if (status == osOK)
			{
				MODULE module = (MODULE)((msg.rx_header.Identifier & 0b00011100000) >> 5);
				COMMAND command = (COMMAND)(msg.rx_header.Identifier & 0b00000001111);
				osMutexAcquire(CANReadHandle, osWaitForever); // Ensure thread safety when accessing the file scope variables
				processCANMessage(&hfdcan2, &tx_header2, &msg, module, command);
				osMutexRelease(CANReadHandle); // Release mutex after accessing the file scope variables
			}
		}
		vTaskDelay(pdMS_TO_TICKS(5));
	}
  /* USER CODE END StartCAN2Read */
}

/* USER CODE BEGIN Header_StartPollPeddalBox */
/**
* @brief Function implementing the PollPeddalBox thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPollPeddalBox */
void StartPollPeddalBox(void *argument)
{
  /* USER CODE BEGIN StartPollPeddalBox */
	while(!can_init_done) vTaskDelay(pdMS_TO_TICKS(5));
	/* Infinite loop */
	for(;;)
	{
		// Poll Status
//		osMutexAcquire(CAN1WriteHandle, osWaitForever); // Ensure thread safety when accessing the CAN peripherals
//		if (fdcanWrite(&hfdcan1, &tx_header1, &tx_data1, status_report_poll, peddal_box, to, normal, peddal_box_status_report) != HAL_OK)
//		{
//			osMutexRelease(CAN1WriteHandle); // Release mutex after accessing the CAN peripherals
//			Error_Handler();
//		}
//		osMutexRelease(CAN1WriteHandle); // Release mutex after accessing the CAN peripherals
//
//		vTaskDelay(pdMS_TO_TICKS(5));

		vTaskDelay(pdMS_TO_TICKS(5));
	}
  /* USER CODE END StartPollPeddalBox */
}

/* USER CODE BEGIN Header_StartPollThrottleControl */
/**
* @brief Function implementing the PollThrottleBox thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPollThrottleControl */
void StartPollThrottleControl(void *argument)
{
  /* USER CODE BEGIN StartPollThrottleControl */
  /* Infinite loop */
  for(;;)
  {
	  // Poll Status
//	  osMutexAcquire(CAN1WriteHandle, osWaitForever); // Ensure thread safety when accessing the CAN peripherals
//	  if (fdcanWrite(&hfdcan1, &tx_header1, &tx_data1, status_report_poll, ic_throttle_control, to, normal, ic_throttle_control_status_report) != HAL_OK)
//	  {
//		  osMutexRelease(CAN1WriteHandle); // Release mutex after accessing the CAN peripherals
//		  Error_Handler();
//	  }
//	  osMutexRelease(CAN1WriteHandle); // Release mutex after accessing the CAN peripherals
//
//	  vTaskDelay(pdMS_TO_TICKS(5));
//
//	  // Send Throttle Value
//	  osMutexAcquire(CAN1WriteHandle, osWaitForever); // Ensure thread safety when accessing the CAN peripherals
//	  memcpy(tx_data1, &accelerationPedalPos, throttle_percentage);
//	  if (fdcanWrite(&hfdcan1, &tx_header1, &tx_data1, throttle_percentage, ic_throttle_control, to, normal, ic_throttle_control_throttle_percent) != HAL_OK)
//	  {
//		  osMutexRelease(CAN1WriteHandle); // Release mutex after accessing the CAN peripherals
//	  	  Error_Handler();
//	  }
//	  osMutexRelease(CAN1WriteHandle); // Release mutex after accessing the CAN peripherals
//
//	  vTaskDelay(pdMS_TO_TICKS(5));

	  vTaskDelay(pdMS_TO_TICKS(5));
  }
  /* USER CODE END StartPollThrottleControl */
}

/* USER CODE BEGIN Header_StartPollShutdownCircuit */
/**
* @brief Function implementing the PollShutdownBox thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPollShutdownCircuit */
void StartPollShutdownCircuit(void *argument)
{
  /* USER CODE BEGIN StartPollShutdownCircuit */
	/* Infinite loop */
	for(;;)
	{
		// Poll Status
//		osMutexAcquire(CAN1WriteHandle, osWaitForever); // Ensure thread safety when accessing the CAN peripherals
//		if (fdcanWrite(&hfdcan1, &tx_header1, &tx_data1, status_report_poll, safety_system, to, normal, safety_system_status_report) != HAL_OK)
//		{
//			osMutexRelease(CAN1WriteHandle); // Release mutex after accessing the CAN peripherals
//			Error_Handler();
//		}
//		osMutexRelease(CAN1WriteHandle); // Release mutex after accessing the CAN peripherals
//
//		vTaskDelay(pdMS_TO_TICKS(5));

		vTaskDelay(pdMS_TO_TICKS(5));
	}
  /* USER CODE END StartPollShutdownCircuit */
}

/* USER CODE BEGIN Header_StartPollSteeringWheel */
/**
* @brief Function implementing the PollWheel thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPollSteeringWheel */
void StartPollSteeringWheel(void *argument)
{
  /* USER CODE BEGIN StartPollSteeringWheel */
	/* Infinite loop */
	for(;;)
	{
		// Poll Status
//		osMutexAcquire(CAN1WriteHandle, osWaitForever); // Ensure thread safety when accessing the CAN peripherals
//		if (fdcanWrite(&hfdcan1, &tx_header1, &tx_data1, status_report_poll, safety_system, to, normal, safety_system_status_report) != HAL_OK)
//		{
//			osMutexRelease(CAN1WriteHandle); // Release mutex after accessing the CAN peripherals
//			Error_Handler();
//		}
//		osMutexRelease(CAN1WriteHandle); // Release mutex after accessing the CAN peripherals
//
//		vTaskDelay(pdMS_TO_TICKS(5));

		vTaskDelay(pdMS_TO_TICKS(5));
	}
  /* USER CODE END StartPollSteeringWheel */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
static void processCANMessage(FDCAN_HandleTypeDef *hfdcan, volatile FDCAN_TxHeaderTypeDef *tx_header, CANMessage *msg, MODULE module, COMMAND command)
{
	switch (module) {
		case safety_system:
			switch (command) {
				case safety_system_status_report:

					break;
				case safety_system_error_report:

					break;
				case safety_system_accelerometer_data:

					break;
				case safety_system_voltage_stage_report:

					break;
				default:
					break;
			}
			break;
		case broadcast:

			break;
		case ic_throttle_control:
			switch (command) {
				case ic_throttle_control_status_report:

					break;
				case ic_throttle_control_error_report:

					break;
				default:
					break;
			}
			break;
		case peddal_box:
			switch (command) {
				case peddal_box_status_report:

					break;
				case peddal_box_error_report:

					break;
				case peddal_box_accelerometer_data:

					break;
				case peddal_box_accelerator_pedal_percent:
					if(msg->rx_header.DataLength == throttle_percentage)
					{
						memcpy(&accelerationPedalPos, msg->data, throttle_percentage);
					}
					break;
				default:
					break;
			}
			break;
		case steering_wheel:
			switch (command) {
				case steering_wheel_status_report:

					break;
				case steering_wheel_error_report:

					break;
				case steering_wheel_reverse_lockout:

					break;
				case steering_wheel_gear_shift:

					break;
				case steering_wheel_hybrid_power_ratio:

					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}
/* USER CODE END Application */

