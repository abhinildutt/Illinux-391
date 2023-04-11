#include "exception.h"
#include "../lib.h"
#include "syscalls_def.h"

#define NUM_EXCEPTIONS 32

const char* exception_messages[NUM_EXCEPTIONS] = {
    "Division by zero",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};

/* 
 * exception_handler
 *   DESCRIPTION: Handle trap exceptions.
 *   INPUTS: int_vector -- negative exception number
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints exception and halts the currently running process
 */
void exception_handler(int int_vector) {
    cli();

    // Check garbage input
    if (int_vector >= 0 || int_vector < -NUM_EXCEPTIONS) {
        sti();
        return;
    }

    printf("Exception: %s\n", exception_messages[-int_vector - 1]);
    printf("Killing program\n");
    
    // infinite loop here, no interrupts can occur
    // asm volatile (".1 : hlt; jmp .1;");
    halt(1);

    sti();
} 
