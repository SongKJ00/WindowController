/*
 * lcd.c
 *
 * Created: 2017-11-13 14:02:23
 *  Author: Skj
 */ 

#include "lcd.h"
#include <util/delay.h>

void init_lcd()
{
	DDR_LCD_CONTROL = 0xff;
	DDR_LCD_DATA = 0xff;
	
	write_lcd_command(0x38);
	write_lcd_command(0x38);
	write_lcd_command(0x38);
	write_lcd_command(0x0c);
	write_lcd_command(0x06);
	write_lcd_command(0x01);
	
	write_lcd_str("NOW ");
	change_lcd_cursor(0, 1);
	write_lcd_str("SET");
}

void write_lcd_command(unsigned char command)
{
	PORT_LCD_CONTROL &= ~(1 << LCD_RS);
	PORT_LCD_CONTROL &= ~(1 << LCD_RW);
	PORT_LCD_DATA = command;
	
	PORT_LCD_CONTROL |= (1 << LCD_E);
	_delay_ms(3);
	PORT_LCD_CONTROL &= ~(1 << LCD_E);
}

void write_lcd_byte(unsigned char data)
{
	PORT_LCD_CONTROL |= (1 << LCD_RS);
	PORT_LCD_CONTROL &= ~(1 << LCD_RW);
	PORT_LCD_DATA = data;
	
	PORT_LCD_CONTROL |= (1 << LCD_E);
	_delay_ms(5);
	PORT_LCD_CONTROL &= ~(1 << LCD_E);
}

void write_lcd_str(char* str)
{
	while(*str)
	{
		write_lcd_byte(*(str++));
	}
}

void change_lcd_cursor(unsigned char x, unsigned char y)
{
	PORT_LCD_CONTROL &= ~(1 << LCD_RS);
	PORT_LCD_CONTROL &= ~(1 << LCD_RW);
	
	if(y == 0)
	{
		PORT_LCD_DATA = x + 0x80;	
	}
	else if(y == 1)
	{
		PORT_LCD_DATA =  x + 0xc0;		
	}
	
	PORT_LCD_CONTROL |= (1 << LCD_E);
	_delay_ms(3);
	PORT_LCD_CONTROL &= ~(1 << LCD_E);
}

