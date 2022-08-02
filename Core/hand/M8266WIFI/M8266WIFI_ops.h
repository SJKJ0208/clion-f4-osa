/********************************************************************
 * M8266WIFI_ops.h
 * .Description
 *     header file of M8266WIFI HAL operations
 * .Copyright(c) Anylinkin Technology 2015.5-
 *     IoT@anylinkin.com
 *     http://www.anylinkin.com
 *     http://anylinkin.taobao.com
 *  Author
 *     wzuo
 *  Date
 *  Version
 ********************************************************************/
 
#ifndef _M8266WIFI_OPS_H_ 
#define _M8266WIFI_OPS_H_

#include "main.h"


void M8266WIFI_Module_delay_ms(uint16_t nms);
void M8266HostIf_Init(void);
void M8266HostIf_SPI_SetSpeed(uint32_t SPI_BaudRatePrescaler);


/////// BELOW FUNCTIONS ARE QUERYING OR CONFIGURATION VIA SPI
/////// MORE TO BE FOUND IN M8266WIFIDrv.h
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
uint8_t M8266WIFI_Module_Init_Via_SPI(void);
void M8266WIFI_Module_Hardware_Reset(void);
uint8_t M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(char* sta_ip, uint8_t max_wait_time_in_s);
uint8_t M8266WIFI_Config_Connection_via_SPI(uint8_t tcp_udp, uint16_t local_port, char* remote_addr, uint16_t remote_port, uint8_t link_no);

void M8266WIFI_Test(void);

uint8_t query_connectPC_state(void);
uint8_t query_connectWIFI_state(void);
void connect_PC(void);
void connect_wifi(void);
void query_M8266_state(void);

#endif
