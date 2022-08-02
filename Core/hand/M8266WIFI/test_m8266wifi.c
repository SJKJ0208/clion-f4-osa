/********************************************************************
wifi_test
 ********************************************************************/
 
#include "stdio.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"
#include "lcd.h"
#include "gui.h"

#define TEST_CONNECTION_TYPE   1 //0 = WIFI module as UDP, 1 = WIFI module as TCP Client, 2= WIFI module as TCP Server
//#define TEST_LOCAL_PORT  			4321  //local port
//#define TEST_REMOTE_ADDR    		"192.168.1.109"
//#define TEST_REMOTE_PORT  	    8888

extern  int pc_connect_flag;
extern  int wifi_connect_flag;

uint8_t test1;
void M8266WIFI_Test(void)
{
	 u16 status = 0;
	 u8  link_no=0;
	#if (TEST_CONNECTION_TYPE==0)        //// if module as UDP (Chinese:如果模组作为UDP，则可以指定目标地址和端口，也可以随便填充，在发送数据时再设置或更改)
        #define TEST_REMOTE_ADDR    		"192.168.1.107"
        #define TEST_REMOTE_PORT  	    8080
    #elif (TEST_CONNECTION_TYPE==1)      //// if module as TCP Client (Chinese:如果模组作为TCP客户端，当然必须指定目标地址和目标端口，即模组所要去连接的TCP服务器的地址和端口)
    //#define TEST_REMOTE_ADDR    	    	"169.254.183.49"  //
    //#define TEST_REMOTE_ADDR    	 	    "192.168.1.100" 	//
    //#define TEST_REMOTE_ADDR    	 	    "192.168.1.105" 	//401
	#define TEST_REMOTE_ADDR                "192.168.1.130"	//508 csf just use to test
	#define TEST_REMOTE_ADDR2    	        "192.168.1.130"  // 508 yjt
    //	#define TEST_REMOTE_ADDR    	    "192.168.137.1"  //508阿祖地址
    //  #define TEST_REMOTE_ADDR    	    "192.168.123.185"  // 508铨坤地址
    //  #define TEST_REMOTE_ADDR    	 	"169.254.183.49" //redmi凯文地址
    //	#define TEST_REMOTE_ADDR    	 	"172.172.247.128"
    //	#define TEST_REMOTE_ADDR    	 	"192.168.43.106" //redmi凯文地址
    #define TEST_REMOTE_PORT  	    8888		//
    #define TEST_LOCAL_PORT          0          //随机端口

    #elif (TEST_CONNECTION_TYPE==2)     //// if module   as TCP Server (Chinese:如果模组作为TCP服务器)
    #define TEST_REMOTE_ADDR      "1.1.1.1" // no need remote ip and port upon setup connection. below values just for filling and any value would be ok.
    #define TEST_REMOTE_PORT  	  1234  	 //(Chinese: 创建TCP服务器时，不需要指定目标地址和端口，这里的数据只是一个格式填充，随便填写。
#else
#error WRONG TEST_CONNECTION_TYPE defined !
#endif			
//创建套接字连接
  //  u8 M8266WIFI_SPI_Setup_Connection(u8 tcp_udp, u16 local_port, char remote_addr, u16 remote_port, u8 link_no, u8 timeout_in_s, u16* status);
	if(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, link_no,5, &status)==0)
	{		
		// printf("fail to conect to the server\r\n");
		connect_PC();
		//M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, link_no,1, &status);
			 //pc_connect_flag=0;
			 //Gui_StrCenter(0,90,RED,BLUE,"fail to conect to the server",16,1);		 
    	//M8266WIFI_SPI_Query_Connection(0,NULL,&test1,NULL,NULL,NULL,NULL);
//			if(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, link_no, 10, &status)==0)
//			{break;}
	}
	else  // else: setup connection successfully, we could config it (Chinese: 创建套接字成功，就可以配置套接字）
	{
		HAL_GPIO_WritePin(LED_wifi_GPIO_Port,LED_wifi_Pin,GPIO_PIN_RESET);
			//printf("success connect to the server\r\n");
			//M8266WIFI_SPI_Send_Data("hello world",sizeof("hello world"),link_no,&status); 
		  //pc_connect_flag=1;
	}
} 
void connect_PC(void)
{
    ///多次链接直到连接上
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
		if(test==3)///正常连接状态
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

