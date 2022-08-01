/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "gui.h"
#include "test.h" 
#include "tim.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"
/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

/* USER CODE BEGIN Private defines */
#define ADC1_CHANNEL_COUNT 1
#define ADC1_BUFFER_SIZE 399
extern uint16_t adc2_value;
extern uint16_t adc1_value[ADC1_BUFFER_SIZE*ADC1_CHANNEL_COUNT];

extern int adc2_flag;
extern int adc1_flag;
/* USER CODE END Private defines */

void MX_ADC1_Init(void);
void MX_ADC2_Init(void);

/* USER CODE BEGIN Prototypes */
void power_detect(void);
void breath_init(void);
void breath_send(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
