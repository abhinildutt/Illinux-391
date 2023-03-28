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
int32_t rtc_open(const uint8_t* filename);
int32_t rtc_close(int32_t fd);
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, void* buf, int32_t nbytes);
void rtc_handler();

int32_t set_rtc_freq(int32_t freq);
int32_t freq_to_rate(int32_t freq);

int32_t interrupt_flag;
int32_t interrupt_counter;

#endif