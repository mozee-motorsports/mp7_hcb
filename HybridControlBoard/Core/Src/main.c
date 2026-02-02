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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*************************/
// CAN IDs
/*************************/

// Hybrid Control Board IDs
#define HCB_EMOTOR_TORQUE_CMD_ID  0x201
#define HCB_ICE_THROTTLE_PER_ID   0x202

// Inverter IDs (offset of 0x14E)
#define INVERTER_ID_RANGE_START 0x1EF
#define INVERTER_ID_RANGE_STOP  0x1FF

// Pedal Box Board IDs
#define PBB_THROTTLE_POSITION_ID  0x200


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
// Throttle Position Data Length
#define HCB_THROTTLE_MESSAGE_DLC  (uint8_t)2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

FDCAN_HandleTypeDef hfdcan1;

/* Definitions for blinkLED */
osThreadId_t blinkLEDHandle;
const osThreadAttr_t blinkLED_attributes = {
  .name = "blinkLED",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for highPrtyMessage */
osThreadId_t highPrtyMessageHandle;
const osThreadAttr_t highPrtyMessage_attributes = {
  .name = "highPrtyMessage",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
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
/* Definitions for mutexFdcanHandle */
osMutexId_t mutexFdcanHandleHandle;
const osMutexAttr_t mutexFdcanHandle_attributes = {
  .name = "mutexFdcanHandle"
};
/* Definitions for priorityMessageFlag */
osEventFlagsId_t priorityMessageFlagHandle;
const osEventFlagsAttr_t priorityMessageFlag_attributes = {
  .name = "priorityMessageFlag"
};
/* Definitions for throttleReceivedFlag */
osEventFlagsId_t throttleReceivedFlagHandle;
const osEventFlagsAttr_t throttleReceivedFlag_attributes = {
  .name = "throttleReceivedFlag"
};
/* USER CODE BEGIN PV */

uint8_t throttlePositionData[HCB_THROTTLE_MESSAGE_DLC];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_FDCAN1_Init(void);
void StartBlinkLED(void *argument);
void HandleHighPriorityMessage(void *argument);
void HandleLowPriorityMessage(void *argument);
void HandleThrottle(void *argument);

/* USER CODE BEGIN PFP */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);

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

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of mutexFdcanHandle */
  mutexFdcanHandleHandle = osMutexNew(&mutexFdcanHandle_attributes);

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
  /* creation of blinkLED */
  blinkLEDHandle = osThreadNew(StartBlinkLED, NULL, &blinkLED_attributes);

  /* creation of highPrtyMessage */
  highPrtyMessageHandle = osThreadNew(HandleHighPriorityMessage, NULL, &highPrtyMessage_attributes);

  /* creation of lowPrtyMessage */
  lowPrtyMessageHandle = osThreadNew(HandleLowPriorityMessage, NULL, &lowPrtyMessage_attributes);

  /* creation of handleThrottle */
  handleThrottleHandle = osThreadNew(HandleThrottle, NULL, &handleThrottle_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* creation of priorityMessageFlag */
  priorityMessageFlagHandle = osEventFlagsNew(&priorityMessageFlag_attributes);

  /* creation of throttleReceivedFlag */
  throttleReceivedFlagHandle = osEventFlagsNew(&throttleReceivedFlag_attributes);

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

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
  RCC_OscInitStruct.PLL.PLLN = 12;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 4096;
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
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
  if (MP7_FDCAN_ConfigureFilter(&hfdcan1, 0x0, 0x3FF, 0, FDCAN_RX_FIFO0) != HAL_OK) {
    Error_Handler();
  }

  // Configure Low Priority Filter on RX_FIFO1
  if (MP7_FDCAN_ConfigureFilter(&hfdcan1, 0x3FF, 0x7FF, 1, FDCAN_RX_FIFO1) != HAL_OK) {
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
  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD1_Pin */
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);

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
    osEventFlagsSet(priorityMessageFlagHandle, HCB_HIGH_PRIORITY_MESSAGE_FLAG);
  }
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
    osEventFlagsSet(priorityMessageFlagHandle, HCB_LOW_PRIORITY_MESSAGE_FLAG);
  }
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartBlinkLED */
/**
  * @brief  Function implementing the blinkLED thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartBlinkLED */
void StartBlinkLED(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    osThreadExit();
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
    uint32_t flags = osEventFlagsWait(priorityMessageFlagHandle, HCB_HIGH_PRIORITY_MESSAGE_FLAG, osFlagsWaitAny, osWaitForever);

    // Get message
    FDCAN_RxHeaderTypeDef RxHeader;
    uint8_t rxData[8];

    osStatus_t status = osMutexAcquire(mutexFdcanHandleHandle, osWaitForever);
    if (status != osOK) {
      Error_Handler();
    }

    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &RxHeader, rxData) != HAL_OK) {
      Error_Handler();
    }

    osStatus_t status = osMutexRelease(mutexFdcanHandleHandle);
    if (status != osOK) {
      Error_Handler();
    }

    // Pass off to proper function handles
    uint32_t messageID = RxHeader.Identifier;
    if (messageID < INVERTER_ID_RANGE_START) { // Error message

    } else if (messageID <= INVERTER_ID_RANGE_STOP) { // Inverter message (offset of 0x14E)

    } else { // Normal message
      if (messageID == PBB_THROTTLE_POSITION_ID) {
        // Copy throttle position data to give to the throttle thread
        memcpy(throttlePositionData, rxData, HCB_THROTTLE_MESSAGE_DLC);
        osEventFlagsSet(throttleReceivedFlagHandle, HCB_THROTTLE_RECEIVED_FLAG);
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
    uint32_t flags = osEventFlagsWait(priorityMessageFlagHandle, HCB_LOW_PRIORITY_MESSAGE_FLAG, osFlagsWaitAny, osWaitForever);

    // Get message
    FDCAN_RxHeaderTypeDef RxHeader;
    uint8_t rxData[8];

    osStatus_t acquireStatus = osMutexAcquire(mutexFdcanHandleHandle, osWaitForever);
    if (acquireStatus != osOK) {
      Error_Handler();
    }

    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO1, &RxHeader, rxData) != HAL_OK) {
      Error_Handler();
    }

    osStatus_t releaseStatus = osMutexRelease(mutexFdcanHandleHandle, osWaitForever);
    if (releaseStatus != osOK) {
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
  if (MP7_FDCAN_ConfigureTxHeader(&hfdcan1, &emotorTxHeader, HCB_EMOTOR_TORQUE_CMD_ID, 2) != HAL_OK) {
    Error_Handler();
  }

  FDCAN_TxHeaderTypeDef iceTxHeader;
  if (MP7_FDCAN_ConfigureTxHeader(&hfdcan1, &iceTxHeader, HCB_ICE_THROTTLE_PER_ID, 2) != HAL_OK) {
    Error_Handler();
  }

  /* Infinite loop */
  for(;;)
  {
    // Do not clear the flag in case there is an error
    // Timeout if 3 throttle messages have been missed
    uint32_t flag = osEventFlagsWait(throttleReceivedFlagHandle, HCB_THROTTLE_RECEIVED_FLAG, osFlagsNoClear, HCB_THROTTLE_SEND_TIME * 3);
    if (flag == osFlagsErrorTimeout) { // timeout occurred
      Error_Handler();
    }

    /********************************/
    // TODO: HYBRID CONTROL ALGORITHM
    /********************************/

    uint8_t iceThrottleData[HCB_THROTTLE_MESSAGE_DLC];
    uint8_t emotorThrottleData[2] = {0, 1}; // 0.1 N*m for now

    // TODO: Figure out how to restrict this thread to run within 50 ms
    osStatus_t acquireStatus = osMutexAcquire(mutexFdcanHandleHandle, osWaitForever);
    if (acquireStatus != osOK) {
      Error_Handler();
    }

    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &emotorTxHeader, emotorThrottleData) != HAL_OK) {
      Error_Handler();
    }

    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &iceTxHeader, iceThrottleData) != HAL_OK) {
      Error_Handler();
    }


    osStatus_t releaseStatus = osMutexRelease(mutexFdcanHandleHandle, osWaitForever);
    if (releaseStatus != osOK) {
      Error_Handler();
    }

    // Clear the flag after the messages were properly sent
    osEventFlagsClear(throttleReceivedFlagHandle, HCB_THROTTLE_RECEIVED_FLAG);

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
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
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
