/*
 * uart.c
 *
 * Created: 2017-11-06 14:51:02
 *  Author: Skj
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

unsigned char uart_rx_buff[100];
unsigned char set_time_data_buff[10];

volatile unsigned char time_rx_flag;
volatile unsigned char idx;

void init_uart()
{
	/* UART1 - HC-06 */
	/* 보레이트 설정 */
	UBRR0H = (unsigned char)((UBRR)>>8);
	UBRR0L = (unsigned char)(UBRR);
	/* UART 수신, 송신 Enable */
	UCSR0B = (1<<RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	/* UART 프레임 포맷 설정, 8bit data, 1 stop bit */
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (1 << UPM01) | (1 << UPM00) | (0 << USBS0);
	
	/* UART2 - USB to TTL */
	/* 보레이트 설정 */
	UBRR1H = (unsigned char)((UBRR)>>8);
	UBRR1L = (unsigned char)(UBRR);
	/* UART 수신, 송신 Enable */
	UCSR1B = (1<<RXEN1) | (1<<TXEN1);
	/* UART 프레임 포맷 설정, 8bit data, 1 stop bit */
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10) | (0 << UPM11) | (0 << UPM10) | (0 << USBS1);

}

void send_ttl_byte(unsigned char data)
{
	/* 전송 버퍼가 빌 때까지 대기 */
	while(!(UCSR1A & (1<<UDRE1)));
	UDR1 = data;
}

unsigned char recv_ttl_byte(void)
{
	/* 수신 가능할 때까지 대기 */
	while(!(UCSR1A & (1<<RXC1)));
	return UDR1;
}

void send_bluetooth_byte(unsigned char data)
{
	/* 전송 버퍼가 빌 때까지 대기 */
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

unsigned char recv_bluetooth_byte(void)
{
	/* 수신 가능할 때까지 대기 */
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void check_time_recv()
{
	if(time_rx_flag == 1)
	{
		time_rx_flag = 0;
		idx = 0;
		for(int i = 0; i < 8; i++)
		{
			send_ttl_byte(uart_rx_buff[i]);
		}
		
		/* PM인 경우 */
		if(uart_rx_buff[6]=='P')
		{
			//24시간 기준으로 바꿈
			set_time_data_buff[0] = (uart_rx_buff[1]-0x30) + 1;
			set_time_data_buff[1] = (uart_rx_buff[2]-0x30) + 2;
			if(set_time_data_buff[0] == 2 && set_time_data_buff[1] == 4)
			{
				set_time_data_buff[0] = 1;
				set_time_data_buff[1] = 2;
			}
			set_time_data_buff[2] = (uart_rx_buff[4]-0x30);
			set_time_data_buff[3] = (uart_rx_buff[5]-0x30);
		}
		else
		{
			set_time_data_buff[0] = (uart_rx_buff[1]-0x30);
			set_time_data_buff[1] = (uart_rx_buff[2]-0x30);
			if((set_time_data_buff[0] == 1) && set_time_data_buff[1] == 2)
			{
				set_time_data_buff[0] = 0;
				set_time_data_buff[1] = 0;
			}
			set_time_data_buff[2] = (uart_rx_buff[4]-0x30);
			set_time_data_buff[3] = (uart_rx_buff[5]-0x30);
		}
		
		send_at24_byte(0, set_time_data_buff[0]);
		send_at24_byte(1, set_time_data_buff[1]);
		send_at24_byte(2, set_time_data_buff[2]);
		send_at24_byte(3, set_time_data_buff[3]);
		
		change_lcd_cursor(6, 1);
		write_lcd_byte(recv_at24_byte(0) + 0x30);
		write_lcd_byte(recv_at24_byte(1) + 0x30);
		write_lcd_byte(':');
		write_lcd_byte(recv_at24_byte(2) + 0x30);
		write_lcd_byte(recv_at24_byte(3) + 0x30);
	}
}