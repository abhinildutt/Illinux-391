#include "rtc.h"
#include "../lib.h"
#include "../i8259.h"

#define bit6 0x40
#define MAX_RTC_FREQ 1024
#define RESET_FREQ 2
#define THEORETICAL_MAX_FREQ 32768

funcptrs rtc_fops = {
    .open = rtc_open,
    .close = rtc_close,
    .read = rtc_read,
    .write = rtc_write
};

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
    interrupt_flag = 0;
    // interrupt_counter = 0;
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
    
    // test_interrupts();
    // Print 1 character for every interrupt
    // test_interrupts_cp2(interrupt_counter);
    // interrupt_counter++;

    outb(disable_NMI_C, RTC_PORT);
    inb(RTC_DATA); // drop unneeded data
    send_eoi(RTC_IRQ_NUM);
}

/* 
 * rtc_open
 *   DESCRIPTION: Initialize RTC frequency to 2HZ.
 *   INPUTS: filename -- filename to open
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t rtc_open(fd_array_member_t* f, const uint8_t* filename) {
    set_rtc_freq(RESET_FREQ);
    return 0;
}

/* 
 * rtc_close
 *   DESCRIPTION: Close RTC.
 *   INPUTS: fd -- file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t rtc_close(fd_array_member_t* f) {
    // Does nothing since we don't virtualize RTC, return 0
    return 0;
}


int32_t rtc_read(fd_array_member_t* f, void* buf, int32_t nbytes) {
    // Block until the next interrupt
    // Wait until the interrupt handler clears interrupt_flag, then return 0
    while(!interrupt_flag);
    interrupt_flag = 0;
    return 0;
}

/* 
 * rtc_write
 *   DESCRIPTION: Set RTC frequency.
 *   INPUTS: fd -- file descriptor
 *           buf -- buffer to read from
 *           nbytes -- number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t rtc_write(fd_array_member_t* f, const void* buf, int32_t nbytes) {
    if(buf == NULL) return -1;
    // The system call should always accept only a 4-byte integer specifying the interrupt rate in Hz,
    // and should set the rate of periodic interrupts accordingly
    if(nbytes != 4) return -1;
    int32_t freq = *((int32_t*)buf);
    return set_rtc_freq(freq);
}

/* 
 * set_rtc_freq
 *   DESCRIPTION: Set RTC frequency.
 *   INPUTS: freq -- frequency to set
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t set_rtc_freq(int32_t freq) {
    int32_t rate = freq_to_rate(freq);
    if(rate == -1) return -1;
    cli();
    outb(disable_NMI_A, RTC_PORT);
    char prev = inb(RTC_DATA);
    outb(disable_NMI_A, RTC_PORT);
    outb((prev & 0xF0) | rate, RTC_DATA);
    sti();
    return 0;
}

/* 
 * freq_to_rate
 *   DESCRIPTION: Converts a frequency to a rate for the RTC.
 *   INPUTS: freq -- frequency to convert
 *   OUTPUTS: none
 *   RETURN VALUE: rate for RTC
 *   SIDE EFFECTS: none
 */
int32_t freq_to_rate(int32_t freq) {
    // Frequency must be power of 2
    if (freq & (freq - 1)) return -1;

    // RTC can only generate interrupts at frequency up to 8192 Hz.
    // Kernel should limit this further to 1024 Hz. Min freq is 2 Hz.
    if (freq > MAX_RTC_FREQ || freq < RESET_FREQ) return -1;

    // frequency =  32768 >> (rate-1);
    int32_t rate = 1;
    while((THEORETICAL_MAX_FREQ >> (rate - 1)) != freq) {
        rate++;
    }

    return rate;
}
