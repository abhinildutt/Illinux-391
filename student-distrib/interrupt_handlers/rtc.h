#ifndef _RTC_H
#define _RTC_H

#include "../lib.h"
#include "i8259.h"

#define RTC_IRQ_NUM 8

#define RTC_PORT 0x70
#define RTC_DATA 0x71

#define REG_A		0x0A
#define REG_B		0x0B
#define REG_C		0x0C

#define disable_NMI_A   0x8A
#define disable_NMI_B   0x8B

void rtc_init();
void rtc_open();
void rtc_close();
void rtc_read();
void rtc_write();
void rtc_handler();






#endif