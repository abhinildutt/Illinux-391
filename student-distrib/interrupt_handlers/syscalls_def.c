#include "syscalls_def.h"
#include "../lib.h"
#include "../x86_desc.h"
#include "../task.h"
#include "../filesystem/filesys.h"
#include "../devices/rtc.h"
#include "../devices/keyboard.h"
#include "../devices/terminal.h"
#include "../paging.h"

/* 
 * halt
 *   DESCRIPTION: Halt the system and return the status to the parent process.
 *   INPUTS: status -- exit status
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t halt(uint8_t status) {
    if (curr_pid == -1) return -1;
    curr_pcb = get_pcb(curr_pid);
    if (curr_pcb == NULL) return -1;

    // Close all task file descriptors
    int i;
    for (i = 0; i < MAX_FILE_COUNT; i++) {
        fs_interface_close(&curr_pcb->fd_array[i]);
    }
    // Disable current task
    curr_pcb->active = 0;
    if (curr_pcb->parent_pid != -1) { // parent exists, return to parent
        map_program(curr_pcb->parent_pid);
        tss.ss0 = KERNEL_DS;
        tss.esp0 = KERNEL_STACK_ADDR - USER_KERNEL_STACK_SIZE * curr_pcb->parent_pid - 0x4;
        // Switch back to parent's PID
        curr_pid = curr_pcb->parent_pid;
        curr_pcb = get_pcb(curr_pid);
        
        // Restore stack pointers & put status code in eax
        asm volatile ("       \n \
            movl %%ebx, %%esp \n \
            movl %%ecx, %%ebp \n \
            movl %%edx, %%eax \n \
            leave             \n \
            ret               \n \
            "
            :
            : "b" (curr_pcb->esp), "c" (curr_pcb->ebp), "d" (status)
            : "eax", "ebp", "esp"
        );
        // asm volatile (".2: hlt; jmp .2;");
    } else { // parent doesn't exist, restart shell
        curr_pid = -1;
        curr_pcb = NULL;
        // printf("restart shell\n");
        execute((const uint8_t*) "shell");
    }
    return 0;
}

/* 
 * execute
 *   DESCRIPTION:  attempts to load and execute a new program, handing off the
 * processor to the new program until it terminates
 *   INPUTS: command -- command to execute
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful, -1 if not successful
 *   SIDE EFFECTS: none 
 */
int32_t execute(const uint8_t* command) {
    // printf("syscall %s (command=%s)\n", __FUNCTION__, command);

    // Validate command
    if (command == NULL) return -1;
    if (command[0] == '\0') return -1;

    cli();

    // Parse command
    uint32_t cmd_len = strlen((int8_t*) command);

    uint8_t file_name[FILE_NAME_LEN];
    int file_name_length = 0;

    uint8_t file_arg[FILE_NAME_LEN];
    int file_arg_length = 0;

    int i = 0;
    while (i < cmd_len) {
        if (command[i] == ' ' && file_name_length > 0) {
            break;
        }
        if (command[i] != ' ') {
            if (file_name_length >= FILE_NAME_LEN) {
                sti();
                return -1;
            }
            file_name[file_name_length] = command[i];
            file_name_length++;
            i++;
        }
    }
    if (file_name_length >= FILE_NAME_LEN) {
        sti();
        return -1;
    }
    file_name[file_name_length] = '\0';

    while (i < cmd_len) {
        if(command[i] == ' ' && file_arg_length == 0) {
            continue;
        }
        if (file_arg_length >= FILE_NAME_LEN) {
            sti();
            return -1;
        }
        file_arg[file_arg_length] = command[i];
        file_arg_length++;
        i++;
    }
    if (file_arg_length >= FILE_NAME_LEN) {
        sti();
        return -1;
    }
    file_arg[file_arg_length] = '\0';
    // printf("parsed cmd (filename=%s, len=%d)\n", file_name, file_name_length);

    // File header checks
    dentry_t syscall_dentry;
    uint8_t file_data_top4B[4];
    uint32_t prog_eip;

    // printf("reading file %s...\n", file_name);
    // file exists or not
    if (read_dentry_by_name(file_name, &syscall_dentry) == -1) {
        sti();
        return -1;
    }
    // printf("file exists\n");
    // file reading errors
    if (read_data(syscall_dentry.inode_num, 0, file_data_top4B, sizeof(int32_t)) == -1) {
        sti();
        return -1;
    }
    // printf("file read properly\n");
    //The first 4 bytes of the file represent a “magic number” that identifies the file as an executable. These
    // bytes are, respectively, 0: 0x7f; 1: 0x45; 2: 0x4c; 3: 0x46.
    if (file_data_top4B[0] != 0x7f || file_data_top4B[1] != 0x45 || file_data_top4B[2] != 0x4c || file_data_top4B[3] != 0x46) {
        sti();
        return -1; // file is not exe
    }
    // printf("file magic correct\n");

    // Get new PID
    int32_t new_pid = get_new_pid(); 
    if (new_pid == -1) {
        printf("NO AVAILABLE PID's\n");
        sti();
        return -1;
    }

    uint8_t entry_buf[4];
    // Getting the eip
    read_data(syscall_dentry.inode_num, PROGRAM_ENTRY_POINT, entry_buf, 4);
    prog_eip = *((uint32_t*) entry_buf);

    // Setup paging
    map_program(new_pid);

    // Load the executable
    // printf("length = %#x\n", inode_ptr[syscall_dentry.inode_num].length);
    read_data(syscall_dentry.inode_num, 0, (uint8_t*) PROGRAM_IMAGE_VIRTUAL_ADDR, inode_ptr[syscall_dentry.inode_num].length);

    // printf("loaded to %#x\nread %d bytes\n", PROGRAM_IMAGE_VIRTUAL_ADDR, br);
    // printf("header = %#x\n", *((uint32_t*) (PROGRAM_IMAGE_VIRTUAL_ADDR)));

    // Init new FS array
    pcb_t* pcb = get_pcb(new_pid);
    fs_interface_init(pcb->fd_array);

    // Setup PCB struct
    pcb->pid = new_pid;
    pcb->parent_pid = curr_pid;

    // Subtract 4 because when, aligning the stack, esp is four byte aligned and to actually point to the stack,
    // we have to delete 4 bytes (esp points beyond the stack)
    pcb->ebp = USER_STACK_VIRTUAL_ADDR + PAGE_SIZE_4MB - 4;
    pcb->esp = USER_STACK_VIRTUAL_ADDR + PAGE_SIZE_4MB - 4;

    // printf("finished setting pcb\n");

    if (curr_pcb != NULL) {
        // Save ebp and esp values
        int saved_esp;
        int saved_ebp;

        asm volatile (
            " movl %%esp, %0 \n\
            movl %%ebp, %1"
            : "=r"(saved_esp), "=r"(saved_ebp)
            :
            : "memory"
        );
        curr_pcb->ebp = saved_ebp;
        curr_pcb->esp = saved_esp;
    }

    // printf("saved esp & ebp (if parent exists)\n");

    // Task switching
    pcb->eip = prog_eip;
    tss.ss0 = KERNEL_DS;
    tss.esp0 = KERNEL_STACK_ADDR - USER_KERNEL_STACK_SIZE * new_pid - 0x4;

    // Switch to create task
    curr_pid = new_pid;
    curr_pcb = pcb;

    sti();

    // printf("switch time\n");
    // PUSH before IRET for context switching
    // $0 = USER_DS, $1 = USER_ESP, $2 = USER_CS, $3 = prog_eip
    
    // OSDev wiki and hardware context switch DIAGRAM in mp3 appendix
    asm volatile(" \
        movw %%ax, %%ds    ;\
        pushl %%eax        ;\
        movl %%ebx, %%eax  ;\
        pushl %%eax        ;\
        pushfl             ;\
        pushl %%ecx        ;\
        pushl %%edx        ;\
        iret               "
        :
        : "a"(USER_DS), "b"(pcb->esp) , "c"(USER_CS), "d"(pcb->eip)
        : "memory"
    );
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
    // printf("syscall %s %d %d\n", __FUNCTION__, fd, nbytes);
    if (fd >= MAX_FILE_COUNT || fd < 0) return -1;
    if (buf == NULL) return -1;
    if (nbytes < 0) return -1;
    curr_pcb = get_pcb(curr_pid);
    return fs_interface_read(&curr_pcb->fd_array[fd], buf, nbytes);
}

/* 
 * write
 *   DESCRIPTION: writes data to the terminal or device (RTC)
 *   INPUTS: fd -- file descriptor
 *           buf -- buffer to write data from
 *           nbytes -- number of bytes to write
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes written
 *   SIDE EFFECTS: none
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    // printf("syscall %s\n", __FUNCTION__);

    if (fd >= MAX_FILE_COUNT || fd < 0) return -1;
    if (buf == NULL) return -1;
    if (nbytes < 0) return -1;
    curr_pcb = get_pcb(curr_pid);
    return fs_interface_write(&curr_pcb->fd_array[fd], buf, nbytes);
}

/* 
 * open
 *   DESCRIPTION: provides access to the file system. Find the directory entry corresponding to the
 *                named file, allocates an unused file descriptor, and sets up any data necessary to
 *                handle the given type of file (directory, RTC device, or regular file).
 *   INPUTS: filename -- name of file to open
 *   OUTPUTS: none
 *   RETURN VALUE: file descriptor
 *   SIDE EFFECTS: none 
 */
int32_t open(const uint8_t* filename) {
    // printf("syscall %s\n", __FUNCTION__);
    if (filename == NULL) return -1;

    curr_pcb = get_pcb(curr_pid);
    if (curr_pcb == NULL) return -1;

    dentry_t syscall_dentry;
    fd_array_member_t* f;
    int fd;
    for (fd = 0; fd < MAX_FILE_COUNT; fd++) {
        f = &curr_pcb->fd_array[fd];
        if (f->flags == 0) {
            // Check if file exists
            if (read_dentry_by_name(filename, &syscall_dentry) == -1) return -1;

            int type = syscall_dentry.filetype;
            printf("Opening file of type %d...\n", type);
            switch (type) {
                case FILE_TYPE_RTC:
                    f->fops = &rtc_fops;
                    f->inode = 0;
                    break;
                case FILE_TYPE_DIR:
                    f->fops = &directory_fops;
                    f->inode = 0;
                    break;
                case FILE_TYPE_FILE:
                    f->fops = &regular_fops;
                    f->inode = syscall_dentry.inode_num;
                    break;
            }

            // Failed to open file
            if (fs_interface_open(f, filename) == -1) return -1;
            f->file_pos = 0;
            f->flags = 1;
            // printf("returning fd %d, opened %s\n", fd, filename);
            return fd;
        }
    }
    return -1;
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
    // printf("syscall %s\n", __FUNCTION__);
    if (fd >= MAX_FILE_COUNT || fd < 0) return -1;
    curr_pcb = get_pcb(curr_pid);
    if (curr_pcb == NULL) {
        return -1;
    }
    return fs_interface_close(&curr_pcb->fd_array[fd]);
}

int32_t getargs(uint8_t* buf, int32_t nbytes) {
    // printf("syscall %s\n", __FUNCTION__);
    return 0;
}

int32_t vidmap(uint8_t** screen_start) {
    // printf("syscall %s\n", __FUNCTION__);
    return 0;
}

int32_t set_handler(int32_t signum, void* handler_address) {
    // printf("syscall %s\n", __FUNCTION__);
    return 0;
}

int32_t sigreturn(void) {
    // printf("syscall %s\n", __FUNCTION__);
    return 0;
}
