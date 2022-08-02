//
// Created by jjj on 2022/8/2.
//
#include "InitTask.h"
#include "ADS1298.h"
#include "adc.h"
#include "spo2.h"
#include "gpio.h"

void StartADS_Task(void const * argument)
{
    /* USER CODE BEGIN StartADS_Task */
    uint16_t status=0;
    uint32_t ads_save_count=0;
    uint8_t ads_save_buff[800];
    /* Infinite loop */
    for(;;)
    {
        ///�ȴ���ʼ�����ź���
        osSemaphoreWait(ADS_BinarySemHandle, osWaitForever);

        ///��ȡgpioE 7�ĵ�ƽΪ 1 ��ʱ��������
        while(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_7))
        {

        }
        ADS_RDATA();				    //READ DATA
        ADS_SendDataBuff();     	    //COPY DATA TO BUFF
        if(!pc_connect_flag)             ///�����ʱ�Ƕ���
        {
            query_M8266_state();
            osDelay(10);
        }
        for(int i=0;i<2;i++)
            for(int j=0;j<8;j++)
            {
                ///ads�����������
                ads_save_buff[ads_save_count]=ecg_eeg_sendbuff[i][j];
                ads_save_count+=1;
            }

        ///���������800һ��ͼ����ռ�
        if(ads_save_count==800)
        {
            ads_save_count=0;
            vTaskSuspendAll();
            M8266WIFI_SPI_Send_BlockData((uint8_t*)ads_save_buff,800,2000,0,NULL,0,&status);
            xTaskResumeAll();
        }

        if(adc2_flag)
        {
            adc2_flag=0;
            send_spo2();
            power_detect();
        }

        ADS_START(EEG_CS_Pin);				//START NEXT EEG CONVERT
        ADS_START(ECG_CS_Pin);				//START NEXT ECG CONVERT
        osDelay(1);
    }
    /* USER CODE END StartADS_Task */
}