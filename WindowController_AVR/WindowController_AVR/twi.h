/*
 * twi.h
 *
 * Created: 2017-11-06 15:01:21
 *  Author: Skj
 */ 


#ifndef TWI_H_
#define TWI_H_

#define CPU_CLOCK 16000000
#define TWI_FREQ 100000

//#define AT24_ADDRESS (unsigned char)0xa0
//#define RTC_ADDRESS (unsigned char)0x68

void init_twi();

#endif /* TWI_H_ */