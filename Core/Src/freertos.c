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
#include "lcd.h"
#include "gui.h"
#include "test.h"
#include "usart.h"
#include "gpio.h"

#include "sdio.h"
#include "adc.h"
#include "tim.h"
#include "spo2.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"
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
osThreadId defaultTaskHandle;
osThreadId RealTime_TaskHandle;
osThreadId Breath_TaskHandle;
osThreadId ADS1299_TaskHandle;
osSemaphoreId Breath_BinarySemHandle;
osSemaphoreId ADS_BinarySemHandle;
osSemaphoreId touch_BinarySemHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartRT_Task(void const * argument);
void StartBreath_Task(void const * argument);
void StartADS_Task(void const * argument);

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
	//portEXIT_CRITICAL();
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	//portENTER_CRITICAL();
	osSemaphoreRelease(ADS_BinarySemHandle);
	osSemaphoreRelease(Breath_BinarySemHandle);
	osSemaphoreRelease(touch_BinarySemHandle);
	
	/****************LCD_init************************/
	//LCD_Init();
	//put out all the leds
	HAL_GPIO_WritePin(LED_wifi_GPIO_Port,LED_wifi_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_charge_GPIO_Port,LED_charge_Pin,GPIO_PIN_SET);
	//HAL_GPIO_WritePin(LED_power_GPIO_Port,LED_wifi_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_SET);
	/****************wifi_init************************/
	M8266WIFI_Module_Init_Via_SPI();
	M8266WIFI_Test();
	//HAL_GPIO_WritePin(LED_wifi_GPIO_Port,LED_wifi_Pin,GPIO_PIN_RESET);
	/****************ADS1299_init************************/

	ADS_Init();
    /****************breath_init************************/

	breath_init();
	/****************spo2_init************************/

	spo2_init();
	/****************adc2_task_1s_init************************/
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&adc2_value,1);	
	
	osThreadTerminate(defaultTaskHandle);//delete Default Task
	//portEXIT_CRITICAL();
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

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
////	tp_dev.sta=0;//消除触发信号 
////	tp_dev.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误
////	init_flag=1;//是否建立新的音频文件
////	touch_init();
////	int reset_flag=0;
////	screen_start_count_flag=1;
////	//osThreadTerminate(RealTime_TaskHandle);
////  /* Infinite loop */
  for(;;)
  {
//		osSemaphoreWait(touch_BinarySemHandle, osWaitForever);
//		
//		touch_lead_detect_show();
//		power_detect();
//    if(change_flag)
//		refresh_lcd();
		
//		receive_spo2();
//		if(adc2_flag)
//		{
//			adc2_flag=0;
////			send_spo2();
//			power_detect();
//		}

//		if(!wifi_connect_flag)
//		{
//			 query_M8266_state();
//			osDelay(10);
//		}
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
void StartBreath_Task(void const * argument)
{
  /* USER CODE BEGIN StartBreath_Task */
	uint8_t adc1_count=0;
	//uint16_t breath_send_count=0;	//发送呼吸音的次数
  /* Infinite loop */
  for(;;)
  {
		osSemaphoreWait(Breath_BinarySemHandle, osWaitForever);

		//if(touch_record_flag)
        breath_send();
		
		
//		if(lcd_wakeup_flag==0){
//		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);//熄灭背光	
//		screen_start_count_flag=0;
//		}
		receive_spo2();
  }
  /* USER CODE END StartBreath_Task */
}

/* USER CODE BEGIN Header_StartADS_Task */
/**
* @brief Function implementing the ADS1299_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartADS_Task */
void StartADS_Task(void const * argument)
{
  /* USER CODE BEGIN StartADS_Task */
	uint16_t status=0;
	uint32_t ads_save_count=0;
	uint8_t ads_save_buff[800];
  /* Infinite loop */
  for(;;)
  {
		osSemaphoreWait(ADS_BinarySemHandle, osWaitForever);
		while((GPIOE->IDR & GPIO_PIN_7) != (uint32_t)GPIO_PIN_RESET)
		{
		
		}
		ADS_RDATA();				//READ DATA
		ADS_SendDataBuff();     	//COPY DATA TO BUFF
		if(!pc_connect_flag) ///如果此时是断线
		{
			query_M8266_state();
			osDelay(10);
		}	
		for(int i=0;i<2;i++)
		for(int j=0;j<8;j++)
		{
            ///ads存回来的数据
			ads_save_buff[ads_save_count]=ecg_eeg_sendbuff[i][j];
			ads_save_count+=1;
		}

        ///如果不足以800一块就继续收集
		if(ads_save_count==800)
		{
			ads_save_count=0;
			vTaskSuspendAll();
			M8266WIFI_SPI_Send_BlockData((uint8_t*)ads_save_buff,800,2000,0,NULL,0,&status);
			xTaskResumeAll();
			//test500hz_count++;
		}

		if(adc2_flag)
		{
			adc2_flag=0;
			send_spo2();	
			power_detect();
		}
//		if(test500hz_count==1000)
//		{

//			osDelay(1);
//		}
		//touch_scan();
		ADS_START(EEG_CS_Pin);				//START NEXT EEG CONVERT
		ADS_START(ECG_CS_Pin);				//START NEXT ECG CONVERT
		osDelay(1);
  }
  /* USER CODE END StartADS_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
