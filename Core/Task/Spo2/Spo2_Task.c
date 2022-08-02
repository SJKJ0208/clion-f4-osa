//
// Created by jjj on 2022/8/2.
//
#include "InitTask.h"
#include "adc.h"
#include "spo2.h"

void StartBreath_Task(void const * argument)
{
    /* USER CODE BEGIN StartBreath_Task */
    uint8_t adc1_count=0;
    //uint16_t breath_send_count=0;	//发送呼吸音的次数
    /* Infinite loop */
    for(;;)
    {
        osSemaphoreWait(Breath_BinarySemHandle, osWaitForever);
        breath_send();
        receive_spo2();
    }
    /* USER CODE END StartBreath_Task */
}