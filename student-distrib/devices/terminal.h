#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "../lib.h"
#include "../types.h"

#define SCREEN_WIDTH (320 / 4)
#define SCREEN_HEIGHT (200 / 4)

#define VGA_INDEX_PORT 0x3D4
#define VGA_DATA_PORT 0x3D5

// http://www.osdever.net/FreeVGA/vga/crtcreg.htm
#define CURSOR_START 0x0A
#define CURSOR_LOCATION_HIGH 0x0E
#define CURSOR_LOCATION_LOW 0x0F

uint32_t screen_x, screen_y;

extern void term_reset();
void term_init();
extern int32_t term_open(const uint8_t* filename);
extern int32_t term_close(int32_t fd);
extern int32_t term_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t term_write(int32_t fd, const void* buf, int32_t nbytes);

extern void cursor_init();
extern void cursor_set(uint32_t x, uint32_t y);

#endif
