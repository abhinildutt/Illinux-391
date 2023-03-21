#define ASM 1

.text

.globl DIVIDE_ZERO
.globl DEBUG_EXCEPTION
.globl NMI_INTERRUPT
.globl BREAKPOINT_EXCEPTION
.globl BOUND_RANGE
.globl OVERFLOW_EXCEPTION
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

DIVIDE_ZERO:
    PUSHAL
    PUSH $0xFFFFFFFF
    CALL exception_handler
    JMP DONE
DEBUG_EXCEPTION:
    PUSHAL
    PUSH $0xFFFFFFFE
    CALL exception_handler
    JMP DONE
NMI_INTERRUPT:
    PUSHAL
    PUSH $0xFFFFFFFD
    CALL exception_handler
    JMP DONE
BREAKPOINT_EXCEPTION:
    PUSHAL
    PUSH $0xFFFFFFFC
    CALL exception_handler
    JMP DONE
BOUND_RANGE:
    PUSHAL
    PUSH $0xFFFFFFFB
    CALL exception_handler
    JMP DONE
OVERFLOW_EXCEPTION:
    PUSHAL
    PUSH $0xFFFFFFFA
    CALL exception_handler
    JMP DONE
INVALID_OPCODE:
    PUSHAL
    PUSH $0xFFFFFFF9
    CALL exception_handler
    JMP DONE
NOT_AVAILABLE:
    PUSHAL
    PUSH $0xFFFFFFF8
    CALL exception_handler
    JMP DONE
DOUBLE_FAULT:
    PUSHAL
    PUSH $0xFFFFFFF7
    CALL exception_handler
    JMP DONE
SEGMENT_OVERRUN:
    PUSHAL
    PUSH $0xFFFFFFF6
    CALL exception_handler
    JMP DONE
INVALID_TSS:
    PUSHAL
    PUSH $0xFFFFFFF5
    CALL exception_handler
    JMP DONE
NOT_PRESENT:
    PUSHAL
    PUSH $0xFFFFFFF4
    CALL exception_handler
    JMP DONE
STACK_FAULT:
    PUSHAL
    PUSH $0xFFFFFFF3
    CALL exception_handler
    JMP DONE
GENERAL_PROTECTION:
    PUSHAL
    PUSH $0xFFFFFFF2
    CALL exception_handler
    JMP DONE
PAGE_FAULT:
    PUSHAL
    PUSH $0xFFFFFFF1
    CALL exception_handler
    JMP DONE
MATH_FAULT:
    PUSHAL
    PUSH $0xFFFFFFEF
    CALL exception_handler
    JMP DONE
ALIGNMENT_CHECK:
    PUSHAL
    PUSH $0xFFFFFFEE
    CALL exception_handler
    JMP DONE
MACHINE_CHECK:
    PUSHAL
    PUSH $0xFFFFFFED
    CALL exception_handler
    JMP DONE
FLOATING_POINT:
    PUSHAL
    PUSH $0xFFFFFFEC
    CALL exception_handler
    JMP DONE
DONE :  
    ADD $4, %esp
    POPAL
    IRET
