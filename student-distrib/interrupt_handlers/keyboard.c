#include "keyboard.h"
#include "keyboard_scancodes.h"
#include "../lib.h"
#include "../i8259.h"

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
}

void keyboard_open() {

}

void keyboard_close() {

}

void keyboard_read() {

}

void keyboard_write() {

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
    uint8_t index = 0;
    if (scancode >= RELEASED_SCANCODE_OFFSET) { // released
        scancode -= RELEASED_SCANCODE_OFFSET;
        if (scancode >= NUM_SCANCODES) {
            send_eoi(KEYBOARD_IRQ_NUM);
            sti();
            return;
        }
    } else { // pressed
        printf("%c", scancodeToKey[scancode][index]);
    }
    send_eoi(KEYBOARD_IRQ_NUM);
    sti();
}
