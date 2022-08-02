/********************************************************************
wifi_test
 ********************************************************************/
 
#include "stdio.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"
#include "InitTask.h"

#define TEST_CONNECTION_TYPE   1 //0 = WIFI module as UDP, 1 = WIFI module as TCP Client, 2= WIFI module as TCP Server
//#define TEST_LOCAL_PORT  			4321  //local port
//#define TEST_REMOTE_ADDR    		"192.168.1.109"
//#define TEST_REMOTE_PORT  	    8888


extern  int wifi_connect_flag;

uint8_t test1;
void M8266WIFI_Test(void)
{
	 u16 status = 0;
	 u8  link_no=0;
	#if (TEST_CONNECTION_TYPE==0)        //// if module as UDP
        #define TEST_REMOTE_ADDR    		"192.168.1.107"
        #define TEST_REMOTE_PORT  	        8080
    #elif (TEST_CONNECTION_TYPE==1)      //// if module as TCP Client


	#define TEST_REMOTE_ADDR                "192.168.1.109"	//508 csf just use to test
	#define TEST_REMOTE_ADDR2    	        "192.168.1.109"  // 508 yjt
    #define TEST_REMOTE_PORT  	    8888
    #define TEST_LOCAL_PORT          0

    #elif (TEST_CONNECTION_TYPE==2)     //// if module   as TCP Server
    #define TEST_REMOTE_ADDR      "1.1.1.1" // no need remote ip and port upon setup connection. below values just for filling and any value would be ok.
    #define TEST_REMOTE_PORT  	  1234  	 //(Chinese:
#else
#error WRONG TEST_CONNECTION_TYPE defined !
#endif
	if(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, link_no,5, &status)==0)
	{		
		connect_PC();
	}
	else  // else: setup connection successfully, we could config it
	{
		HAL_GPIO_WritePin(LED_wifi_GPIO_Port,LED_wifi_Pin,GPIO_PIN_RESET);
	}
}

void connect_PC(void)
{

	if(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, 0, 5, NULL)==0)
	{
		if(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, TEST_REMOTE_ADDR2, TEST_REMOTE_PORT, 0, 5, NULL)==0)
		{
			pc_connect_flag=0;
		}
		else
		{
			pc_connect_flag=1;
			HAL_GPIO_WritePin(LED_wifi_GPIO_Port,LED_wifi_Pin,GPIO_PIN_RESET);
		}
	}
	else{
		pc_connect_flag=1;
		HAL_GPIO_WritePin(LED_wifi_GPIO_Port,LED_wifi_Pin,GPIO_PIN_RESET);
	}
}


void query_M8266_state(void)
{			
	uint8_t test;
	if(wifi_connect_flag==1)
	{
		M8266WIFI_SPI_Query_Connection(0,NULL,&test,NULL,NULL,NULL,NULL);
		if(test==6|test==0|test==1)
		{
			connect_PC();
		}
		if(test==3)
		pc_connect_flag=1;
	}
	else
	{
		if(query_connectWIFI_state())
		{
			wifi_connect_flag=1;
		}
		else
		{
			wifi_connect_flag=0;
			connect_wifi();
		}
	}
}

