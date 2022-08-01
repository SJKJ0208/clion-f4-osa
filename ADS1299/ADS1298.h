/*==============================================================================
* Reference: https://github.com/OpenBCI/OpenBCI-V2hardware-DEPRECATED/tree/master/Arduino/Libraries/ADS1299
* Datasheet: http://www.ti.com/lit/ds/symlink/ads1298.pdf
* Author: Kravchenko S.
*
*
*==============================================================================*/

#ifndef __ADS1298_H
#define __ADS1298_H

#include "stm32f4xx_hal.h"
#include "stdbool.h"
#include "main.h"
#include "usart.h" 
#include "spi.h"
//#include "cmsis_os.h"

#define _WAKEUP 	0x02 // Wake-up from standby mode
#define _STANDBY 	0x04 // Enter Standby mode
#define _RESET 		0x06 // Reset the device registers to default
#define _START 		0x08 // Start and restart (synchronize) conversions
#define _STOP 		0x0A // Stop conversion

#define _RDATAC 	0x10 // Enable Read Data Continuous mode (default mode at power-up)
#define _SDATAC 	0x11 // Stop Read Data Continuous mode
#define _RDATA 		0x12 // Read data by command; supports multiple read back


#define WREG			0X40	//写入010r rrrr 000n nnnn
//Register Addresses
#define ID 0x00   //复位后为xxx11110
#define CONFIG1 0x01
#define CONFIG2 0x02 //配置测试信号，可参照ads1299pdf47页，复位后为c0h
#define CONFIG3 0x03   //RLD操作
#define LOFF 0x04     //导联脱落控制寄存器
#define CH1SET 0x05
#define CH2SET 0x06
#define CH3SET 0x07
#define CH4SET 0x08
#define CH5SET 0x09
#define CH6SET 0x0A
#define CH7SET 0x0B
#define CH8SET 0x0C
#define BIAS_SENSP 0x0D //RLD 正信号导出寄存器
#define BIAS_SENSN 0x0E //RLD 负信号导出寄存器
#define LOFF_SENSP 0x0F  //正信号导联脱落检测字段说明
#define LOFF_SENSN 0x10 //：负信号导联脱落检测寄存器
#define LOFF_FLIP 0x11  //导联脱落翻转寄存器
#define LOFF_STATP 0x12
#define LOFF_STATN 0x13//
#define GPIO 0x14
#define MISC1 0x15  //MISC1
#define MISC2 0x16  //MISC2
#define CONFIG4 0x17 //pdf60页  

#define CONFIG1_DAISY_EN				0x00 //菊花链模式，CONFIG1
#define CONFIG1_DAIST_MUL				0x40//多读回模式，CONFIG1
#define CONFIG1_CLK_EN_OUT			0x00
#define CONFIG1_CLK_DIS_OUT			0x20//是否启用振荡器时钟输出

#define CONFIG1_DR_250SPS				0x06 //输出数据速率为250sps
#define CONFIG1_DR_500SPS				0x05 
#define CONFIG1_DR_1KSPS				0x04 
#define CONFIG1_DR_2KSPS				0x03 


#define CONFIG2_TESTSIGNAL_1HZ      0X00
#define CONFIG2_TESTSIGNAL_2HZ      0X01
#define CONFIG2_TESTSIGNAL_NOUSE    0X02


#define CHx_SET_TESTSIGNAL      0X05

typedef union
{
	uint8_t UINT8[4];
	int32_t INT32;
}channel_union;

extern bool INIT_DRDY ; // Flag to ready data from ADS1298
extern int32_t channelData [8];
extern int32_t stat_1;
extern uint32_t test500hz_count;
//extern uint32_t ads_save_count;
extern uint8_t ecg_eeg_sendbuff[4][8];

void ADS_Init(void);

void ADS_RDATAC(uint16_t CS_PIN);
void ADS_SDATAC(uint16_t CS_PIN);
void ADS_RESET(uint16_t CS_PIN);
void ADS_START(uint16_t CS_PIN);
void ADS_STOP(uint16_t CS_PIN);
void ADS_WAKEUP(uint16_t CS_PIN);

uint8_t ADS_getDeviceID(uint16_t CS_PIN);
uint8_t ADS_RREG(uint8_t _address,uint16_t CS_PIN);

void ADS_RREGS(uint8_t _address, uint8_t _numRegistersMinusOne);
void ADS_WREG(uint8_t _address, uint8_t _value,uint16_t CS_PIN);
void ADS_WREGS(uint8_t _address, uint8_t _numRegistersMinusOne);
void ADS_updateChannelData(void);
void ADS_RDATA(void);

void ADS_SendDataBuff(void);
int Uint24ToUint32(int NumIn);//转32位int
uint8_t transferSPI(uint8_t send);

void change_leadoff_detect(bool state);
void start_ch2_sample(bool state);
void start_ch1_sample(bool state);
void ADS1299_SendState(void);


#endif //__ADS1298_H
