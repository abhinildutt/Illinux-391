#include "rtc.h"
#include "../lib.h"

#define bit6 0x40

void rtc_init() {
    outb(disable_NMI_B, RTC_PORT);
    char prev = inb(RTC_DATA);
    outb(disable_NMI_B, RTC_PORT);
    outb(prev | bit6, RTC_DATA);

    enable_irq(RTC_IRQ_NUM);
}

void rtc_handler() {
    test_interrupts();
}