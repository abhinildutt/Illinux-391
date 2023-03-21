/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    master_mask = 0xFF;
    slave_mask  = 0xFF;

    outb(ICW1,MASTER_8259_PORT);
    outb(ICW2_MASTER,MASTER_8259_DATA);
    outb(ICW3_MASTER,MASTER_8259_DATA);
    outb(ICW4,MASTER_8259_DATA);

    outb(ICW1,SLAVE_8259_PORT);
    outb(ICW2_SLAVE,SLAVE_8259_DATA);
    outb(ICW3_SLAVE,SLAVE_8259_DATA);
    outb(ICW4,SLAVE_8259_DATA);

    outb(master_mask, MASTER_8259_DATA);
    outb(slave_mask, SLAVE_8259_DATA);

    return;
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint8_t value = master_mask;
    uint16_t port = MASTER_8259_DATA; 
    if(irq_num >= 8) {
        value = slave_mask;
        port = SLAVE_8259_DATA;
    }
    value = value & ~(1 << irq_num);
    outb(value, port);
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint8_t value = master_mask;
    uint16_t port = MASTER_8259_DATA; 
    if(irq_num >= 8) {
        value = slave_mask;
        port = SLAVE_8259_DATA;
    }
    value = value | (1 << irq_num);
    outb(value, port);
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if(irq_num >= 8) {
        // printf("Secondary PIC called");
        outb(EOI | (irq_num - 8), SLAVE_8259_PORT);
        outb(EOI | 0x2, MASTER_8259_PORT);
    }
    else {
        outb(EOI | irq_num, MASTER_8259_PORT);
    }
    return;
}
