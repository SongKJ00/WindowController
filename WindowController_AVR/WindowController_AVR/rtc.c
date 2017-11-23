/*
 * rtc.c
 *
 * Created: 2017-11-13 15:20:38
 *  Author: Skj
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include "rtc.h"

RTCTime rtc_time;
char flag = 1;

void send_rtc_byte(unsigned char data_addr, unsigned char data)
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWDR = DS3231_ADDRESS;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWDR = data_addr;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWCR = (1<<TWINT) | (1<<TWEN) |(1<<TWSTO);
	
	_delay_ms(10);
}

unsigned char recv_rtc_byte(unsigned char data_addr)
{
	unsigned char data;
	
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWDR = DS3231_ADDRESS;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWDR = data_addr;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWDR = DS3231_ADDRESS | 0x01;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWCR = (1 << TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	data = TWDR;
	
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
	
	return data;
}

void send_rtc_time(unsigned char hour, unsigned char min, unsigned char sec)
{
	send_rtc_byte(0x00, sec);
	send_rtc_byte(0x01, min);
	send_rtc_byte(0x02, hour);
}

void recv_rtc_time()
{
	unsigned char hour, min, sec;
	
	hour = recv_rtc_byte(0x02);
	min = recv_rtc_byte(0x01);
	sec = recv_rtc_byte(0x00);
	
	rtc_time.hour = ((hour >> 4) * 10) + (hour & 0x0f);
	rtc_time.min = ((min >> 4) * 10) + (min & 0x0f);
	rtc_time.sec =  ((sec >> 4) * 10) + (sec & 0x0f);
}

void display_current_time()
{
	change_lcd_cursor(5, 0);
	
	if(rtc_time.hour >= 10)
	{
		write_lcd_byte((rtc_time.hour / 10) + 0x30);
		write_lcd_byte((rtc_time.hour % 10) + 0x30);	
	}
	else
	{
		write_lcd_byte('0');
		write_lcd_byte(rtc_time.hour + 0x30);		
	}
	write_lcd_byte(':');
	
	if(rtc_time.min >= 10)
	{
		write_lcd_byte((rtc_time.min / 10) + 0x30);
		write_lcd_byte((rtc_time.min % 10) + 0x30);
	}
	else
	{
		write_lcd_byte('0');
		write_lcd_byte(rtc_time.min + 0x30);		
	}
	write_lcd_byte(':');
	
	if(rtc_time.sec >= 10)
	{
		write_lcd_byte((rtc_time.sec / 10) + 0x30);
		write_lcd_byte((rtc_time.sec % 10) + 0x30);
	}
	else
	{
		write_lcd_byte('0');
		write_lcd_byte(rtc_time.sec + 0x30);
	}
	if(rtc_time.sec >= 30)
	{
		PORTB = 0x00;
	}
}

void compare_time()
{
	unsigned char at24_time_data[10];
	
	for(int i = 0; i < 4; i++)
	{
		at24_time_data[i] = recv_at24_byte(i);
	}
	
	if((rtc_time.hour/10 == at24_time_data[0]) && (rtc_time.hour%10 == at24_time_data[1]))
	{
		if((rtc_time.min/10 == 	at24_time_data[2]) && (rtc_time.min%10 == at24_time_data[3]))
		{
				PORTB = 0xff;
		}
		else
		{
			PORTB |= 0x20;
		}
	}
	else
	{
		PORTB |= 0x20;
	}
}