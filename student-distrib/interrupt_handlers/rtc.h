#ifndef _RTC_H
#define _RTC_H

#include "../lib.h"
#include "../i8259.h"

#define RTC_IRQ_NUM 8

#define RTC_PORT 0x70
#define RTC_DATA 0x71

#define REG_A		0x0A
#define REG_B		0x0B
#define REG_C		0x0C

#define disable_NMI_A   0x8A
#define disable_NMI_B   0x8B
#define disable_NMI_C   0x8C

void rtc_init();
void rtc_open();
void rtc_close();
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, void* buf, int32_t nbytes);
void rtc_handler();



#endif
