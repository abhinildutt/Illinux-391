#define ASM 1

.text

/* 
 * DEFINE_TRAMPOLINE
 *   DESCRIPTION: Define a interrupt handler that wraps a C device interrupt handler.
 *   INPUTS: interrupt_handler_name -- name of the new ASM interrupt handler
 *           handler_name -- name of the C device interrupt handler
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
# Inspired by ece391syscall.S
#define DEFINE_TRAMPOLINE(interrupt_handler_name, handler_name)   \
.GLOBL interrupt_handler_name  ;\
interrupt_handler_name:        ;\
    PUSHAL                     ;\
    PUSHFL                     ;\
    CALL handler_name          ;\
    POPFL                      ;\
    POPAL                      ;\
    IRET

DEFINE_TRAMPOLINE(rtc_interrupt, rtc_handler);
DEFINE_TRAMPOLINE(keyboard_interrupt, keyboard_handler);
