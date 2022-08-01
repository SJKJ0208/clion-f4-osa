

#include "spo2.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"
#include "GUI.h"

uint8_t spo2_rx2Buffer[100];
int spo2_rec2Flag = 0;
//int OxygenSaturationOfBlood;
//uint8_t spo2_sendbuf;
	
__IO struct XueYangStruct
{
	__IO unsigned PH:4;
	__IO unsigned SearchTimeout:1;
	__IO unsigned reserve1:1;
	__IO unsigned LungRate:1;
	__IO unsigned Sync1:1;
	
	__IO unsigned WareShape:7;
	__IO unsigned Sync2:1;
	
	__IO unsigned PP:4;
	__IO unsigned ConnectionDetect:1;
	__IO unsigned SearchPP:1;
	__IO unsigned PR7:1;
	__IO unsigned Sync3:1;
	
	__IO unsigned PR:7;
	__IO unsigned Sync4:1;
	
	__IO unsigned OxygenSaturationOfBlood:7;
	__IO unsigned Sync5:1;
}XueYangStruct;

channel_union spo2_value;
uint8_t spo2_sendbuff[8];
void spo2_init()
{
    ///血氧使用的是串口3
	__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart3,spo2_rx2Buffer,100);
	 for(int i=0;i<8;i++)
	{
		spo2_sendbuff[i]=0;
	}
	spo2_sendbuff[0]=0xAA;
	spo2_sendbuff[1]=0xA2;
	spo2_sendbuff[7]=0xBB;
	spo2_value.INT32=0;
}
uint8_t miss_count=0;//spo2_value no update counts
void receive_spo2()
{
	if(spo2_rec2Flag)
		{
			uint8_t *tempp = (uint8_t*)&XueYangStruct;
			for(uint8_t i = 0; i < 6; ++i)
			{
                ///前6个取出
				tempp[i] = spo2_rx2Buffer[i];
			}
			spo2_rec2Flag = 0;
		}
	//else spo2_value.INT32=0;
		
		if(XueYangStruct.ConnectionDetect == 0)
			{
				if(XueYangStruct.Sync5 == 0)
				{
					spo2_value.INT32 = XueYangStruct.OxygenSaturationOfBlood;
					//send_spo2();
					miss_count=0;
				}
			}
			else
				miss_count++;
			if(miss_count==30)
			{
				spo2_value.INT32=0;
				miss_count=0;
			}
}
uint16_t spo2_send_result=0;

void send_spo2()
{
	uint16_t send_count=0;
	//spo2_value.INT32=97;//测试
	//LCD_ShowNum(40+24,155,spo2_value.INT32,4,16);		//显示数值
	for(int i=0;i<4;i++)
	{
        ///后面2-5四个————贞头AAA2 ++ 2-5 ++ 7 ++BB
		spo2_sendbuff[i+2]=(spo2_value.UINT8[i]+1);
	}
        ///第七个
		spo2_sendbuff[6]=spo2_value.UINT8[0]+spo2_value.UINT8[1]+spo2_value.UINT8[2]+spo2_value.UINT8[3];
		send_count=M8266WIFI_SPI_Send_BlockData((uint8_t*)spo2_sendbuff,8,10,0,NULL,0,&spo2_send_result);

//	if(send_count!=8)
//	send_count=M8266WIFI_SPI_Send_BlockData((uint8_t*)spo2_sendbuff,8,1000,0,NULL,0,&spo2_send_result);
	//spo2_value.INT32=0;
}

