#include "rtc.h"
#include "../lib.h"

#define bit6 0x40

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
    test_interrupts();
    outb(disable_NMI_C, RTC_PORT);
    inb(RTC_DATA); // drop unneeded data
    send_eoi(RTC_IRQ_NUM);
}
