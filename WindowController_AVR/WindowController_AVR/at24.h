/*
 * at24.h
 *
 * Created: 2017-11-13 14:37:03
 *  Author: Skj
 */ 


#ifndef AT24_H_
#define AT24_H_

#define AT24_ADDRESS (unsigned char)0xa0

void send_at24_byte( unsigned int data_addr, unsigned char data);
unsigned char recv_at24_byte(unsigned char data_addr);
void set_initial_time();
void send_at24_str(char* str);
void display_save_time();

#endif /* AT24_H_ */