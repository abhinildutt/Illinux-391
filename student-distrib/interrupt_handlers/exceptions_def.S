#define ASM 1

.text

.globl DIVIDE_ZERO
.globl DEBUG_EXCEPTION
.globl NMI_INTERRUPT
.globl BREAKPOINT_EXCEPTION
.globl OVERFLOW_EXCEPTION
.globl BOUND_RANGE
.globl INVALID_OPCODE
.globl NOT_AVAILABLE
.globl DOUBLE_FAULT
.globl SEGMENT_OVERRUN
.globl INVALID_TSS
.globl NOT_PRESENT
.globl STACK_FAULT
.globl GENERAL_PROTECTION
.globl PAGE_FAULT
.globl MATH_FAULT
.globl ALIGNMENT_CHECK
.globl MACHINE_CHECK
.globl FLOATING_POINT
.globl VIRTUALIZATION
.globl CONTROL_PROTECTION
.globl HYPERVISOR_INJECTION
.globl VMM_COMMUNICATION
.globl SECURITY_EXCEPTION

/* Ex. DIVIDE_ZERO pushes the error code -1 onto the stack and calls the exception handler
Negative values are passed to the exception handler because positive values are used for interrupts
Error code is used to index into the IDT to execute the correct handler */
DIVIDE_ZERO:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFFF
    CALL exception_handler
    JMP DONE
DEBUG_EXCEPTION:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFFE
    CALL exception_handler
    JMP DONE
NMI_INTERRUPT:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFFD
    CALL exception_handler
    JMP DONE
BREAKPOINT_EXCEPTION:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFFC
    CALL exception_handler
    JMP DONE
OVERFLOW_EXCEPTION:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFFB
    CALL exception_handler
    JMP DONE
BOUND_RANGE:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFFA
    CALL exception_handler
    JMP DONE

INVALID_OPCODE:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFF9
    CALL exception_handler
    JMP DONE
NOT_AVAILABLE:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFF8
    CALL exception_handler
    JMP DONE
DOUBLE_FAULT:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFF7
    CALL exception_handler
    JMP DONE
SEGMENT_OVERRUN:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFF6
    CALL exception_handler
    JMP DONE
INVALID_TSS:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFF5
    CALL exception_handler
    JMP DONE
NOT_PRESENT:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFF4
    CALL exception_handler
    JMP DONE
STACK_FAULT:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFF3
    CALL exception_handler
    JMP DONE
GENERAL_PROTECTION:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFF2
    CALL exception_handler
    JMP DONE
PAGE_FAULT:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFF1
    CALL exception_handler
    JMP DONE
MATH_FAULT:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFEF
    CALL exception_handler
    JMP DONE
ALIGNMENT_CHECK:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFEE
    CALL exception_handler
    JMP DONE
MACHINE_CHECK:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFED
    CALL exception_handler
    JMP DONE
FLOATING_POINT:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFEC
    CALL exception_handler
    JMP DONE
VIRTUALIZATION:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFEB
    CALL exception_handler
    JMP DONE
CONTROL_PROTECTION: 
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFEA
    CALL exception_handler
    JMP DONE
HYPERVISOR_INJECTION:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFE9
    CALL exception_handler
    JMP DONE
VMM_COMMUNICATION:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFE8
    CALL exception_handler
    JMP DONE
SECURITY_EXCEPTION:
    PUSHAL
    PUSHFL
    PUSH $0xFFFFFFE7
    CALL exception_handler
    JMP DONE
DONE :  
    ADD $4, %esp
    POPFL
    POPAL
    IRET
