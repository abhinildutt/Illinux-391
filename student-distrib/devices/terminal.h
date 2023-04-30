#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "../lib.h"
#include "../types.h"
#include "../filesystem/filesys_interface.h"
#include "../devices/keyboard.h"

#define SCREEN_WIDTH (320 / 4)
#define SCREEN_HEIGHT (200 / 4)

#define VGA_INDEX_PORT 0x3D4
#define VGA_DATA_PORT 0x3D5

// http://www.osdever.net/FreeVGA/vga/crtcreg.htm
#define CURSOR_START 0x0A
#define CURSOR_LOCATION_HIGH 0x0E
#define CURSOR_LOCATION_LOW 0x0F
#define MAX_TERMINAL_ID 3

typedef struct terminal_data {
    char keyboard_buffer[KBUFFER_SIZE];
    uint32_t keyboard_buffer_size;
    uint8_t is_done_typing;

    uint32_t screen_x;
    uint32_t screen_y;

    int32_t curr_pid;

    int32_t rtc_enabled;
    int32_t rtc_freq;
    int32_t rtc_counter;
    int32_t rtc_flag;

    
} terminal_data_t;

extern uint8_t curr_executing_terminal_id;
extern uint8_t curr_displaying_terminal_id;
extern terminal_data_t terminals[MAX_TERMINAL_ID];

extern funcptrs stdin_fops;
extern funcptrs stdout_fops;

extern void term_reset();
void term_init();
extern int32_t term_open(fd_array_member_t* f, const uint8_t* filename);
extern int32_t term_close(fd_array_member_t* f);
extern int32_t term_read(fd_array_member_t* f, void* buf, int32_t nbytes);
extern int32_t term_write(fd_array_member_t* f, const void* buf, int32_t nbytes);
extern int32_t stdin_write_bad_call(fd_array_member_t *f, const void *buf, int32_t nbytes);
extern int32_t stdout_read_bad_call(fd_array_member_t *f, void *buf, int32_t nbytes);

extern void cursor_init();
extern void cursor_set(uint32_t x, uint32_t y);

int get_current_terminal_id();
void term_video_switch(uint8_t terminal_id);
void term_context_switch(uint8_t terminal_id);

#endif
