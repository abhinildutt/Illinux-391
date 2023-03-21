#include "exception.h"
#include "../lib.h"
#include "syscalls_def.h"

const char* exception_messages[] = {
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
    "SIMD Floating-Point Exception"
};

void exception_handler(int int_vector) {
    cli();

    printf("Exception: %s\n", exception_messages[-int_vector - 1]);
    printf("Killing program\n");
    
    halt(1); // infinite loop here, no interrupts can occur

    sti();
} 
