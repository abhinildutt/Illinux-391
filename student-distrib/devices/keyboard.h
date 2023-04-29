#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "../lib.h"
#include "../i8259.h"

#define KEYBOARD_IRQ_NUM 1

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_CONTROL_PORT 0x64

#define KBUFFER_SIZE 128

uint8_t is_extended;
uint8_t caps_lock_toggle;
uint8_t caps_lock_active;
uint8_t left_control_pressed;
uint8_t right_control_pressed;
uint8_t left_shift_pressed;
uint8_t right_shift_pressed;
uint8_t alt_pressed;

extern void keyboard_init();
extern void keyboard_handler();

extern void clear_kbuffer();

#endif
