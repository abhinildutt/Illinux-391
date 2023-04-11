#include "../task.h"
#include "../devices/rtc.h"
#include "filesys.h"
#include "../devices/terminal.h"

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

int32_t fs_interface_read(fd_array_member_t* f, void* buf, int32_t nbytes) {
    if (f->fops == NULL) return -1;
    if (f->fops->read == NULL) return -1;
    return f->fops->read(f, buf, nbytes);
}

int32_t fs_interface_write(fd_array_member_t* f, const void* buf, int32_t nbytes) {
    if (f->fops == NULL) return -1;
    if (f->fops->write == NULL) return -1;
    return f->fops->write(f, buf, nbytes);
}

int32_t fs_interface_open(fd_array_member_t* f, const uint8_t* filename) {
    if (f->fops == NULL) return -1;
    if (f->fops->open == NULL) return -1;
    return f->fops->open(f, filename);
}

int32_t fs_interface_close(fd_array_member_t* f) {
    if (f->fops == NULL) return -1;
    if (f->fops->close == NULL) return -1;
    if (f->flags == 0) return -1;

    f->flags = 0;
    f->file_pos = 0;
    f->inode = 0;
    return f->fops->close(f);
}
