#include "rtc.h"
#include "../lib.h"

#define bit6 0x40

int interrupt_flag = 0;

/* 
 * rtc_init
 *   DESCRIPTION: Initialize RTC by turning on IRQ with a default 1024 Hz rate.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: enables interrupts for IRQ 8 (rtc)
 */
void rtc_init() {
    // Adapted from https://wiki.osdev.org/RTC#Turning_on_IRQ_8
    cli();
    outb(disable_NMI_B, RTC_PORT);   // select register B, and disable NMI
    char prev = inb(RTC_DATA);   // read the current value of register B
    outb(disable_NMI_B, RTC_PORT);   // set the index again (a read will reset the index to register D)
    outb(prev | bit6, RTC_DATA);     // write the previous value ORed with 0x40. This turns on bit 6 of register B
    enable_irq(RTC_IRQ_NUM); 
    sti();
}

/* 
 * rtc_handler
 *   DESCRIPTION: Handle a periodic RTC interrupt.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: reads available data and acknowledges interrupt
 */
void rtc_handler() {
    interrupt_flag = 1;
    test_interrupts();
    outb(disable_NMI_C, RTC_PORT);
    inb(RTC_DATA); // drop unneeded data
    send_eoi(RTC_IRQ_NUM);
}


void rtc_open() {

}


void rtc_close() {

}


int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {
    if(interrupt_flag == 1) return 0;
    return -1;
}

int32_t rtc_write(int32_t fd, void* buf, int32_t nbytes) {
    if(buf == NULL) return -1;
    int32_t freq = (int32_t) buf;   
    int32_t rate = freq_to_rate(freq);
    if(freq == -1) return -1;
     
}


int32_t freq_to_rate(int32_t freq) {
    int32_t rate = 1;
    while((32768 >> rate) != freq) {
        rate++;
    }
    uint32_t hex[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

    if(rate > 15 || rate < 2) return -1;
    return hex[rate];
}