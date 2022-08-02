/********************************************************************
wifi module initialization
wifi module connect to wifi net (sta mode)
 ********************************************************************/
#include "main.h"
#include "string.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"


#define M8266WIFI_INTERFACE_SPI  SPI3

#define op_mode 3
#define wifi_name "508"
#define wifi_password "508508508508"

///wifi的标签
int wifi_connect_flag=0;

void M8266WIFI_Module_delay_ms(u16 nms)
{
	HAL_Delay(nms);
}

/************************************************************************************************************************
 * M8266WIFI_Module_Hardware_Reset                                                                                      *
 * Description                                                                                                          *
 *    1. To perform a hardware reset to M8266WIFI module via the nReset Pin                                             *
 *       and bring M8266WIFI module to boot up from external SPI flash                                                  *
 *    2. In order to make sure the M8266WIFI module bootup from external                                                *
 *       SPI flash, nCS should be low during Reset out via nRESET pin                                                   *
 * Parameter(s):                                                                                                        *
 *    none                                                                                                              *
 * Return:                                                                                                              *
 *    none                                                                                                              *
 ************************************************************************************************************************/
void M8266WIFI_Module_Hardware_Reset(void) // total 800ms  (Chinese: 本例子中这个函数的总共执行时间大约800毫秒)
{
	M8266HostIf_Set_SPI_nCS_Pin(0);   			// Module nCS==ESP8266 GPIO15 as well, Low during reset in order for a normal reset (Chinese: 为了实现正常复位，模块的片选信号nCS在复位期间需要保持拉低)
	 M8266WIFI_Module_delay_ms(1); 	    		// delay 1ms, adequate for nCS stable (Chinese: 延迟1毫秒，确保片选nCS设置后有足够的时间来稳定)
	
	M8266HostIf_Set_nRESET_Pin(0);					// Pull low the nReset Pin to bring the module into reset state (Chinese: 拉低nReset管脚让模组进入复位状态)
	 M8266WIFI_Module_delay_ms(5);      		// delay 5ms, adequate for nRESET stable(Chinese: 延迟5毫秒，确保片选nRESER设置后有足够的时间来稳定，也确保nCS和nRESET有足够的时间同时处于低电平状态)
	                                        // give more time especially for some board not good enough
	                                        //(Chinese: 如果主板不是很好，导致上升下降过渡时间较长，或者因为失配存在较长的振荡时间，所以信号到轨稳定的时间较长，那么在这里可以多给一些延时)
	
	 M8266HostIf_Set_nRESET_Pin(1);					// Pull high again the nReset Pin to bring the module exiting reset state (Chinese: 拉高nReset管脚让模组退出复位状态)
	M8266WIFI_Module_delay_ms(300); 	  		// at least 18ms required for reset-out-boot sampling boottrap pin (Chinese: 至少需要18ms的延时来确保退出复位时足够的boottrap管脚采样时间)
	                                        // Here, we use 300ms for adequate abundance, since some board GPIO, (Chinese: 在这里我们使用了300ms的延时来确保足够的富裕量，这是因为在某些主板上，)
																					// needs more time for stable(especially for nRESET) (Chinese: 他们的GPIO可能需要较多的时间来输出稳定，特别是对于nRESET所对应的GPIO输出)
																					// You may shorten the time or give more time here according your board v.s. effiency
																					// (Chinese: 如果你的主机板在这里足够好，你可以缩短这里的延时来缩短复位周期；反之则需要加长这里的延时。
																					//           总之，你可以调整这里的时间在你们的主机板上充分测试，找到一个合适的延时，确保每次复位都能成功。并适当保持一些富裕量，来兼容批量化时主板的个体性差异)
	M8266HostIf_Set_SPI_nCS_Pin(1);         // release/pull-high(defualt) nCS upon reset completed (Chinese: 释放/拉高(缺省)片选信号
	M8266WIFI_Module_delay_ms(493);
	                                        // (Chinese: 延迟大约500毫秒，来等待模组成功复位后完成自己的启动过程和自身初始化，包括串口信息打印。但是此时不影响模组和单片主机之间的通信，这里的时间可以根据需要适当调整.如果调整缩短了这里的时间，建议充分测试，以确保系统(时序关系上的)可靠性)
}
/***********************************************************************************
 * M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip                              *
 * Description                                                                     *
 *    Wait M8266WIFI in STA mode connecting to AP/Router and get ip address        *
 *    via SPI API functions in M8266WIFIDrv.lib                                    *
 * Parameter(s):                                                                   *
 *    1. sta_ip: if success, sta_ip return the ip address achieved                 *
 *    2. max_wait_time_in_s: max timeout to wait in seconds                        *
 * Return:                                                                         *
 *       1 = M8266WIFI module connects AP/router and gets a valid IP               *
 *           from AP/router before the timeout succefully                          *
 *       0 = M8266WIFI module fails to connect the AP/router and get               *
 *           a valid IP before the timeout                                         *
 ***********************************************************************************/
u8 M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(char* sta_ip, u8 max_wait_time_in_s)
{
	 u16  i;
	 u16  status=0;
	 
	 for(i=0; i<1*max_wait_time_in_s; i++)  // max wait
	 {
		   if(   (M8266WIFI_SPI_Get_STA_IP_Addr(sta_ip , &status) ==1)
				   && (strcmp(sta_ip, "0.0.0.0")!=0) )
               ///如果说不为0000即获取到了ip
           break;
     			 
			M8266WIFI_Module_delay_ms(1000);
			continue;
   }
   if(i>=1*max_wait_time_in_s)
		 return 0; // false
	 
	 return 1;  // true
}

/***********************************************************************************
 * M8266WIFI_Module_Init_Via_SPI(void)                                             *
 * Description                                                                     *
 *    To perform a Initialization sequency to M8266WIFI module via SPI I/F         *
 *    (1) Reboot the M8266WIFI module via nRESET pin                               *
 *    (2) Wait and Check if the M8266WIFI module gets an valid IP address          *
 *        if the module in STA or STA+AP mode                                      *
 *    Call this function after Host SPI Interface Initialised                      *
 *    if use SPI interface to config module                                        *
 * Parameter(s):                                                                   *
 *    none                                                                         *
 * Return:                                                                         *
 * Return:                                                                         *
 *       0 = failed                                                                *
 *       1 = success                                                               * 
 ***********************************************************************************/
//volatile uint32_t spi_result=0;
 u8 M8266WIFI_Module_Init_Via_SPI(void)
 {
	 
	u32  	spi_clk = 40000000;
	u8   	sta_ap_mode = 0;
	char 	sta_ip[15+1]={0};
	u16  	status = 0;

	//第一步：对模组执行硬复位时序(在片选nCS拉低的时候对nRESET管脚输出低高电平)，并等待模组复位启动完毕
	M8266WIFI_Module_Hardware_Reset();

	//第二步，在确保SPI底层通信可靠的前提下，调整SPI时钟尽可能的快，以免支持最快速度通信。本模组最大可以支持40MHz的SPI频率
	 M8266HostIf_SPI_SetSpeed(SPI_BAUDRATEPRESCALER_4);
	 spi_clk = 10500000;
     M8266WIFI_Module_delay_ms(1);

	 //第三步：调用M8266HostIf_SPI_Select()。 在正式调用驱动API函数和模组进行通信之前，调用M8266HostIf_SPI_Select()来告诉驱动使用哪个SPI以及SPI的时钟有多快，这一点非常重要。
	 //如果没有调用这个API，单片机主机和模组之间将可能将无法通信)
    if(M8266HostIf_SPI_Select((uint32_t)M8266WIFI_INTERFACE_SPI, 	spi_clk , &status)==0)
		{
		   while(1)
			 HAL_Delay(1000);
		}

         // (第四步：配置模组)
        //如果你希望改变模组的op_mode，不使用模组启动时缺省op_mode，你可以这里改成 #if 1，并调整下面的API函数里的相关参数值
		
	if(M8266WIFI_SPI_Set_Opmode(op_mode, 0, &status)==0)
        // set to AP Only mode, not saved // 1=STA Only, 2=AP Only, 3=STA+AP
        return 0;  //(Chinese: 设置为AP Only模式。1=STA Only, 2=AP Only, 3=STA+AP)
            // 查询当前的op_mode，如果处于STA模式或者STA+AP模式，那么可根据需要执行配网去链接上第三方热点/路由器，并等待获取ip地址)

    if(M8266WIFI_SPI_Get_Opmode(&sta_ap_mode, &status)==0)
	{
        return 0;
    }
	if(  (sta_ap_mode == 1)   // if STA mode (Chinese: 如果是STA模式
	   ||(sta_ap_mode == 3))  // if STA+AP mode(Chinese: 如果是STA+AP模式)
	{
		 //将其中的SSID和密码改成你所期望连接的热点/路由器的)
		if( M8266WIFI_SPI_Set_Opmode(sta_ap_mode,0,&status)==1)
             {
                    u16    wifi_status=0;
                    u16    wifi_test=3;
                    wifi_test=M8266WIFI_SPI_STA_Connect_Ap(wifi_name,wifi_password,0,10,&wifi_status);
                    if(wifi_test==1)
                    {
                         wifi_connect_flag=1;
                    }
             }
             else
             {
                     while(1)
                     {
                     HAL_Delay(1000);
                     }
            }
			 // Wait the module to got ip address if it works in STA mode
			// (Chinese: 如果模组工作在包含STA的模式下，需要等待模组从所连接的热点/路由器获取到ip地址。因为获取到ip地址，是后面进行套接字通信的前提，因此，这里需要等待，确保模组获取到ip真正连接成功
            if(M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(sta_ip, 10)==0) // max wait 10s to get sta ip
			 {
                //(Chinese: 最多等待10秒。max_wait_time_in_s可以根据实际情形调整。但这个时间不是实际等待的时间，而是最大等待时间超时上限。这个函数会在获取到ip地址或等待时间到达这里的超时上限时返回)
				 return 0;
		   }
	} 
  return 1;
 }
 
void connect_wifi(void)
 {
	  M8266WIFI_SPI_STA_Connect_Ap(wifi_name,wifi_password,0,0,NULL);
 }
uint8_t query_connectWIFI_state(void)
{
	 uint8_t test;
	 M8266WIFI_SPI_Get_STA_Connection_Status(&test,NULL);
	 if(test==5)///连接成功
		 return 1;
	 else
		 return 0;
 } 