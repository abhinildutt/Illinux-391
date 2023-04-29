#ifndef _PIT_H
#define _PIT_H

#define PIT_IRQ_NUM 0

#define PIT_PORT 0x43
#define PIT_DATA 0x40

// this is our desired frequency in Hz
#define PIT_DESIRED_FREQ 100
#define PIT_BASE_FREQ 1193180
#define PIT_DIVISOR (PIT_BASE_FREQ / PIT_DESIRED_FREQ)

// http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs%20and%20the%20PIT.html
// https://wiki.osdev.org/Programmable_Interval_Timer#I.2FO_Ports
// 0b110110
// Channel 0, lobyte/hibyte, square wave generator, binary mode
#define PIT_MODE 0x36

void pit_init();
void pit_handler();

#endif
