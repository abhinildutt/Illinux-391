#include "idt.h"
#include "../x86_desc.h"
#include "exceptions_def.h"
#include "exception.h"
#include "syscall.h"
#include "device_handlers.h"
#include "keyboard.h"
#include "rtc.h"

#define NUM_EXCEPTIONS 32
#define SYSCALL_NUM 0x80
#define PIC_BASE_NUM 0x20

/* 
 * setup_idt
 *   DESCRIPTION: Set up the IDT.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: modifies the IDT.
 */
void setup_idt() {
    // https://wiki.osdev.org/Interrupt_Descriptor_Table#Gate_Descriptor
    int i;
    for (i = 0; i < NUM_VEC; i++) {
        idt[i].present = 1;                   // using the interrupt
        idt[i].seg_selector = KERNEL_CS;      // kernel code segment
        
        idt[i].reserved4 = 0;                 // reserved

        // gate type
        idt[i].reserved3 = (i < NUM_EXCEPTIONS) ? 1 : 0;  // trap gate if it's an exception, interrupt gate otherwise
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;                      // 1 = 32 bit gate, 0 = 16 bit gate
        idt[i].reserved0 = 0;

        idt[i].dpl = (i == SYSCALL_NUM) ? 3 : 0;     // ring 0 for kernel calls, ring 3 for syscalls
    }

    // put exception handler on IDT table
    SET_IDT_ENTRY(idt[0x00],  DIVIDE_ZERO);
    SET_IDT_ENTRY(idt[0x01],  DEBUG_EXCEPTION);
    SET_IDT_ENTRY(idt[0x02],  NMI_INTERRUPT);
    SET_IDT_ENTRY(idt[0x03],  BREAKPOINT_EXCEPTION);
    SET_IDT_ENTRY(idt[0x04],  OVERFLOW_EXCEPTION);
    SET_IDT_ENTRY(idt[0x05],  BOUND_RANGE);
    SET_IDT_ENTRY(idt[0x06],  INVALID_OPCODE);
    SET_IDT_ENTRY(idt[0x07],  NOT_AVAILABLE);
    SET_IDT_ENTRY(idt[0x08],  DOUBLE_FAULT);
    SET_IDT_ENTRY(idt[0x09],  SEGMENT_OVERRUN);
    SET_IDT_ENTRY(idt[0x0A], INVALID_TSS);
    SET_IDT_ENTRY(idt[0x0B], NOT_PRESENT);
    SET_IDT_ENTRY(idt[0x0C], STACK_FAULT);
    SET_IDT_ENTRY(idt[0x0D], GENERAL_PROTECTION);
    SET_IDT_ENTRY(idt[0x0E], PAGE_FAULT);
    // idt[15] reserved by Intel
    SET_IDT_ENTRY(idt[0x10], MATH_FAULT);
    SET_IDT_ENTRY(idt[0x11], ALIGNMENT_CHECK);
    SET_IDT_ENTRY(idt[0x12], MACHINE_CHECK);
    SET_IDT_ENTRY(idt[0x13], FLOATING_POINT);
    SET_IDT_ENTRY(idt[0x14], VIRTUALIZATION);
    SET_IDT_ENTRY(idt[0x15], CONTROL_PROTECTION);
    // idt[22-27] reserved
    SET_IDT_ENTRY(idt[0x1C], HYPERVISOR_INJECTION);
    SET_IDT_ENTRY(idt[0x1D], VMM_COMMUNICATION);
    SET_IDT_ENTRY(idt[0x1E], SECURITY_EXCEPTION);
    // idt[31] is reserved
    
    // put hardware interrupt handlers in IDT
    SET_IDT_ENTRY(idt[PIC_BASE_NUM + KEYBOARD_IRQ_NUM], keyboard_interrupt);
    SET_IDT_ENTRY(idt[PIC_BASE_NUM + RTC_IRQ_NUM], rtc_interrupt);

    // put syscall handler in IDT
    SET_IDT_ENTRY(idt[SYSCALL_NUM], syscall_handler);
}
