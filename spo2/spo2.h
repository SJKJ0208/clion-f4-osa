#ifndef __SPO2_H
#define __SPO2_H

#include "stdbool.h"
#include "main.h"
#include "usart.h"
#include "ADS1298.h"

extern uint8_t spo2_rx2Buffer[100];
extern int spo2_rec2Flag;
extern uint16_t spo2_send_result;


void spo2_init(void);
void receive_spo2(void);
void send_spo2(void);

#endif

