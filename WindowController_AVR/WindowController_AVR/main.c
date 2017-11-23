/*
 * SmartWindow.c
 *
 * Created: 2017-11-06 14:14:10
 * Author : Skj
 */ 
#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi.h"
#include "uart.h"
#include "lcd.h"
#include "at24.h"
#include "rtc.h"

volatile unsigned char idx;

ISR(USART0_RX_vect)
{
	//data = UDR0;
	uart_rx_buff[idx] = UDR0;
	idx++;
	//send_ttl_byte(uart_rx_buff[idx]);
	if(idx >= 8)
	{
		idx = 0;
		time_rx_flag = 1;
	}
}

int main(void)
{
    ///* Replace with your application code */
	cli();
	
	unsigned char data;
	unsigned char sec, min, hour;
	
	init_uart();
	init_twi();
	init_lcd();
	DDRB = 0xff;
	
	sei();
	
	//write_lcd_str("NOW ");
	//change_lcd_cursor(0, 1);
	//write_lcd_str("SET");
	
	/* 저장한 시간 표시 */
	//change_lcd_cursor(6, 1);
	//write_lcd_byte(recv_at24_byte(0x00) + 0x30);
	//write_lcd_byte(recv_at24_byte(0x01) + 0x30);
	//write_lcd_byte(':');
	//write_lcd_byte(recv_at24_byte(0x02) + 0x30);
	//write_lcd_byte(recv_at24_byte(0x03) + 0x30);
	
	display_save_time();
	
	/* 1초마다 현재 시간 표시 */
    while(1)
	{
		//send_ttl_byte('a');
		if(time_rx_flag == 1)
		{
			time_rx_flag = 0;
			idx = 0;
			
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
		//check_time_recv();
		recv_rtc_time();
		display_current_time();
		compare_time();
	}
}

