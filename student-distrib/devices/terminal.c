#include "terminal.h"
#include "keyboard.h"
#include "../lib.h"
#include "../address.h"
#include "../task.h"
#include "../paging.h"
#include "../interrupt_handlers/syscalls_def.h"
#include "../x86_desc.h"

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

uint8_t curr_executing_terminal_id = 0;
uint8_t curr_displaying_terminal_id = 0;
terminal_data_t terminals[MAX_TERMINAL_ID];

/*
 * stdin_write_bad_call
 *   DESCRIPTION: No-op function for invalid stdin write
 *   INPUTS: f -- file descriptor struct
 *           buf -- buffer to write to
 &           nbytes -- the number of bytes to write
 *   OUTPUTS: none
 *   RETURN VALUE: -1 since can't write to stdin
 *   SIDE EFFECTS: none
 */
int32_t stdin_write_bad_call(fd_array_member_t *f, const void *buf, int32_t nbytes) {
    return -1;
}

/*
 * stdout_read_bad_call
 *   DESCRIPTION: No-op function for invalid stdout read
 *   INPUTS: f -- file descriptor struct
 *           buf -- buffer to write to
 &           nbytes -- the number of bytes to write
 *   OUTPUTS: none
 *   RETURN VALUE: -1 since can't read from stdout
 *   SIDE EFFECTS: none
 */
int32_t stdout_read_bad_call(fd_array_member_t *f, void *buf, int32_t nbytes) {
    return -1;
}

/*
 * term_reset
 *   DESCRIPTION: Clears the screen and resets the cursor to the top left corner
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: clears screen & sets cursor to (0, 0)
 */
void term_reset() {
    clear();
    cursor_set(0, 0);
}

/*
 * term_init
 *   DESCRIPTION: Initializes the terminal.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: initializes the cursor & resets the terminal
 */
void term_init() {
    uint32_t backing_video_page;
    int32_t i, j;
    for (i = 0; i < MAX_TERMINAL_ID; i++) {
        terminals[i].screen_x = 0;
        terminals[i].screen_y = 0;
        terminals[i].is_done_typing = 0;
        terminals[i].keyboard_buffer_size = 0;
        terminals[i].curr_pid = -1;

        terminals[i].rtc_enabled = 0;
        terminals[i].rtc_freq = 0;
        terminals[i].rtc_counter = 0;
        terminals[i].rtc_flag = 0;

        backing_video_page = VIDEO_MEM_BACKGROUND_START_ADDR + i * PAGE_SIZE_4KB;
        for (j = 0; j < NUM_ROWS * NUM_COLS; j++) {
            *(uint8_t *)(backing_video_page + (j << 1)) = ' ';
            *(uint8_t *)(backing_video_page + (j << 1) + 1) = ATTRIB;
        }
    }
    cursor_init();
    term_reset();
    return;
}

/*
 * term_open
 *   DESCRIPTION: Opens the terminal.
 *   INPUTS: f -- file descriptor struct
 *           filename -- the name of the file to open (unused)
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: resets the terminal's keyboard buffer
 */
int32_t term_open(fd_array_member_t* f, const uint8_t* filename) {
    terminals[curr_executing_terminal_id].keyboard_buffer_size = 0;
    return 0;
}

/*
 * term_close
 *   DESCRIPTION: Does nothing.
 *   INPUTS: f -- file descriptor struct
 *           filename -- the name of the file to open (unused)
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   SIDE EFFECTS: resets the terminal's keyboard buffer
 */
int32_t term_close(fd_array_member_t* f) {
    return -1;
}

/*
 * term_read
 *   DESCRIPTION: Reads from the terminal input buffer.
 *   INPUTS: buf -- the buffer to read into
 *           nbytes -- the number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success, -1 for failrure
 *   SIDE EFFECTS: resets the terminal's keyboard buffer once done
 */
int32_t term_read(fd_array_member_t* f, void* buf, int32_t nbytes) {
    if (buf == NULL) return -1;

    // Wait until user is done typing
    while (terminals[curr_executing_terminal_id].is_done_typing == 0) {
        asm volatile("hlt");
    }

    cli();
    int i;
    for (i = 0; i < terminals[curr_executing_terminal_id].keyboard_buffer_size; i++) {
        ((char *) buf)[i] = terminals[curr_executing_terminal_id].keyboard_buffer[i];
        if (terminals[curr_executing_terminal_id].keyboard_buffer[i] == '\n') {
            break;
        }
    }
    clear_kbuffer();
    terminals[curr_executing_terminal_id].is_done_typing = 0;
    sti();
    return i + 1;
}

/*
 * term_write
 *   DESCRIPTION: Writes to the terminal.
 *   INPUTS: buf -- the buffer to write from
 *           nbytes -- the number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success, -1 for failrure
 *   SIDE EFFECTS: prints buffer to the screen
 */
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

/*
 * cursor_init
 *   DESCRIPTION: Initializes the cursor.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: initializes the cursor.
 */
void cursor_init() {
    outb(CURSOR_START, VGA_INDEX_PORT);
    char data = inb(VGA_DATA_PORT);
    outb(CURSOR_START, VGA_INDEX_PORT);
    // 0b 1000 1111 = 0xDF
    outb(data & 0xDF, VGA_DATA_PORT);
}

/*
 * cursor_set
 *   DESCRIPTION: Sets the cursor to the given coordinates.
 *   INPUTS: x -- the x coordinate of the cursor
 *           y -- the y coordinate of the cursor
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets the cursor position.
 */
void cursor_set(uint32_t x, uint32_t y) {
    uint32_t pos = y * SCREEN_WIDTH + x;
    outb(CURSOR_LOCATION_HIGH, VGA_INDEX_PORT);
    outb(pos >> 8, VGA_DATA_PORT); // high 8 bits
    outb(CURSOR_LOCATION_LOW, VGA_INDEX_PORT);
    outb(pos, VGA_DATA_PORT);
}

/*
 * term_video_switch
 *   DESCRIPTION: Switches the video memory to the given terminal.
 *   INPUTS: terminal_id -- the id of the terminal to switch to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets the current display memory & remaps program paging.
 */
void term_video_switch(uint8_t terminal_id) {
    if (curr_displaying_terminal_id == terminal_id) return;
    if (terminal_id >= MAX_TERMINAL_ID) return;
    
    // Copy current video memory to the terminal background video memory
    memcpy((void*) (VIDEO_MEM_BACKGROUND_START_ADDR + curr_displaying_terminal_id * PAGE_SIZE_4KB), 
        (const void*) VIDEO_PERM_MEM_ADDR, PAGE_SIZE_4KB);
    curr_displaying_terminal_id = terminal_id;

    // Previously background task is now being displayed, update paging accordingly
    map_program(curr_pid, curr_pcb->is_vidmapped, curr_pcb->terminal_id, curr_pcb->terminal_id == curr_displaying_terminal_id);

    // Copy target terminal background video memory to video memory
    memcpy((void*) VIDEO_PERM_MEM_ADDR, 
        (const void*) (VIDEO_MEM_BACKGROUND_START_ADDR + curr_displaying_terminal_id * PAGE_SIZE_4KB), PAGE_SIZE_4KB);


    cursor_set(terminals[curr_displaying_terminal_id].screen_x, terminals[curr_displaying_terminal_id].screen_y);
}

/*
 * term_context_switch
 *   DESCRIPTION: Switches to the task context of the given terminal.
 *   INPUTS: terminal_id -- the id of the terminal to switch to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets the current display memory & remaps program paging.
 */
void term_context_switch(uint8_t terminal_id) {
    if (curr_executing_terminal_id == terminal_id) return;
    if (terminal_id >= MAX_TERMINAL_ID) return;

    curr_pcb = get_pcb(curr_pid);
    if (curr_pcb != NULL) {
        // Save stack registers for current task
        uint32_t saved_esp, saved_ebp;

        asm volatile (
            " movl %%esp, %0 \n\
            movl %%ebp, %1"
            : "=r"(saved_esp), "=r"(saved_ebp)
            :
            : "memory"
        );
        curr_pcb->esp = saved_esp;
        curr_pcb->ebp = saved_ebp;
    }

    // Set the new terminal & its task as the current
    curr_executing_terminal_id = terminal_id;
    curr_pid = terminals[curr_executing_terminal_id].curr_pid;
    // printf("now executing tid=%d pid=%d\n", curr_executing_terminal_id, curr_pid);
    if (curr_pid != -1) { // switch to already running task
        // basically copied from halt, but we aren't returning to a parent
        tss.ss0 = KERNEL_DS;
        // 8MB (bottom of 4MB kernel page) - 8KB (size of kernel stack) - 4B (to get to top of stack)
        tss.esp0 = KERNEL_STACK_ADDR - USER_KERNEL_STACK_SIZE * curr_pid - 0x4;
        curr_pcb = get_pcb(curr_pid);
        map_program(curr_pid, curr_pcb->is_vidmapped, curr_pcb->terminal_id, curr_pcb->terminal_id == curr_displaying_terminal_id);
        
        // Restore stack pointers (no status code this time)
        asm volatile ("       \n \
            movl %0, %%esp \n \
            movl %1, %%ebp \n \
            "
            :
            : "r" (curr_pcb->esp), "r" (curr_pcb->ebp)
            : "esp", "ebp"
        );
    } else { // create new shell task
        // printf("launch\n");
        curr_pcb = NULL;
        execute((const uint8_t*) "shell");
    }
}
