/*
 * twi.c
 *
 * Created: 2017-11-06 15:01:35
 *  Author: Skj
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include "twi.h"

void init_twi()
{
	/* TWI Bit Rate 설정 */
	TWBR = (CPU_CLOCK / TWI_FREQ - 16) / 2;
	/* Prescaler 1로 설정 */
	TWSR = (0<<TWPS1) | (0<<TWPS0);
}

//
//void send_at24_byte(unsigned int data_addr, unsigned char data)
//{
	///* START */
	//TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	//while(!(TWCR & (1<<TWINT)));
	//
	///* */
	//TWDR = AT24_ADDRESS | ((data_addr >> 7) & 0x000e);
	//TWCR = (1<<TWINT) | (1<<TWEN);
	//while(!(TWCR & (1<<TWINT)));
	//
	//TWDR = data_addr & 0x00ff;
	//TWCR = (1 << TWINT) | (1<<TWEN);
	//while(!(TWCR & (1<<TWINT)));
	//
	//TWDR = data;
	//TWCR = (1 << TWINT) | (1 << TWEN);
	//while (!(TWCR & (1 << TWINT)));
	//
	//TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	//
	//_delay_ms(5);
//}
//
//unsigned char read_at24_byte(unsigned char data_addr)
//{
	//unsigned char data;
	//
	//TWCR = (1<<TWINT) | (1<<TWSTA)  | (1<<TWEN);
	//while(!(TWCR & (1<<TWINT)));
	//
	//TWDR = (unsigned char)AT24_ADDRESS | ((data_addr>>7) & 0x000e);
	//TWCR = (1<<TWINT) | (1<<TWEN);
	//while(!(TWCR & (1<<TWINT)));
	//
	//TWDR = data_addr & 0x00ff;
	//TWCR = (1<<TWINT) | (1<<TWEN);
	//while(!(TWCR & (1<<TWINT)));
	//
	//TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	//while (!(TWCR & (1 << TWINT)));
	//
	//TWDR = (unsigned char)AT24_ADDRESS | ((data_addr>>7) & 0x000e) | 0x0001;
	//TWCR = (1<<TWINT) | (1<<TWEN);
	//while(!(TWCR & (1<<TWINT)));
	//
	//TWCR = (1 << TWINT) | (1<<TWEN);
	//while(!(TWCR & (1<<TWINT)));
	//
	//data = TWDR;
	//
	//TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
	//
	//return data;
//}