#ifndef _SYSCALLS_DEF_H
#define _SYSCALLS_DEF_H

#include "../types.h"

#include "rtc.h"
#include "keyboard.h"
#include "terminal.h"
#include "filesys.h"

typedef struct
{
  int32_t (*open)(const uint8_t* filename);
  int32_t (*close)(int32_t fd);
  int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
  int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
}  funcptrs;

typedef struct fd_array_member {
    uint32_t fops_pointer;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags;
} fd_array_member_t;

fd_array_member_t fd_array[8];

funcptrs rtc_fop = {rtc_open, rtc_close, rtc_read, rtc_write};
funcptrs directory_fop = {dir_open, dir_close, dir_read, dir_write};
funcptrs regular_fop = {file_open, file_close, file_read, file_write};
// funcptrs stdin_fop = {keyboard_init, NULL, keyboard_handler, NULL};
funcptrs stdout_fop = {term_open, term_close, term_read, term_write};



int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t getargs(uint8_t* buf, int32_t nbytes);
int32_t vidmap(uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);

#endif
