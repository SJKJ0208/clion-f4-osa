/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "fatfs.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"
#include "ADS1298.h"
#include "spo2.h"
#include "lcd.h"
#include "gui.h"
#include "test.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t tim3_count_seconds=0;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
int init_flag;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void RCCdelay_us(uint8_t udelay)
{
  __IO uint32_t Delay = udelay * 168 / 8;//(SystemCoreClock / 8U / 1000000U)
    //��stm32f1xx_hal_rcc.c -- static void RCC_Delay(uint32_t mdelay)
  do
  {
    __NOP();
  }
  while (Delay --);	
}
const double NUM2[6] = {
     0.9265782515687,   -4.632891257843,    9.265782515687,   -9.265782515687,
      4.632891257843,  -0.9265782515687
};

const double DEN2[6] = {
                   1,   -4.847508003664,      9.4015942664,   -9.119196657394,
      4.423657866459,  -0.8585472562801
};

float highpass_80hz_5jie(double y[], double x[],double xn)
{
	float yn;
  x[0]=x[1];
  x[1]=x[2];
  x[2]=x[3];
  x[3]=x[4];
  x[4]=x[5];
  x[5]=xn;
  y[0]=y[1];
  y[1]=y[2];
  y[2]=y[3];
  y[3]=y[4];
  y[4]=y[5];
  yn=NUM2[0]*x[5]+NUM2[1]*x[4]+NUM2[2]*x[3] + NUM2[3]*x[2]+NUM2[4]*x[2]+NUM2[5]*x[0]-DEN2[1]*y[4]-DEN2[2]*y[3]-DEN2[3]*y[2]-DEN2[4]*y[1]-DEN2[5]*y[0];
  y[5]=yn;
	return yn;
	
}

const double NUM_1200hzlp5jie[6] = {
    0.006933196130143,  0.03466598065071,  0.06933196130143,  0.06933196130143,
    0.03466598065071, 0.006933196130143
};

const double DEN_1200hzlp5jie[6] = {
     1,   -1.975901616441,       2.013473026,   -1.102617977778,
     0.3276183340002, -0.04070948961667
};

float lowpass_1200hz_5jie(double y[], double x[],double xn)
{
	float yn;
  x[0]=x[1];
  x[1]=x[2];
  x[2]=x[3];
  x[3]=x[4];
  x[4]=x[5];
  x[5]=xn;
  y[0]=y[1];
  y[1]=y[2];
  y[2]=y[3];
  y[3]=y[4];
  y[4]=y[5];
  yn=NUM_1200hzlp5jie[0]*x[5]+NUM_1200hzlp5jie[1]*x[4]+NUM_1200hzlp5jie[2]*x[3] + NUM_1200hzlp5jie[3]*x[2]+NUM_1200hzlp5jie[4]*x[2]+NUM_1200hzlp5jie[5]*x[0]
			-DEN_1200hzlp5jie[1]*y[4]-DEN_1200hzlp5jie[2]*y[3]-DEN_1200hzlp5jie[3]*y[2]-DEN_1200hzlp5jie[4]*y[1]-DEN_1200hzlp5jie[5]*y[0];
  y[5]=yn;
	return yn;
}

const double NUM[3] = {//Q=1
      0.9950290778951,   -1.988523893073,   0.9950290778951
};
const double DEN[3] = {
            1,   -1.988523893073,   0.9900581557902
};
float Notching50Hz(double y[], double x[],double xn){
  float yn;
	x[0]=x[1];
  x[1]=x[2];
  x[2]=xn;
  y[0]=y[1];
	y[1]=y[2];
  yn=(float)(NUM[0]*x[2]+NUM[1]*x[1]+NUM[2]*x[0]-DEN[1]*y[1]-DEN[2]*y[0]);
	y[2]=yn;
	return yn;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART3_UART_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_SPI3_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
//		/****************LCD_init************************/
//	LCD_Init();
//	main_test();
	/****************wifi_init************************/
//	M8266WIFI_Module_Init_Via_SPI();
//	M8266WIFI_Test();
//	/****************ADS1299_init************************/
////	ADS_Init();
//	/****************spo2_init************************/
//	spo2_init();
//	/****************adc2_task_1s************************/
//	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
//	HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&adc2_value,1);
//	/****************breath_init************************/
//	breath_init();
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
extern osSemaphoreId ADS_BinarySemHandle;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
	{
	if(GPIO_Pin == EEG_DRDY_Pin)
		{
			//HAL_GPIO_TogglePin(led0_GPIO_Port,led0_Pin);
		//INIT_DRDY = true;		
		//osSemaphoreRelease(ADS_BinarySemHandle);
		BaseType_t  xHigherPriorityTaskWoken;
		xSemaphoreGiveFromISR(ADS_BinarySemHandle,&xHigherPriorityTaskWoken);
//		ADS_updateChannelData();
//		ADS_SendDataBuff();//���Ʋ�����+
	  
	}			
//	if(GPIO_Pin == ECG_DRDY_Pin)
//		{
//			//HAL_GPIO_TogglePin(led0_GPIO_Port,led0_Pin);
//		//INIT_DRDY = true;		
//		//osSemaphoreRelease(ADS_BinarySemHandle);
//		BaseType_t  xHigherPriorityTaskWoken;
//		xSemaphoreGiveFromISR(ADS_BinarySemHandle,&xHigherPriorityTaskWoken);
////		ADS_updateChannelData();
////		ADS_SendDataBuff();//���Ʋ�����+
//	  
//	}	
}
/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
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
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/