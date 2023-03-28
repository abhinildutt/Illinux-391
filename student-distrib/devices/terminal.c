#include "terminal.h"
#include "keyboard.h"
#include "../lib.h"

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
int32_t term_open(const uint8_t* filename) {
    term_reset();
    return 0;
}

/* Terminal Close
    * Inputs: fd - the file descriptor of the terminal to close
    * Return Value: 0
    * Function: Closes the terminal */
int32_t term_close(int32_t fd) {
    term_reset();
    return 0;
}

/* Terminal Read
    * Inputs: fd - the file descriptor of the terminal to read from
    *         buf - the buffer to read into
    *         nbytes - the number of bytes to read
    * Return Value: the number of bytes read
    * Function: Reads from the terminal */
int32_t term_read(int32_t fd, void* buf, int32_t nbytes) {
    if (buf == NULL) return -1;

    sti();
    while (done_typing == 0);
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
    * Inputs: fd - the file descriptor of the terminal to write to
    *         buf - the buffer to write from
    *         nbytes - the number of bytes to write
    * Return Value: the number of bytes written
    * Function: Writes to the terminal */
int32_t term_write(int32_t fd, void* buf, int32_t nbytes) {
    if (buf == NULL) return -1;
    int i;
    for (i = 0; i < nbytes; i++) {
        putc(((char *) buf)[i]);
    }
    return nbytes;
}

/* Terminal Putc
    * Inputs: c - the character to print
    * Return Value: none
    * Function: Prints a character to the screen */
void cursor_init() {
    outb(CURSOR_START, VGA_INDEX_PORT);
    char data = inb(VGA_DATA_PORT);
    outb(CURSOR_START, VGA_INDEX_PORT);
    // 0b 1000 1111 = 0xDF
    outb(data & 0xDF, VGA_DATA_PORT);
}

/* Terminal Putc
    * Inputs: c - the character to print
    * Return Value: none
    * Function: Prints a character to the screen */
void cursor_set(uint32_t x, uint32_t y) {
    uint32_t pos = y * SCREEN_WIDTH + x;
    outb(CURSOR_LOCATION_HIGH, VGA_INDEX_PORT);
    outb(pos >> 8, VGA_DATA_PORT); // high 8 bits
    outb(CURSOR_LOCATION_LOW, VGA_INDEX_PORT);
    outb(pos, VGA_DATA_PORT);
}