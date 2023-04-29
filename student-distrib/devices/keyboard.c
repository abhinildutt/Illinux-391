#include "keyboard.h"
#include "keyboard_scancodes.h"
#include "terminal.h"

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
    is_extended = 0;
    caps_lock_toggle = 0;
    caps_lock_active = 0;
    left_control_pressed = 0;
    right_control_pressed = 0;
    left_shift_pressed = 0;
    right_shift_pressed = 0;
    alt_pressed = 0;
    done_typing = 0;
}

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
    if (scancode == CODE_EXTENDED) {
        is_extended = 1;
        send_eoi(KEYBOARD_IRQ_NUM);
        sti();
        return;
    }
    if (is_extended) { // we ignore extended for now
        is_extended = 0;
        send_eoi(KEYBOARD_IRQ_NUM);
        sti();
        return;
    }
    if (scancode >= RELEASED_SCANCODE_OFFSET) { // released
        scancode -= RELEASED_SCANCODE_OFFSET;
        if (scancode >= NUM_SCANCODES) {
            send_eoi(KEYBOARD_IRQ_NUM);
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
                if (terminals[curr_terminal_id].keyboard_buffer_size > 0) {
                    putc('\b');
                    terminals[curr_terminal_id].keyboard_buffer_size--;
                }
                break;
            case CODE_TAB:
                if (terminals[curr_terminal_id].keyboard_buffer_size < KBUFFER_SIZE) {
                    terminals[curr_terminal_id].keyboard_buffer[terminals[curr_terminal_id].keyboard_buffer_size++] = '\t';
                    putc('\t');
                }
                break;
            case CODE_ENTER:
                if (terminals[curr_terminal_id].keyboard_buffer_size < KBUFFER_SIZE) {
                    terminals[curr_terminal_id].keyboard_buffer[terminals[curr_terminal_id].keyboard_buffer_size++] = '\n';
                    putc('\n');
                    done_typing = 1;
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
                if (alt_pressed) {
                    switch_terminal(0);
                }
                break;
            case CODE_F2:
                if (alt_pressed) {
                    switch_terminal(1);
                }
                break;
            case CODE_F3:
                if (alt_pressed) {
                    switch_terminal(2);
                }
                break;
            default:
                if (scancode == CODE_L && (left_control_pressed || right_control_pressed)) {
                    term_reset();
                } else if (terminals[curr_terminal_id].keyboard_buffer_size < KBUFFER_SIZE) {
                    char c;
                    if (left_shift_pressed || right_shift_pressed) {
                        c = scancodeToKey[scancode][1];
                    } else if (caps_lock_active && islower(scancodeToKey[scancode][0])) {
                        c = scancodeToKey[scancode][1];
                    } else {
                        c = scancodeToKey[scancode][0];
                    }
                    terminals[curr_terminal_id].keyboard_buffer[terminals[curr_terminal_id].keyboard_buffer_size++] = c;
                    putc(c);
                }
                break;
        }
    }
    send_eoi(KEYBOARD_IRQ_NUM);
    sti();
}

void clear_kbuffer() {
    terminals[curr_terminal_id].keyboard_buffer_size = 0;
}   
