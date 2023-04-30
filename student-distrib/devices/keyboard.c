#include "keyboard.h"
#include "keyboard_scancodes.h"
#include "terminal.h"
#include "../address.h"
#include "../lib.h"

/* 
 * keyboard_init
 *   DESCRIPTION: Initialize keyboard by enabling the interrupt line.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: enables interrupts for IRQ 1 (keyboard)
 */
void keyboard_init() {
    enable_irq(KEYBOARD_IRQ_NUM);
    // is_extended = 0;
    caps_lock_toggle = 0;
    caps_lock_active = 0;
    left_control_pressed = 0;
    right_control_pressed = 0;
    left_shift_pressed = 0;
    right_shift_pressed = 0;
    alt_pressed = 0;
}

// Pretend to be in the context of the displayed terminal
#define KEYBOARD_HANDLER_PROLOGUE(original_terminal_id)        \
    original_terminal_id = curr_executing_terminal_id;         \
    curr_executing_terminal_id = curr_displaying_terminal_id;  \
    video_mem = (char*) (VIDEO_PERM_MEM_ADDR);                 \

// Restore the original executing terminal context
#define KEYBOARD_HANDLER_EPILOGUE(original_terminal_id)  \
    send_eoi(KEYBOARD_IRQ_NUM);                          \
    curr_executing_terminal_id = original_terminal_id;   \
    video_mem = (char*) VIDEO_MEM;

/* 
 * keyboard_handler
 *   DESCRIPTION: Handle a keyboard interrupt (data available).
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: reads available data and acknowledges interrupt
 */
void keyboard_handler() {
    cli();

    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    // If the scancode is 0xE0, then the next byte is an extended scancode
    if (scancode == CODE_EXTENDED) {
        // is_extended = 1;
        send_eoi(KEYBOARD_IRQ_NUM);
        sti();
        return;
    }
    // if (is_extended) { // we ignore extended for now
    //     is_extended = 0;
    //     send_eoi(KEYBOARD_IRQ_NUM);
    //     sti();
    //     return;
    // }

    // Keyboard should only ever be active on the currently shown terminal
    // Don't context switch, just fake it
    uint8_t original_executing_terminal_id;
    KEYBOARD_HANDLER_PROLOGUE(original_executing_terminal_id);
    terminal_data_t* curr_terminal = &terminals[curr_displaying_terminal_id];

    if (scancode >= RELEASED_SCANCODE_OFFSET) { // released
        scancode -= RELEASED_SCANCODE_OFFSET;
        if (scancode >= NUM_SCANCODES) {
            KEYBOARD_HANDLER_EPILOGUE(original_executing_terminal_id);
            sti();
            return;
        }
        switch (scancode) {
            case CODE_LEFT_SHIFT:
                left_shift_pressed = 0;
                break;
            case CODE_RIGHT_SHIFT:
                right_shift_pressed = 0;
                break;
            case CODE_LEFT_CONTROL:
                left_control_pressed = 0;
                break;
            case CODE_CAPS_LOCK:
                caps_lock_toggle = 0;
                break;
            case CODE_ALT:
                alt_pressed = 0;
                break;
            default:
                break;
        }
    } else { // pressed
        switch (scancode) {
            case CODE_BACKSPACE:
                if (curr_terminal->keyboard_buffer_size > 0) {
                    putc('\b');
                    curr_terminal->keyboard_buffer_size--;
                }
                break;
            case CODE_TAB:
                if (curr_terminal->keyboard_buffer_size < KBUFFER_SIZE) {
                    curr_terminal->keyboard_buffer[curr_terminal->keyboard_buffer_size++] = '\t';
                    putc('\t');
                }
                break;
            case CODE_ENTER:
                if (curr_terminal->keyboard_buffer_size < KBUFFER_SIZE) {
                    curr_terminal->keyboard_buffer[curr_terminal->keyboard_buffer_size++] = '\n';
                    putc('\n');
                    curr_terminal->is_done_typing = 1;
                }
                break;
            case CODE_LEFT_CONTROL:
                left_control_pressed = 1;
                break;
            case CODE_LEFT_SHIFT:
                left_shift_pressed = 1;
                break;
            case CODE_RIGHT_SHIFT:
                right_shift_pressed = 1;
                break;
            case CODE_ALT:
                alt_pressed = 1;
                break;
            case CODE_CAPS_LOCK:
                if (!caps_lock_toggle) {
                    caps_lock_toggle = 1;
                    caps_lock_active = !caps_lock_active;
                }
                break;
            case CODE_F1:
            case CODE_F2:
            case CODE_F3:
                if (alt_pressed) {
                    // Don't forget to restore keyboard display terminal context
                    KEYBOARD_HANDLER_EPILOGUE(original_executing_terminal_id);
                    // We know the scancodes are consecutive!
                    term_video_switch(scancode - CODE_F1);
                    sti();
                    return;
                }
                break;
            default:
                if (scancode == CODE_L && (left_control_pressed || right_control_pressed)) {
                    term_reset();
                } else if (curr_terminal->keyboard_buffer_size < KBUFFER_SIZE) {
                    char c;
                    if (left_shift_pressed || right_shift_pressed) {
                        c = scancodeToKey[scancode][1];
                    } else if (caps_lock_active && islower(scancodeToKey[scancode][0])) {
                        c = scancodeToKey[scancode][1];
                    } else {
                        c = scancodeToKey[scancode][0];
                    }
                    curr_terminal->keyboard_buffer[curr_terminal->keyboard_buffer_size++] = c;
                    putc(c);
                }
                break;
        }
    }
    KEYBOARD_HANDLER_EPILOGUE(original_executing_terminal_id);
    sti();
}

void clear_kbuffer() {
    terminals[curr_executing_terminal_id].keyboard_buffer_size = 0;
}   
