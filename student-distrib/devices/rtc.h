#ifndef _RTC_H
#define _RTC_H

#include "../lib.h"
#include "../filesystem/filesys_interface.h"

#define RTC_IRQ_NUM 8

#define RTC_PORT 0x70
#define RTC_DATA 0x71

#define REG_A		0x0A
#define REG_B		0x0B
#define REG_C		0x0C

#define disable_NMI_A   0x8A
#define disable_NMI_B   0x8B
#define disable_NMI_C   0x8C
#define RTC_FREQ       1024

extern funcptrs rtc_fops;

void rtc_init();
int32_t rtc_open(fd_array_member_t* f, const uint8_t* filename);
int32_t rtc_close(fd_array_member_t* f);
int32_t rtc_read(fd_array_member_t* f, void* buf, int32_t nbytes);
int32_t rtc_write(fd_array_member_t* f, const void* buf, int32_t nbytes);
void rtc_handler();

int32_t set_rtc_freq(int32_t freq);
int32_t freq_to_rate(int32_t freq);

// int32_t interrupt_flag;
// int32_t interrupt_counter;

#endif
