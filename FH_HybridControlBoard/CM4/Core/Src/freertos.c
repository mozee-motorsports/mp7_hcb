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

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UpdateDash */
osThreadId_t UpdateDashHandle;
const osThreadAttr_t UpdateDash_attributes = {
  .name = "UpdateDash",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DriverInput */
osThreadId_t DriverInputHandle;
const osThreadAttr_t DriverInput_attributes = {
  .name = "DriverInput",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for SevenSegUpdate */
osThreadId_t SevenSegUpdateHandle;
const osThreadAttr_t SevenSegUpdate_attributes = {
  .name = "SevenSegUpdate",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DataLogger */
osThreadId_t DataLoggerHandle;
const osThreadAttr_t DataLogger_attributes = {
  .name = "DataLogger",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UpdateExtICs */
osThreadId_t UpdateExtICsHandle;
const osThreadAttr_t UpdateExtICs_attributes = {
  .name = "UpdateExtICs",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTaskUD(void *argument);
void StartTaskDI(void *argument);
void StartTaskSSU(void *argument);
void StartTaskDL(void *argument);
void StartTaskUEIC(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
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

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of UpdateDash */
  UpdateDashHandle = osThreadNew(StartTaskUD, NULL, &UpdateDash_attributes);

  /* creation of DriverInput */
  DriverInputHandle = osThreadNew(StartTaskDI, NULL, &DriverInput_attributes);

  /* creation of SevenSegUpdate */
  SevenSegUpdateHandle = osThreadNew(StartTaskSSU, NULL, &SevenSegUpdate_attributes);

  /* creation of DataLogger */
  DataLoggerHandle = osThreadNew(StartTaskDL, NULL, &DataLogger_attributes);

  /* creation of UpdateExtICs */
  UpdateExtICsHandle = osThreadNew(StartTaskUEIC, NULL, &UpdateExtICs_attributes);

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
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTaskUD */
/**
* @brief Function implementing the UpdateDash thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUD */
void StartTaskUD(void *argument)
{
  /* USER CODE BEGIN StartTaskUD */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskUD */
}

/* USER CODE BEGIN Header_StartTaskDI */
/**
* @brief Function implementing the DriverInput thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskDI */
void StartTaskDI(void *argument)
{
  /* USER CODE BEGIN StartTaskDI */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskDI */
}

/* USER CODE BEGIN Header_StartTaskSSU */
/**
* @brief Function implementing the SevenSegUpdate thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskSSU */
void StartTaskSSU(void *argument)
{
  /* USER CODE BEGIN StartTaskSSU */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskSSU */
}

/* USER CODE BEGIN Header_StartTaskDL */
/**
* @brief Function implementing the DataLogger thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskDL */
void StartTaskDL(void *argument)
{
  /* USER CODE BEGIN StartTaskDL */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskDL */
}

/* USER CODE BEGIN Header_StartTaskUEIC */
/**
* @brief Function implementing the UpdateExtICs thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUEIC */
void StartTaskUEIC(void *argument)
{
  /* USER CODE BEGIN StartTaskUEIC */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskUEIC */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

