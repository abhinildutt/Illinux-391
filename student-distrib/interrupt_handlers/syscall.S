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

syscall_handler:
    # syscall 0 doesn't exist
    CMPL $0, %EAX
    JE syscall_handler_failed

    # syscall 10 is the max
    CMPL $10, %EAX
    JG syscall_handler_failed

    # Save all general registers
    # https://c9x.me/x86/html/file_module_x86_id_270.html
    PUSHL %EDI
    PUSHL %ESI
    PUSHL %EBP
    PUSHFL

    # Call corresponding syscall
    PUSHL %EDX
    PUSHL %ECX
    PUSHL %EBX
    CALL *syscall_table(, %EAX, 4)

    POPL %EBX
    POPL %ECX
    POPL %EDX
    POPFL
    POPL %EBP
    POPL %ESI
    POPL %EDI
    IRET

syscall_handler_failed:
    MOVL $-1, %EAX
    IRET
