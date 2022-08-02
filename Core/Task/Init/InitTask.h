//
// Created by jjj on 2022/8/2.
//

#ifndef OSAHS_PROJECT_INITTASK_H
#define OSAHS_PROJECT_INITTASK_H
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

void StartDefaultTask(void const * argument);
void StartADS_Task(void const * argument);
extern void StartRT_Task(void const * argument);
extern void StartBreath_Task(void const * argument);

extern osSemaphoreId Breath_BinarySemHandle;
extern osSemaphoreId ADS_BinarySemHandle;
extern osSemaphoreId touch_BinarySemHandle;

extern osThreadId defaultTaskHandle;
extern osThreadId RealTime_TaskHandle;
extern osThreadId Breath_TaskHandle;
extern osThreadId ADS1299_TaskHandle;

extern int wifi_connect_flag;

#endif //OSAHS_PROJECT_INITTASK_H
