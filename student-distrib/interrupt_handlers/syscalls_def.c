#include "syscalls_def.h"
#include "../lib.h"

/* 
 * halt
 *   DESCRIPTION: Halt the system and return the status to the parent process.
 *   INPUTS: status -- exit status
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t halt(uint8_t status) {
    printf("syscall %s (%d)\n", __FUNCTION__, status);
    asm volatile (".1: hlt; jmp .1;");
    return 0;
}

int32_t execute(const uint8_t* command) {
    printf("syscall %s\n", __FUNCTION__);
    return 0;
}

int32_t read(int32_t fd, void* buf, int32_t nbytes) {
    printf("syscall %s\n", __FUNCTION__);
    return 0;
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    printf("syscall %s\n", __FUNCTION__);
    return 0;
}

int32_t open(const uint8_t* filename) {
    printf("syscall %s\n", __FUNCTION__);
    return 0;
}

int32_t close(int32_t fd) {
    printf("syscall %s\n", __FUNCTION__);
    return 0;
}

int32_t getargs(uint8_t* buf, int32_t nbytes) {
    printf("syscall %s\n", __FUNCTION__);
    return 0;
}

int32_t vidmap(uint8_t** screen_start) {
    printf("syscall %s\n", __FUNCTION__);
    return 0;
}

int32_t set_handler(int32_t signum, void* handler_address) {
    printf("syscall %s\n", __FUNCTION__);
    return 0;
}

int32_t sigreturn(void) {
    printf("syscall %s\n", __FUNCTION__);
    return 0;
}
