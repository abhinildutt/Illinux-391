#include "syscalls_def.h"
#include "../lib.h"

#define MAX_NUM_FILES 8
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
    // printf("syscall %s \n", __FUNCTION__);

    int cmd_len = strlen(command);



    return 0;
}

int32_t read(int32_t fd, void* buf, int32_t nbytes) {
    printf("syscall %s\n", __FUNCTION__);

    if(fd >= 8 || fd < 0) return -1;

    funcptrs* curr_fops = fd_array[fd].fops_pointer;
    return curr_fops->read(fd, buf, nbytes);
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    printf("syscall %s\n", __FUNCTION__);

    if(fd >= 8 || fd < 0) return -1;

    funcptrs* curr_fops = fd_array[fd].fops_pointer;
    return curr_fops->write(fd, buf, nbytes);
}

int32_t open(const uint8_t* filename) {
    printf("syscall %s\n", __FUNCTION__);
    uint32_t fl;
    dentry_t syscall_dentry;

    if(filename == NULL) return -1;
    if(read_dentry_by_name(filename, &syscall_dentry) == -1) return -1;

    int i;
    for(i = 0; i < 8; i++) {
        if(fd_array[i].flags == 0) {
            fd_array[i].file_pos = 0;
            fd_array[i].flags = 1;
            int type = syscall_dentry.filetype;
            if(type == 0) {
                fd_array[i].fops_pointer = &rtc_fop;
                fd_array[i].inode = 0;
            }
            if(type == 1) {
                fd_array[i].fops_pointer = &directory_fop;
                fd_array[i].inode = 0;
            }
            if(type == 2) {
                fd_array[i].fops_pointer = &regular_fop;
                fd_array[i].inode = syscall_dentry.inode_num; // this is something look at future.
            }
            break;
        }
    }
    return i;
}

int32_t close(int32_t fd) {
    printf("syscall %s\n", __FUNCTION__);

    if(fd >= 8 || fd < 0) return -1;
    if(fd_array[fd].flags == 0) return -1;

    fd_array[fd].flags = 0;
    fd_array[fd].file_pos = 0;
    fd_array[fd].inode = 0;

    funcptrs* fp = fd_array[fd].fops_pointer;
    
    return fp->close(fd);
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
