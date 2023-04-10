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

    // PARSING COMMAND

    int cmd_len = strlen(command);

    uint8_t file_name[1000];
    int file_name_length = 0;

    uint8_t file_arg[1000];
    int file_arg_length = 0;

    int i = 0;
    while(i < cmd_len) {
        if(command[i] == ' ' && file_name_length > 0) {
            break;
        }
        file_name[file_name_length] = command[i];
        file_name_length++;
        i++;
    }   

    while(i < cmd_len) {
        if(command[i] == ' ' && file_arg_length == 0) {
            continue;
        }
        file_arg[file_arg_length] = command[i];
        file_arg_length++;
        i++;
    }

    // FILE CHECKS

    dentry_t syscall_dentry;
    uint8_t file_data_top4B[4];

    if(file_name == NULL || read_dentry_by_name(file_name, &syscall_dentry) == -1) return -1; // file exists or not
    if(read_data(syscall_dentry.inode_num, 0, file_data_top4B, sizeof(int32_t)) == -1) return -1; // file reading errors
    if(file_data_top4B[0] != 0x7f || file_data_top4B[1] != 0x45 || file_data_top4B[2] != 0x4c || file_data_top4B[3] != 0x46) return -1; // file is not exe

    // CREATE NEW PCB

    // SETUP MEMORY



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

void fd_array_init() {
    rtc_fop.read = rtc_read;
    rtc_fop.write = rtc_write;
    rtc_fop.open = rtc_open;
    rtc_fop.close = rtc_close;

    directory_fop.read = dir_read;
    directory_fop.write = dir_write;
    directory_fop.open = dir_open;
    directory_fop.close = dir_close;

    regular_fop.read = file_read;
    regular_fop.write = file_write;
    regular_fop.open = file_open;
    regular_fop.close = file_close;

    stdin_fop.read = term_read;
    stdin_fop.write = NULL;
    stdin_fop.open = term_open;
    stdin_fop.close = term_close;

    stdout_fop.read = NULL;
    stdout_fop.write = term_write;
    stdout_fop.open = term_open;
    stdout_fop.close = term_close;


    int i;
    for(i = 0; i < 8; i++) {
        fd_array[i].file_pos = 0;
        fd_array[i].flags = 0;
        fd_array[i].fops_pointer = NULL;
        fd_array[i].inode = 0;
    }
    fd_array[0].fops_pointer = &stdin_fop;
    fd_array[0].flags = 1;

    fd_array[1].fops_pointer = &stdout_fop;
    fd_array[1].flags = 1;
}