/*
 * lcd.h
 *
 * Created: 2017-11-13 13:58:59
 *  Author: Skj
 */ 


#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>

#define F_CPU 16000000

#define DDR_LCD_CONTROL DDRG
#define DDR_LCD_DATA DDRA
#define PORT_LCD_CONTROL PORTG
#define PORT_LCD_DATA PORTA

#define LCD_RS PORTG0
#define LCD_RW PORTG1
#define LCD_E PORTG2

void init_lcd();
void write_lcd_command(unsigned char command);
void write_lcd_byte(unsigned char data);
void write_lcd_str(char* str);
void change_lcd_cursor(unsigned char x, unsigned char y);

#endif /* LCD_H_ */