#ifndef FILESYS_H
#define FILESYS_H

#include "types.h"
#include "lib.h"

#define FILE_NAME_LEN 32
#define BLOCK_SIZE 4096
#define FILE_TYPE_SIZE 4
#define FILE_SIZE_SIZE 4

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

void fs_init(uint32_t * fs_start_addr);


extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);



extern int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t file_open(const uint8_t* filename);
extern int32_t file_close(int32_t fd);

extern int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t dir_open(const uint8_t* filename);
extern int32_t dir_close(int32_t fd);



#endif
