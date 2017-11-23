/*
 * rtc.h
 *
 * Created: 2017-11-13 15:20:47
 *  Author: Skj
 */ 


#ifndef RTC_H_
#define RTC_H_

#define DS3231_ADDRESS (unsigned char)0xd0

typedef struct _rtc_time
{
	unsigned char sec;
	unsigned char min;
	unsigned char hour;
}RTCTime;

void send_rtc_byte(unsigned char data_addr, unsigned char data);
unsigned char recv_rtc_byte(unsigned char data_addr);
void send_rtc_time(unsigned char hour, unsigned char min, unsigned char sec);
void recv_rtc_time();
void display_current_time();
void compare_time();

extern RTCTime rtc_time;

#endif /* RTC_H_ */