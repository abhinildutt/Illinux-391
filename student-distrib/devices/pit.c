#include "pit.h"
#include "../lib.h"
#include "../i8259.h"
#include "../devices/terminal.h"

/* 
 * pit_init
 *   DESCRIPTION: Initialize PIT by turning on IRQ
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: enables interrupts for IRQ 0 (pit)
 */
void pit_init() {
    cli();
    // Set the desired PIT mode
    outb(PIT_MODE, PIT_PORT);

    // Send 16 bit divisor byte-wise
    uint8_t l = (uint8_t) (PIT_DIVISOR & 0xFF);
    uint8_t h = (uint8_t) ((PIT_DIVISOR >> 8) & 0xFF);

    // Send the frequency divisor
    outb(l, PIT_DATA);
    outb(h, PIT_DATA);

    // Finally, enable the PIT IRQ
    enable_irq(PIT_IRQ_NUM);

    sti();
}

/* 
 * pit_handler
 *   DESCRIPTION: Round robin scheduling for terminals
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sends EOI and switches terminals
 */
void pit_handler() {
    cli();
    // printf("PIT interrupt\n");
    send_eoi(PIT_IRQ_NUM);
    // printf("context switch to %d\n", curr_executing_terminal_id + 1);
    term_context_switch((curr_executing_terminal_id + 1) % MAX_TERMINAL_ID);
    sti();
}
