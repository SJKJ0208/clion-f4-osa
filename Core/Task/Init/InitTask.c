//
// Created by jjj on 2022/8/2.
//

#include "InitTask.h"
#include "adc.h"
#include "tim.h"
#include "spo2.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"


osSemaphoreId Breath_BinarySemHandle;
osSemaphoreId ADS_BinarySemHandle;
osSemaphoreId touch_BinarySemHandle;

osThreadId defaultTaskHandle;

osThreadId RealTime_TaskHandle;
osThreadId Breath_TaskHandle;
osThreadId ADS1299_TaskHandle;


void StartDefaultTask(void const * argument)
{
    ///释放信号量---释放第一次
    osSemaphoreRelease(ADS_BinarySemHandle);
    osSemaphoreRelease(Breath_BinarySemHandle);
    osSemaphoreRelease(touch_BinarySemHandle);

    ///LED的初始化
    HAL_GPIO_WritePin(LED_wifi_GPIO_Port,LED_wifi_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_charge_GPIO_Port,LED_charge_Pin,GPIO_PIN_SET);
    //HAL_GPIO_WritePin(LED_power_GPIO_Port,LED_wifi_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_SET);
    /****************wifi_init************************/

    M8266WIFI_Module_Init_Via_SPI();
    M8266WIFI_Test();
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
    for(;;)
    {
        osDelay(1);
    }
}
