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

/* 
 * execute
 *   DESCRIPTION:  attempts to load and execute a new program, handing off the
 * processor to the new program until it terminates
 *   INPUTS: command -- command to execute
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful, -1 if not successful
 *   SIDE EFFECTS: none */
int32_t execute(const uint8_t* command) {
    // printf("syscall %s \n", __FUNCTION__);

    // PARSING COMMAND

    uint32_t cmd_len = strlen((int8_t*)command);

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
    // Checks if top 4 bytes are 0x7f, 0x45, 0x4c, 0x46 which means ELF
    if(file_data_top4B[0] != 0x7f || file_data_top4B[1] != 0x45 || file_data_top4B[2] != 0x4c || file_data_top4B[3] != 0x46) return -1; // file is not exe

    // CREATE NEW PCB

    // SETUP MEMORY



    return 0;
}

/* 
 * read
 *   DESCRIPTION: reads data from the keyboard, a file,
 * device (RTC), or directory
 *   INPUTS: fd -- file descriptor
 *           buf -- buffer to read data into
 *           nbytes -- number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes read
 *   SIDE EFFECTS: none
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes) {
    printf("syscall %s\n", __FUNCTION__);

    if(fd >= MAX_NUM_FILES || fd < 0) return -1;

    funcptrs* curr_fops = curr_pcb->fd_array[fd].fops_pointer;
    return curr_fops->read(fd, buf, nbytes);
}

/* 
 * write
 *   DESCRIPTION: writes data to the terminal or device (RTC)
 *   INPUTS: fd -- file descriptor
 *           buf -- buffer to write data from
 *           nbytes -- number of bytes to write
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes written
 *   SIDE EFFECTS: none */
int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    printf("syscall %s\n", __FUNCTION__);

    if(fd >= MAX_NUM_FILES || fd < 0) return -1;

    funcptrs* curr_fops = curr_pcb->fd_array[fd].fops_pointer;
    return curr_fops->write(fd, buf, nbytes);
}

/* 
 * open
 *   DESCRIPTION: provides access to the file system. Find the directory entry corresponding to the
named file, allocates an unused file descriptor, and sets up any data necessary to
handle the given type of file (directory, RTC device, or regular file)
 *   INPUTS: filename -- name of file to open
 *   OUTPUTS: none
 *   RETURN VALUE: file descriptor
 *   SIDE EFFECTS: none */
int32_t open(const uint8_t* filename) {
    printf("syscall %s\n", __FUNCTION__);
    dentry_t syscall_dentry;

    if(filename == NULL) return -1;
    if(read_dentry_by_name(filename, &syscall_dentry) == -1) return -1;

    int i;
    // Find the file in the file system and assign an unused file descriptor
    for (i = 0; i < MAX_NUM_FILES; i++) {
        if(curr_pcb->fd_array[i].flags == 0) {
            curr_pcb->fd_array[i].file_pos = 0;
            curr_pcb->fd_array[i].flags = 1;
            int type = syscall_dentry.filetype;
            // File descriptors need to be set up according to the file type
            if(type == 0) {
                curr_pcb->fd_array[i].fops_pointer = &rtc_fop;
                curr_pcb->fd_array[i].inode = 0;
            }
            if(type == 1) {
                curr_pcb->fd_array[i].fops_pointer = &directory_fop;
                curr_pcb->fd_array[i].inode = 0;
            }
            if(type == 2) {
                curr_pcb->fd_array[i].fops_pointer = &regular_fop;
                curr_pcb->fd_array[i].inode = syscall_dentry.inode_num; // this is something look at future.
            }
            break;
        }
    }

    funcptrs* fp = curr_pcb->fd_array[i].fops_pointer;
    if (fp->open(filename) == -1) return -1;

    return i;
}

/* 
 * close
 *   DESCRIPTION: closes the specified file descriptor and
 * makes it available for return from later calls to open
 *   INPUTS: fd -- file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: none */
int32_t close(int32_t fd) {
    printf("syscall %s\n", __FUNCTION__);

    if(fd >= MAX_NUM_FILES || fd < 0) return -1;
    // Check for invalid fd
    if(curr_pcb->fd_array[fd].flags == 0) return -1;

    curr_pcb->fd_array[fd].flags = 0;
    curr_pcb->fd_array[fd].file_pos = 0;
    curr_pcb->fd_array[fd].inode = 0;

    funcptrs* fp = curr_pcb->fd_array[fd].fops_pointer;
    
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

/* 
 * get_curr_pcb
 *   DESCRIPTION: gets the current pcb
 *   INPUTS: curr_pid -- current pid
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to current pcb
 *   SIDE EFFECTS: none */
pcb_t* get_curr_pcb(uint32_t curr_pid){
    curr_pcb = (pcb_t *)(EIGHT_MB - (curr_pid + 1) * EIGHT_KB);
    return curr_pcb;
}

/* 
 * fd_array_init
 *   DESCRIPTION: initializes the file descriptor array
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none */
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

    // When a process is started, open stdin and stdout, which correspond to file descriptors 0 and 1, respectively.
    int i;
    for(i = 0; i < MAX_NUM_FILES; i++) {
        curr_pcb->fd_array[i].file_pos = 0;
        curr_pcb->fd_array[i].flags = 0;
        curr_pcb->fd_array[i].fops_pointer = NULL;
        curr_pcb->fd_array[i].inode = 0;
    }
    curr_pcb->fd_array[0].fops_pointer = &stdin_fop;
    curr_pcb->fd_array[0].flags = 1;

    curr_pcb->fd_array[1].fops_pointer = &stdout_fop;
    curr_pcb->fd_array[1].flags = 1;
}
