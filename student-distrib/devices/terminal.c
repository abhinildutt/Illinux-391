#include "terminal.h"
#include "keyboard.h"
#include "../lib.h"
#include "../address.h"

funcptrs stdin_fops = {
    .open = term_open,
    .close = term_close,
    .read = term_read,
    .write = stdin_write_bad_call
};

funcptrs stdout_fops = {
    .open = term_open,
    .close = term_close,
    .read = stdout_read_bad_call,
    .write = term_write
};

uint8_t curr_terminal_id = 0;
terminal_data_t terminals[MAX_TERMINAL_ID];

/* Terminal Write Bad Call
    * Inputs: f - the file descriptor
    *         buf - the buffer to write from
    *         nbytes - the number of bytes to write
    * Return Value: -1
    * Function: Returns -1, since can't write to stdin */
int32_t stdin_write_bad_call(fd_array_member_t *f, const void *buf, int32_t nbytes) {
    return -1;
}

/* Terminal Read Bad Call
    * Inputs: f - the file descriptor
    *         buf - the buffer to read into
    *         nbytes - the number of bytes to read
    * Return Value: -1
    * Function: Returns -1, since can't read from stdout */
int32_t stdout_read_bad_call(fd_array_member_t *f, void *buf, int32_t nbytes) {
    return -1;
}

/* Terminal Reset
    * Inputs: none
    * Return Value: none
    * Function: Clears the screen and resets the cursor to the top left corner */
void term_reset() {
    clear();
    terminals[curr_terminal_id].cursor_x = 0;
    terminals[curr_terminal_id].cursor_y = 0;
    cursor_set(0, 0);
}

/* Terminal Init
    * Inputs: none
    * Return Value: none
    * Function: Initializes the terminal */
void term_init() {
    int i;
    for (i = 0; i < MAX_TERMINAL_ID; i++) {
        terminals[i].screen_x = 0;
        terminals[i].screen_y = 0;
        terminals[i].cursor_y = 0;
        terminals[i].cursor_y = 0;
        terminals[i].keyboard_buffer_size = 0;
        terminals[i].vidmem = VIDEO_MEM + i * PAGE_SIZE_4KB;
        terminals[i].active_pid = -1;
    }
    cursor_init();
    term_reset();
    return;
}

/* Terminal Open
    * Inputs: filename - the name of the file to open
    * Return Value: 0
    * Function: Opens the terminal */
int32_t term_open(fd_array_member_t* f, const uint8_t* filename) {
    terminals[curr_terminal_id].keyboard_buffer_size = 0;
    return 0;
}

/* Terminal Close
    * Inputs: none
    * Return Value: -1
    * Function: Doesn't do anything, since can't close terminal */
int32_t term_close(fd_array_member_t* f) {
    return -1;
}

/* Terminal Read
    * Inputs: buf - the buffer to read into
    *         nbytes - the number of bytes to read
    * Return Value: the number of bytes read
    * Function: Reads from the terminal */
int32_t term_read(fd_array_member_t* f, void* buf, int32_t nbytes) {
    if (buf == NULL) return -1;

    sti();
    while (done_typing == 0) {
        asm volatile("hlt");
    }
    cli();

    int i;
    for (i = 0; i < terminals[curr_terminal_id].keyboard_buffer_size; i++) {
        ((char *) buf)[i] = terminals[curr_terminal_id].keyboard_buffer[i];
        if (terminals[curr_terminal_id].keyboard_buffer[i] == '\n') {
            break;
        }
    }
    clear_kbuffer();
    done_typing = 0;
    sti();
    return i + 1;
}

/* Terminal Write
    * Inputs: buf - the buffer to write from
    *         nbytes - the number of bytes to write
    * Return Value: the number of bytes written
    * Function: Writes to the terminal */
int32_t term_write(fd_array_member_t* f, const void* buf, int32_t nbytes) {
    if (buf == NULL) return -1;
    int i;
    // char c;
    for (i = 0; i < nbytes; i++) {
        // if ((c = ((char *) buf)[i]) == '\0') {
        //     return i;
        // }
        putc(((char *) buf)[i]);
    }
    return nbytes;
}

/* Cursor Init
    * Inputs: none
    * Return Value: none
    * Function: Initializes the cursor */
void cursor_init() {
    outb(CURSOR_START, VGA_INDEX_PORT);
    char data = inb(VGA_DATA_PORT);
    outb(CURSOR_START, VGA_INDEX_PORT);
    // 0b 1000 1111 = 0xDF
    outb(data & 0xDF, VGA_DATA_PORT);
}

/* Cursor Set
    * Inputs: x - the x coordinate of the cursor
    *         y - the y coordinate of the cursor
    * Return Value: none
    * Function: Sets the cursor to the given coordinates */
void cursor_set(uint32_t x, uint32_t y) {
    uint32_t pos = y * SCREEN_WIDTH + x;
    outb(CURSOR_LOCATION_HIGH, VGA_INDEX_PORT);
    outb(pos >> 8, VGA_DATA_PORT); // high 8 bits
    outb(CURSOR_LOCATION_LOW, VGA_INDEX_PORT);
    outb(pos, VGA_DATA_PORT);
}

void switch_terminal(uint8_t terminal_id) {
    if (curr_terminal_id == terminal_id) return;
    if (terminal_id > MAX_TERMINAL_ID) return;

    memcpy((void*) terminals[curr_terminal_id].vidmem, (const void*) VIDEO_MEM, PAGE_SIZE_4KB);
    memcpy((void*) VIDEO_MEM, (const void*) terminals[terminal_id].vidmem, PAGE_SIZE_4KB);

    curr_terminal_id = terminal_id;
    cursor_set(terminals[curr_terminal_id].cursor_x, terminals[curr_terminal_id].cursor_y);
}
