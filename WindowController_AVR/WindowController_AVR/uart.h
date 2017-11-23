/*
 * uart.h
 *
 * Created: 2017-11-06 14:49:24
 *  Author: Skj
 */ 


#ifndef UART_H_
#define UART_H_

#define CPU_CLOCK 16000000
#define BAUD 9600
#define UBRR CPU_CLOCK/16/BAUD-1

void init_uart();
void send_ttl_byte(unsigned char data);
unsigned char recv_ttl_byte(void);
void send_bluetooth_byte(unsigned char data);
unsigned char recv_bluetooth_byte(void);
void check_time_recv();

extern unsigned char uart_rx_buff[100];
extern unsigned char set_time_data_buff[10];
extern volatile unsigned char time_rx_flag;
//extern volatile unsigned char idx;

#endif /* UART_H_ */