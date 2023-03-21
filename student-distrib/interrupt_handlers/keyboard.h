#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_IRQ_NUM 1

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_CONTROL_PORT 0x64

void keyboard_init();
void keyboard_open();
void keyboard_close();
void keyboard_read();
void keyboard_write();
void keyboard_handler();

#endif
