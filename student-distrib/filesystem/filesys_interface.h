#ifndef FILESYS_INTERFACE_H
#define FILESYS_INTERFACE_H

#include "../types.h"

typedef struct fd_array_member_t fd_array_member_t;
typedef struct funcptrs funcptrs;

struct fd_array_member_t {
    funcptrs *fops;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags;
};

struct funcptrs {
  int32_t (*open)(fd_array_member_t* f, const uint8_t* filename);
  int32_t (*close)(fd_array_member_t* f);
  int32_t (*read)(fd_array_member_t* f, void* buf, int32_t nbytes);
  int32_t (*write)(fd_array_member_t* f, const void* buf, int32_t nbytes);
};

int32_t fs_interface_init(fd_array_member_t* fd_array);
int32_t fs_interface_read(fd_array_member_t* f, void* buf, int32_t nbytes);
int32_t fs_interface_write(fd_array_member_t* f, const void* buf, int32_t nbytes);
int32_t fs_interface_open(fd_array_member_t* f, const uint8_t* filename);
int32_t fs_interface_close(fd_array_member_t* f);

#endif
