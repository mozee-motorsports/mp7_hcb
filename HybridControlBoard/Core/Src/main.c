/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mp7_fdcan.h"
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*************************/
// CAN Messages
/*************************/
/*
 * CAN DLCs
 */
#define MP7_CAN_MAX_DLC             8
#define PBB_THROTTLE_POSITION_DLC   2
#define HCB_ICE_THROTTLE_PERC_DLC   2
#define HCB_EMOTOR_TORQUE_CMD_DLC   2

/*
 * CAN IDs
 */
// TODO: Move ranges to mp7_fdcan.h
// High Priority Range
#define MP7_HIGH_PRIORITY_MIN_ID  0x000
#define MP7_HIGH_PRIORITY_MAX_ID  0x3FF

// Error ID Range
#define MP7_ERROR_MIN_ID  0x000
#define MP7_ERROR_MAX_ID  0x1E5

// Normal ID Range
#define MP7_NORMAL_MIN_ID 0x1E6
#define MP7_NORMAL_MAX_ID 0x3CC

// Hybrid Control Board IDs
#define HCB_EMOTOR_TORQUE_CMD_ID  0x3FD
#define HCB_ICE_THROTTLE_PERC_ID  0x1E7

// Inverter IDs (offset of 0x3DD)
#define INVERTER_ID_MIN_ID  0x3DD
#define INVERTER_ID_MAX_ID  0x3FF

// Battery Management System (BMS) IDs
#define MP7_BMS_MIN_ID  0x3CD
#define MP7_BMS_MAX_ID  0x3DC

// Pedal Box Board IDs
#define PBB_THROTTLE_POSITION_ID  0x1E7

// Low Priority Range
#define MP7_LOW_PRIORITY_MIN_ID 0x400
#define MP7_LOW_PRIORITY_MAX_ID 0x7FF

/*************************/
// RTOS Event Flags
/*************************/
#define HCB_HIGH_PRIORITY_MESSAGE_FLAG  0x00000001
#define HCB_LOW_PRIORITY_MESSAGE_FLAG   0x00000002

#define HCB_THROTTLE_RECEIVED_FLAG      0x00000001

/*************************/
// Other
/*************************/
// Send throttle every 50 milliseconds
#define HCB_THROTTLE_SEND_TIME   (uint32_t)50

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

FDCAN_HandleTypeDef hfdcan1;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for highPrtyMessage */
osThreadId_t highPrtyMessageHandle;
const osThreadAttr_t highPrtyMessage_attributes = {
  .name = "highPrtyMessage",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for lowPrtyMessage */
osThreadId_t lowPrtyMessageHandle;
const osThreadAttr_t lowPrtyMessage_attributes = {
  .name = "lowPrtyMessage",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for handleThrottle */
osThreadId_t handleThrottleHandle;
const osThreadAttr_t handleThrottle_attributes = {
  .name = "handleThrottle",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for mutexHfdcan */
osMutexId_t mutexHfdcanHandle;
const osMutexAttr_t mutexHfdcan_attributes = {
  .name = "mutexHfdcan"
};
/* Definitions for mutexThrottleData */
osMutexId_t mutexThrottleDataHandle;
const osMutexAttr_t mutexThrottleData_attributes = {
  .name = "mutexThrottleData"
};
/* Definitions for throttleReceivedFlag */
osEventFlagsId_t throttleReceivedFlagHandle;
const osEventFlagsAttr_t throttleReceivedFlag_attributes = {
  .name = "throttleReceivedFlag"
};
/* Definitions for highPriorityEvent */
osEventFlagsId_t highPriorityEventHandle;
const osEventFlagsAttr_t highPriorityEvent_attributes = {
  .name = "highPriorityEvent"
};
/* Definitions for lowPriorityEvent */
osEventFlagsId_t lowPriorityEventHandle;
const osEventFlagsAttr_t lowPriorityEvent_attributes = {
  .name = "lowPriorityEvent"
};
/* USER CODE BEGIN PV */

typedef struct {
  uint8_t throttlePositionData[PBB_THROTTLE_POSITION_DLC];
} MP7_HCB_ThrottleDataTypeDef;

static MP7_HCB_ThrottleDataTypeDef throttleData;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_FDCAN1_Init(void);
void DefaultTask(void *argument);
void HandleHighPriorityMessage(void *argument);
void HandleLowPriorityMessage(void *argument);
void HandleThrottle(void *argument);

/* USER CODE BEGIN PFP */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FDCAN1_Init();
  /* USER CODE BEGIN 2 */

  // TODO: RTD System

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of mutexHfdcan */
  mutexHfdcanHandle = osMutexNew(&mutexHfdcan_attributes);

  /* creation of mutexThrottleData */
  mutexThrottleDataHandle = osMutexNew(&mutexThrottleData_attributes);

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
  defaultTaskHandle = osThreadNew(DefaultTask, NULL, &defaultTask_attributes);

  /* creation of highPrtyMessage */
  highPrtyMessageHandle = osThreadNew(HandleHighPriorityMessage, NULL, &highPrtyMessage_attributes);

  /* creation of lowPrtyMessage */
  lowPrtyMessageHandle = osThreadNew(HandleLowPriorityMessage, NULL, &lowPrtyMessage_attributes);

  /* creation of handleThrottle */
  handleThrottleHandle = osThreadNew(HandleThrottle, NULL, &handleThrottle_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* creation of throttleReceivedFlag */
  throttleReceivedFlagHandle = osEventFlagsNew(&throttleReceivedFlag_attributes);

  /* creation of highPriorityEvent */
  highPriorityEventHandle = osEventFlagsNew(&highPriorityEvent_attributes);

  /* creation of lowPriorityEvent */
  lowPriorityEventHandle = osEventFlagsNew(&lowPriorityEvent_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 34;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 3072;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = ENABLE;
  hfdcan1.Init.ProtocolException = ENABLE;
  hfdcan1.Init.NominalPrescaler = 5;
  hfdcan1.Init.NominalSyncJumpWidth = 1;
  hfdcan1.Init.NominalTimeSeg1 = 17;
  hfdcan1.Init.NominalTimeSeg2 = 2;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 1;
  hfdcan1.Init.DataTimeSeg2 = 1;
  hfdcan1.Init.MessageRAMOffset = 0;
  hfdcan1.Init.StdFiltersNbr = 2;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.RxFifo0ElmtsNbr = 16;
  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxFifo1ElmtsNbr = 16;
  hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxBuffersNbr = 0;
  hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.TxEventsNbr = 0;
  hfdcan1.Init.TxBuffersNbr = 0;
  hfdcan1.Init.TxFifoQueueElmtsNbr = 4;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */
  if (MP7_FDCAN_ConfigureGlobalFilter(&hfdcan1) != HAL_OK) {
    Error_Handler();
  }

  // Configure High Priority Filter on RX_FIFO0
  if (MP7_FDCAN_ConfigureFilter(&hfdcan1, MP7_HIGH_PRIORITY_MIN_ID, MP7_HIGH_PRIORITY_MAX_ID, 0, FDCAN_FILTER_TO_RXFIFO0) != HAL_OK) {
    Error_Handler();
  }

  // Configure Low Priority Filter on RX_FIFO1
  if (MP7_FDCAN_ConfigureFilter(&hfdcan1, MP7_LOW_PRIORITY_MIN_ID, MP7_LOW_PRIORITY_MAX_ID, 1, FDCAN_FILTER_TO_RXFIFO1) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0) != HAL_OK) {
    Error_Handler();
  }

  /* USER CODE END FDCAN1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LD1_Pin LD3_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
    osEventFlagsSet(highPriorityEventHandle, HCB_HIGH_PRIORITY_MESSAGE_FLAG);
  }
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs) {
  if ((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != RESET) {
    osEventFlagsSet(lowPriorityEventHandle, HCB_LOW_PRIORITY_MESSAGE_FLAG);
  }
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_DefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_DefaultTask */
void DefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);

  /* Infinite loop */
  for(;;)
  {
    osThreadYield();
  }

  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_HandleHighPriorityMessage */
/**
* @brief Function implementing the highPrtyMessage thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_HandleHighPriorityMessage */
void HandleHighPriorityMessage(void *argument)
{
  /* USER CODE BEGIN HandleHighPriorityMessage */
  /* Infinite loop */
  for(;;)
  {
    // Wait until High Priority message has been received
    if (osEventFlagsWait(highPrtyMessageHandle, HCB_HIGH_PRIORITY_MESSAGE_FLAG, osFlagsWaitAny, osWaitForever) != osOK) {
      Error_Handler();
    }

    // Get message
    FDCAN_RxHeaderTypeDef RxHeader;
    uint8_t rxData[MP7_CAN_MAX_DLC];

    // TODO: Figure out timeouts for all mutexes
    if (osMutexAcquire(mutexHfdcanHandle, osWaitForever) != osOK) {
      Error_Handler();
    }

    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &RxHeader, rxData) != HAL_OK) {
      Error_Handler();
    }

    if (osMutexRelease(mutexHfdcanHandle) != osOK) {
      Error_Handler();
    }


    // Pass off to proper function handles
    uint32_t messageID = RxHeader.Identifier;
    if (messageID <= MP7_ERROR_MAX_ID) { // Error message

    } else if (messageID <= MP7_NORMAL_MAX_ID) { // Normal message

    } else if (messageID <= MP7_BMS_MAX_ID) {

    } else if (messageID <= INVERTER_ID_MAX_ID) { // Inverter message

    } else { // Normal message
      if (messageID == PBB_THROTTLE_POSITION_ID) {
        if (osMutexAcquire(mutexThrottleDataHandle, osWaitForever) != osOK) {
          Error_Handler();
        }

        memcpy(throttleData.throttlePositionData, rxData, PBB_THROTTLE_POSITION_DLC);

        if (osMutexRelease(mutexThrottleDataHandle) != osOK) {
          Error_Handler();
        }

        if (osEventFlagsSet(throttleReceivedFlagHandle, HCB_THROTTLE_RECEIVED_FLAG) < 0) {
          Error_Handler();
        }
      }

    }
  }
  /* USER CODE END HandleHighPriorityMessage */
}

/* USER CODE BEGIN Header_HandleLowPriorityMessage */
/**
* @brief Function implementing the lowPrtyMessage thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_HandleLowPriorityMessage */
void HandleLowPriorityMessage(void *argument)
{
  /* USER CODE BEGIN HandleLowPriorityMessage */
  /* Infinite loop */
  for(;;)
  {
    // Wait until Low Priority message has been received
    if (osEventFlagsWait(lowPrtyMessageHandle, HCB_LOW_PRIORITY_MESSAGE_FLAG, osFlagsWaitAny, osWaitForever) < 0) {
      Error_Handler();
    }

    // Get message
    FDCAN_RxHeaderTypeDef RxHeader;
    uint8_t rxData[MP7_CAN_MAX_DLC];

    if (osMutexAcquire(mutexHfdcanHandle, osWaitForever) != osOK) {
      Error_Handler();
    }

    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO1, &RxHeader, rxData) != HAL_OK) {
      Error_Handler();
    }

    if (osMutexRelease(mutexHfdcanHandle) != osOK) {
      Error_Handler();
    }

    uint32_t messageID = RxHeader.Identifier;

    // TODO: Handle low priority messages

  }
  /* USER CODE END HandleLowPriorityMessage */
}

/* USER CODE BEGIN Header_HandleThrottle */
/**
* @brief Function implementing the handleThrottle thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_HandleThrottle */
void HandleThrottle(void *argument)
{
  /* USER CODE BEGIN HandleThrottle */
  FDCAN_TxHeaderTypeDef emotorTxHeader;
  if (MP7_FDCAN_ConfigureTxHeader(&emotorTxHeader, HCB_EMOTOR_TORQUE_CMD_ID, HCB_EMOTOR_TORQUE_CMD_DLC) != HAL_OK) {
    Error_Handler();
  }

  FDCAN_TxHeaderTypeDef iceTxHeader;
  if (MP7_FDCAN_ConfigureTxHeader(&iceTxHeader, HCB_ICE_THROTTLE_PERC_ID, HCB_ICE_THROTTLE_PERC_DLC) != HAL_OK) {
    Error_Handler();
  }

  /* Infinite loop */
  for(;;)
  {
    // Do not clear the flag in case there is an error
    // Timeout if 3 throttle messages have been missed
    if (osEventFlagsWait(throttleReceivedFlagHandle, HCB_THROTTLE_RECEIVED_FLAG, osFlagsNoClear, HCB_THROTTLE_SEND_TIME * 3) < 0) {
      Error_Handler();
    }

    /********************************/
    // TODO: HYBRID CONTROL ALGORITHM
    /********************************/

    uint8_t iceThrottleData[MP7_CAN_MAX_DLC] = {0};
    uint8_t emotorThrottleData[MP7_CAN_MAX_DLC] = {1, 0, 0, 0, 1, 1, 0, 0}; // 0.1 N*m for now

    // TODO: Figure out how to restrict this thread to run within 50 ms
    if (osMutexAcquire(mutexHfdcanHandle, HCB_THROTTLE_SEND_TIME) != osOK) {
      Error_Handler();
    }

    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &emotorTxHeader, emotorThrottleData) != HAL_OK) {
      Error_Handler();
    }

    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &iceTxHeader, iceThrottleData) != HAL_OK) {
      Error_Handler();
    }


    if (osMutexRelease(mutexHfdcanHandle) != osOK) {
      Error_Handler();
    }

    // Clear the flag after the messages were properly sent
    if (osEventFlagsClear(throttleReceivedFlagHandle, HCB_THROTTLE_RECEIVED_FLAG) < 0) {
      Error_Handler();
    }

  }
  /* USER CODE END HandleThrottle */
}

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
    HAL_GPIO_WritePin(GPIOE, LD3_Pin, GPIO_PIN_SET);
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
