#include "idt.h"
#include "../x86_desc.h"
#include "exceptions_def.h"
#include "exception.h"
#include "syscall.h"

void setup_idt() {
    // https://wiki.osdev.org/Interrupt_Descriptor_Table#Gate_Descriptor
    int i;
    for (i = 0; i < NUM_VEC; i++) {
        idt[i].present = 1;                   // using the interrupt
        idt[i].seg_selector = KERNEL_CS;      // kernel code segment
        
        idt[i].reserved4 = 0;                 // reserved

        // gate type
        idt[i].reserved3 = (i < 32) ? 1 : 0;  // trap gate if it's an exception, interrupt gate otherwise
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;                      // 1 = 32 bit gate, 0 = 16 bit gate
        idt[i].reserved0 = 0;

        idt[i].dpl = (i == 0x80) ? 3 : 0;     // ring 0 for kernel calls, ring 3 for syscalls
    }

    // put exception handler on IDT table
    SET_IDT_ENTRY(idt[0x00], DIVIDE_ZERO);
    SET_IDT_ENTRY(idt[0x01], DEBUG_EXCEPTION);
    SET_IDT_ENTRY(idt[0x02], NMI_INTERRUPT);
    SET_IDT_ENTRY(idt[0x03], BREAKPOINT_EXCEPTION);
    SET_IDT_ENTRY(idt[0x04], OVERFLOW_EXCEPTION);
    SET_IDT_ENTRY(idt[0x05], BOUND_RANGE);
    SET_IDT_ENTRY(idt[0x06], INVALID_OPCODE);
    SET_IDT_ENTRY(idt[0x07], NOT_AVAILABLE);
    SET_IDT_ENTRY(idt[0x08], DOUBLE_FAULT);
    SET_IDT_ENTRY(idt[0x09], SEGMENT_OVERRUN);
    SET_IDT_ENTRY(idt[0x0A], INVALID_TSS);
    SET_IDT_ENTRY(idt[0x0B], NOT_PRESENT);
    SET_IDT_ENTRY(idt[0x0C], STACK_FAULT);
    SET_IDT_ENTRY(idt[0x0D], GENERAL_PROTECTION);
    SET_IDT_ENTRY(idt[0x0E], PAGE_FAULT);
    // idt[15] is used by Intel
    SET_IDT_ENTRY(idt[0x0F], MATH_FAULT);
    SET_IDT_ENTRY(idt[0x10], ALIGNMENT_CHECK);
    SET_IDT_ENTRY(idt[0x11], MACHINE_CHECK);
    SET_IDT_ENTRY(idt[0x12], FLOATING_POINT);

    // put syscall handler in IDT
    SET_IDT_ENTRY(idt[0x80], syscall_handler);
}
