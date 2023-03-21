#define ASM     1

.text

.globl  loadPageDirectory 

/* loadPageDirectory takes one parameter: the address of the page directory.
It then loads the address onto the CR3 register, where the MMU will find it. */
loadPageDirectory:
    push %ebp
    mov %esp, %ebp
    mov 8(%esp), %eax
    mov %eax, %cr3
    mov %ebp, %esp
    pop %ebp
    ret

.globl  enablePaging

/* Enable paging by creating 4MB paging using the cr4 register and then 
enable paging in protected mode using the cr0 register. */
enablePaging:
    push %ebp
    mov %esp, %ebp

    mov %cr4, %eax
    or  $0x00000010, %eax
    mov %eax, %cr4

    mov %cr0, %eax
    or $0x80000001, %eax
    mov %eax, %cr0

    mov %ebp, %esp
    pop %ebp
    ret
