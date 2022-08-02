/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "InitTask.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int pc_connect_flag=0;
extern int touch_record_flag;
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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	//portENTER_CRITICAL();
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of Breath_BinarySem */
  osSemaphoreDef(Breath_BinarySem);
  Breath_BinarySemHandle = osSemaphoreCreate(osSemaphore(Breath_BinarySem), 1);
  /* definition and creation of ADS_BinarySem */
  osSemaphoreDef(ADS_BinarySem);
  ADS_BinarySemHandle = osSemaphoreCreate(osSemaphore(ADS_BinarySem), 1);
  /* definition and creation of touch_BinarySem */
  osSemaphoreDef(touch_BinarySem);
  touch_BinarySemHandle = osSemaphoreCreate(osSemaphore(touch_BinarySem), 1);
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
  /* definition and creation of defaultTask */
  ///≥ı ºªØ
  osThreadDef(defaultTask, StartDefaultTask, osPriorityAboveNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
  /* definition and creation of RealTime_Task */
  osThreadDef(RealTime_Task, StartRT_Task, osPriorityBelowNormal, 0, 256);
  RealTime_TaskHandle = osThreadCreate(osThread(RealTime_Task), NULL);
  /* definition and creation of Breath_Task */
  osThreadDef(Breath_Task, StartBreath_Task, osPriorityBelowNormal, 0, 512);
  Breath_TaskHandle = osThreadCreate(osThread(Breath_Task), NULL);
  /* definition and creation of ADS1299_Task */
  osThreadDef(ADS1299_Task, StartADS_Task, osPriorityNormal, 0, 512);
  ADS1299_TaskHandle = osThreadCreate(osThread(ADS1299_Task), (void*) ADS_BinarySemHandle);
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */


/* USER CODE BEGIN Header_StartRT_Task */
/**
* @brief Function implementing the RealTime_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRT_Task */
void StartRT_Task(void const * argument)
{
  /* USER CODE BEGIN StartRT_Task */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartRT_Task */
}

/* USER CODE BEGIN Header_StartBreath_Task */
/**
* @brief Function implementing the Breath_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBreath_Task */


/* USER CODE BEGIN Header_StartADS_Task */
/**
* @brief Function implementing the ADS1299_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartADS_Task */


/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
