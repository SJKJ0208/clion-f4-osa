
#include "main.h"
#include "stdio.h"
#include "string.h"	
#include "spi.h"
#include "M8266WIFIDrv.h"


/***********************************************************************************
 * M8266HostIf_SPI_SetSpeed                                                        *
 * Description                                                                     *
 *    To setup the SPI Clock Speed for M8266WIFI module                            *
 * Parameter(s):                                                                   *
 *    SPI_BaudRatePrescaler: SPI BaudRate Prescaler                                *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_SPI_SetSpeed(u32 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	__HAL_SPI_DISABLE(&hspi3); //关闭 SPI
	hspi3.Instance->CR1&=0XFFC7; //位 3-5 清零，用来设置波特率
	hspi3.Instance->CR1|=SPI_BaudRatePrescaler; //设置 SPI 速度
	__HAL_SPI_ENABLE(&hspi3); //使能 SPI
} 
//////////////////////////////////////////////////////////////////////////////////////
// BELOW FUNCTIONS ARE REQUIRED BY M8266WIFIDRV.LIB. 
// PLEASE IMPLEMENTE THEM ACCORDING TO YOUR HARDWARE
//////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
 * M8266HostIf_Set_nRESET_Pin                                                      *
 * Description                                                                     *
 *    To Outpout HIGH or LOW onto the GPIO pin for M8266WIFI nRESET                *
 *    You may update the macros of GPIO PIN usages for nRESET from brd_cfg.h       *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    1. level: LEVEL output to nRESET pin                                         *
 *              0 = output LOW  onto nRESET                                        *
 *              1 = output HIGH onto nRESET                                        *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_Set_nRESET_Pin(u8 level)
{
    if(level!=0)
	{
		 HAL_GPIO_WritePin(M8266WIFI_RESET_GPIO_Port,M8266WIFI_RESET_Pin, GPIO_PIN_SET);
	}
	else
		 HAL_GPIO_WritePin(M8266WIFI_RESET_GPIO_Port,M8266WIFI_RESET_Pin, GPIO_PIN_RESET);
}
/***********************************************************************************
 * M8266HostIf_Set_SPI_nCS_PIN                                                     *
 * Description                                                                     *
 *    To Outpout HIGH or LOW onto the GPIO pin for M8266WIFI SPI nCS               *
 *    You may update the macros of GPIO PIN usages for SPI nCS from brd_cfg.h      *
 *    You are not recommended to modify codes below please                         *
 * Parameter(s):                                                                   *
 *    1. level: LEVEL output to SPI nCS pin                                        *
 *              0 = output LOW  onto SPI nCS                                       *
 *              1 = output HIGH onto SPI nCS                                       *
 * Return:                                                                         *
 *    None                                                                         *
 ***********************************************************************************/
void M8266HostIf_Set_SPI_nCS_Pin(u8 level)
{
	if(level!=0)
	{
		 HAL_GPIO_WritePin(M8266WIFI_CS_GPIO_Port,M8266WIFI_CS_Pin, GPIO_PIN_SET);
	}
	else
		 HAL_GPIO_WritePin(M8266WIFI_CS_GPIO_Port,M8266WIFI_CS_Pin, GPIO_PIN_RESET);
}

/***********************************************************************************
 * M8266WIFIHostIf_delay_us                                                        *
 * Description                                                                     *
 *    To loop delay some micro seconds.                                            *
 * Parameter(s):                                                                   *
 *    1. nus: the micro seconds to delay                                           *
 * Return:                                                                         *
 *    none                                                                         *
 ***********************************************************************************/
void M8266HostIf_delay_us(u8 nus)
{
  RCCdelay_us(nus);
}

/***********************************************************************************
 * M8266HostIf_SPI_ReadWriteByte                                                   *
 * Description                                                                     *
 *    To write a byte onto the SPI bus from MCU MOSI to the M8266WIFI module       *
 *    and read back a byte from the SPI bus MISO meanwhile                         *
 *    You may update the macros of SPI usage from brd_cfg.h                        *
 * Parameter(s):                                                                   *
 *    1. TxdByte: the byte to be sent over MOSI                                    *
 * Return:                                                                         *
 *    1. The byte read back from MOSI meanwhile                                    *                                                                         *
 ***********************************************************************************/
u8 M8266HostIf_SPI_ReadWriteByte(u8 TxdByte)
{
  uint8_t Rxdata;
  //HAL_SPI_TransmitReceive(&hspi3,&TxdByte,&Rxdata,1, 1000);
	HAL_SPI_TransmitReceive_DMA(&hspi3,&TxdByte,&Rxdata,1);
  return Rxdata; //返回收到的数据
}
