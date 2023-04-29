#define ASM 1

.data

syscall_table:
    .long   0
    .long   halt
    .long   execute
    .long   read
    .long   write
    .long   open
    .long   close
    .long   getargs
    .long   vidmap
    .long   set_handler
    .long   sigreturn

.text

.globl syscall_handler

/* 
 * syscall_handler
 *   DESCRIPTION: Handle a syscall interrupt and dispatch to appropriate syscall.
 *   INPUTS: %EAX -- syscall number
 *           %EBX -- argument 1 for syscall
 *           %ECX -- argument 2 for syscall
 *           %EDX -- argument 3 for syscall
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: performs syscall corresponding to the given syscall number
 */
syscall_handler:
    # syscall 0 doesn't exist
    CMP $1, %EAX
    JL syscall_handler_failed

    # syscall 10 is the max
    CMP $10, %EAX
    JG syscall_handler_failed

    # Save all general registers
    # https://c9x.me/x86/html/file_module_x86_id_270.html
    PUSHL %EDI
    PUSHL %ESI
    PUSHL %EBP

    # Call corresponding syscall
    PUSHL %EDX
    PUSHL %ECX
    PUSHL %EBX
    CALL *syscall_table(, %EAX, 4)

    # Restore registers
    POPL %EBX
    POPL %ECX
    POPL %EDX
    POPL %EBP
    POPL %ESI
    POPL %EDI
    IRET

syscall_handler_failed:
    MOVL $-1, %EAX
    IRET
