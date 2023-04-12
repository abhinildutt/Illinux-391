#ifndef FILESYS_H
#define FILESYS_H

#include "../types.h"
#include "../lib.h"
#include "filesys_interface.h"

#define FILE_NAME_LEN 32
#define BLOCK_SIZE 4096
#define FILE_TYPE_SIZE 4
#define FILE_SIZE_SIZE 4

#define FILE_TYPE_RTC 0
#define FILE_TYPE_DIR 1
#define FILE_TYPE_FILE 2

typedef struct dentry {
    uint8_t filename[FILE_NAME_LEN];
    uint32_t filetype;
    uint32_t inode_num;
    uint8_t reserved[24];
} dentry_t;

typedef struct boot_block {
    uint32_t num_dentries;
    uint32_t num_inodes;
    uint32_t num_data_blocks;
    uint8_t reserved[52];
    dentry_t dentries[63];
} boot_block_t;

typedef struct inode {
    uint32_t length;
    uint32_t data_block_num[1023];
} inode_t;

typedef struct data_block {
    uint8_t data[BLOCK_SIZE];
} data_block_t;

boot_block_t* boot_block_ptr;
inode_t* inode_ptr;
data_block_t* data_block_ptr;

dentry_t curr_dentry;

int32_t curr_idx;

extern funcptrs directory_fops;
extern funcptrs regular_fops;

void fs_init(uint32_t * fs_start_addr);

extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

extern int32_t file_read(fd_array_member_t* f, void* buf, int32_t nbytes);
extern int32_t file_write(fd_array_member_t* f, const void* buf, int32_t nbytes);
extern int32_t file_open(fd_array_member_t* f, const uint8_t* filename);
extern int32_t file_close(fd_array_member_t* f);

extern int32_t dir_read(fd_array_member_t* f, void* buf, int32_t nbytes);
extern int32_t dir_write(fd_array_member_t* f, const void* buf, int32_t nbytes);
extern int32_t dir_open(fd_array_member_t* f, const uint8_t* filename);
extern int32_t dir_close(fd_array_member_t* f);

#endif
