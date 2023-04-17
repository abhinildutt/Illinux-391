#include "terminal.h"
#include "keyboard.h"
#include "../lib.h"

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

int32_t stdin_write_bad_call(fd_array_member_t *f, const void *buf, int32_t nbytes) {
    return -1;
}

int32_t stdout_read_bad_call(fd_array_member_t *f, void *buf, int32_t nbytes) {
    return -1;
}

/* Terminal Reset
    * Inputs: none
    * Return Value: none
    * Function: Clears the screen and resets the cursor to the top left corner */
void term_reset() {
    screen_x = screen_y = 0;
    clear();
    cursor_set(0, 0);
}

/* Terminal Init
    * Inputs: none
    * Return Value: none
    * Function: Initializes the terminal */
void term_init() {
    cursor_init();
    term_reset();
    return;
}

/* Terminal Open
    * Inputs: filename - the name of the file to open
    * Return Value: 0
    * Function: Opens the terminal */
int32_t term_open(fd_array_member_t* f, const uint8_t* filename) {
    kbuffer_size = 0;
    return 0;
}

/* Terminal Close
    * Inputs: none
    * Return Value: 0
    * Function: Closes the terminal */
int32_t term_close(fd_array_member_t* f) {
    return 0;
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
    for (i = 0; i < kbuffer_size; i++) {
        ((char *) buf)[i] = kbuffer[i];
        if (kbuffer[i] == '\n') {
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
    for (i = 0; i < nbytes; i++) {
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
