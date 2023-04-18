#include "../task.h"
#include "../devices/rtc.h"
#include "filesys.h"
#include "../devices/terminal.h"

/* 
* fs_interface_init
*   DESCRIPTION: Initializes the file system interface by setting up the file descriptor array and setting up stdin and stdout
*   INPUTS: fd_array: the file descriptor array
*   OUTPUTS: none
*   RETURN VALUE: 0 on success, -1 on failure
*   SIDE EFFECTS: none
*/
int32_t fs_interface_init(fd_array_member_t* fd_array) {
    int i;
    for (i = 0; i < MAX_FILE_COUNT; i++) {
        fd_array[i].file_pos = 0;
        fd_array[i].flags = 0;
        fd_array[i].fops = NULL;
        fd_array[i].inode = 0;
    }
    fd_array[0].flags = 1;
    fd_array[0].fops = &stdin_fops;
    fd_array[1].flags = 1;
    fd_array[1].fops = &stdout_fops;
    return 0;
}

/* 
* fs_interface_read
*   DESCRIPTION: Reads from the keyboard, a file, device (RTC), or directory
*   INPUTS: f: the file descriptor array member
*           buf: the buffer to be read into
*           nbytes: the number of bytes to be read
*   OUTPUTS: none
*   RETURN VALUE: number of bytes read if successful, -1 if not successful
*   SIDE EFFECTS: fills the buffer
*/
int32_t fs_interface_read(fd_array_member_t* f, void* buf, int32_t nbytes) {
    if (buf == NULL || f->fops == NULL || f->fops->read == NULL) return -1;
    return f->fops->read(f, buf, nbytes);
}

/* 
* fs_interface_write
*   DESCRIPTION: Writes to the terminal, a file, or device (RTC)
*   INPUTS: f: the file descriptor array member
*           buf: the buffer to be written into
*           nbytes: the number of bytes to be written
*   OUTPUTS: none
*   RETURN VALUE: number of bytes written if successful, -1 if not successful
*   SIDE EFFECTS: none
*/
int32_t fs_interface_write(fd_array_member_t* f, const void* buf, int32_t nbytes) {
    if (buf == NULL || f->fops == NULL || f->fops->write == NULL) return -1;
    return f->fops->write(f, buf, nbytes);
}

/*
* fs_interface_open
*   DESCRIPTION: Opens a file, directory, or device (RTC)
*   INPUTS: f: the file descriptor array member
*           filename: the name of the file to be opened
*   OUTPUTS: none
*   RETURN VALUE: 0 on success, -1 on failure
*   SIDE EFFECTS: none
*/
int32_t fs_interface_open(fd_array_member_t* f, const uint8_t* filename) {
    if (f->fops == NULL) return -1;
    if (f->fops->open == NULL) return -1;
    return f->fops->open(f, filename);
}

/*
* fs_interface_close
*   DESCRIPTION: Closes a file, directory, or device (RTC)
*   INPUTS: f: the file descriptor array member
*   OUTPUTS: none
*   RETURN VALUE: 0 on success, -1 on failure
*   SIDE EFFECTS: none
*/
int32_t fs_interface_close(fd_array_member_t* f) {
    if (f->fops == NULL) return -1;
    if (f->fops->close == NULL) return -1;
    if (f->flags == 0) return -1;

    f->flags = 0;
    f->file_pos = 0;
    f->inode = 0;
    return f->fops->close(f);
}
