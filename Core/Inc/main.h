/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern int init_flag;
extern uint8_t tim3_count_seconds;
extern int wifi_connect_flag;
extern int pc_connect_flag;
extern float power[4];
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void RCCdelay_us(uint8_t udelay);//us¶¨Ê±Æ÷

float lowpass_1200hz_5jie(double y[], double x[],double xn);
float highpass_80hz_5jie(double y[], double x[],double xn);
float Notching50Hz(double y[], double x[],double xn);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CHARGE_Pin GPIO_PIN_4
#define CHARGE_GPIO_Port GPIOC
#define ECG_DRDY_Pin GPIO_PIN_7
#define ECG_DRDY_GPIO_Port GPIOE
#define ECG_CS_Pin GPIO_PIN_8
#define ECG_CS_GPIO_Port GPIOE
#define ECG_RESET_Pin GPIO_PIN_9
#define ECG_RESET_GPIO_Port GPIOE
#define ECG_PWDN_Pin GPIO_PIN_10
#define ECG_PWDN_GPIO_Port GPIOE
#define EEG_DRDY_Pin GPIO_PIN_11
#define EEG_DRDY_GPIO_Port GPIOE
#define EEG_DRDY_EXTI_IRQn EXTI15_10_IRQn
#define EEG_CS_Pin GPIO_PIN_12
#define EEG_CS_GPIO_Port GPIOE
#define ADS_START_Pin GPIO_PIN_13
#define ADS_START_GPIO_Port GPIOE
#define EEG_RESET_Pin GPIO_PIN_14
#define EEG_RESET_GPIO_Port GPIOE
#define EEG_PWDN_Pin GPIO_PIN_15
#define EEG_PWDN_GPIO_Port GPIOE
#define led0_Pin GPIO_PIN_12
#define led0_GPIO_Port GPIOB
#define LED_wifi_Pin GPIO_PIN_8
#define LED_wifi_GPIO_Port GPIOD
#define LED_power_Pin GPIO_PIN_9
#define LED_power_GPIO_Port GPIOD
#define LED_charge_Pin GPIO_PIN_10
#define LED_charge_GPIO_Port GPIOD
#define LED0_Pin GPIO_PIN_11
#define LED0_GPIO_Port GPIOD
#define TCLK_Pin GPIO_PIN_12
#define TCLK_GPIO_Port GPIOD
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOD
#define DC_RS_Pin GPIO_PIN_14
#define DC_RS_GPIO_Port GPIOD
#define RST_Pin GPIO_PIN_15
#define RST_GPIO_Port GPIOD
#define SDCD_Pin GPIO_PIN_8
#define SDCD_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_15
#define CS_GPIO_Port GPIOA
#define M8266WIFI_RESET_Pin GPIO_PIN_6
#define M8266WIFI_RESET_GPIO_Port GPIOD
#define M8266WIFI_CS_Pin GPIO_PIN_7
#define M8266WIFI_CS_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */
#define ADS_CS_Port GPIOE
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
