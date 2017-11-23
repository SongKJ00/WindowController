/*
 * at24.c
 *
 * Created: 2017-11-13 14:37:12
 *  Author: Skj
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "at24.h"

void send_at24_byte(unsigned int data_addr, unsigned char data)
{
	/* START */
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	/* */
	TWDR = AT24_ADDRESS | ((data_addr >> 7) & 0x000e);
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWDR = data_addr & 0x00ff;
	TWCR = (1 << TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	
	_delay_ms(5);
}

unsigned char recv_at24_byte(unsigned char data_addr)
{
	unsigned char data;
	
	TWCR = (1<<TWINT) | (1<<TWSTA)  | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWDR = (unsigned char)AT24_ADDRESS | ((data_addr>>7) & 0x000e);
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWDR = data_addr & 0x00ff;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	
	TWDR = (unsigned char)AT24_ADDRESS | ((data_addr>>7) & 0x000e) | 0x0001;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	TWCR = (1 << TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	data = TWDR;
	
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
	
	return data;
}

void set_initial_time()
{
	char* str = "??:??";
	send_at24_str(str);
}

void send_at24_str(char* str)
{
	int idx = 0;

	while(*str)
	{
		send_at24_byte(idx, *(str++));
		idx++;
	}
}

void display_save_time()
{
	change_lcd_cursor(6, 1);
	write_lcd_byte(recv_at24_byte(0x00) + 0x30);
	write_lcd_byte(recv_at24_byte(0x01) + 0x30);
	write_lcd_byte(':');
	write_lcd_byte(recv_at24_byte(0x02) + 0x30);
	write_lcd_byte(recv_at24_byte(0x03) + 0x30);
}