#ifndef _SYSCALLS_DEF_H
#define _SYSCALLS_DEF_H

#include "../types.h"

#include "../devices/rtc.h"
#include "../devices/keyboard.h"
#include "../devices/terminal.h"
#include "../filesys.h"


#define EIGHT_MB 0x800000
#define EIGHT_KB 0x2000
#define MAX_NUM_FILES 8


typedef struct
{
  int32_t (*open)(const uint8_t* filename);
  int32_t (*close)(int32_t fd);
  int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
  int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
}  funcptrs;

typedef struct {
    funcptrs* fops_pointer;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags;
} fd_array_member_t;

typedef struct pcb{
  uint32_t pid;
  uint32_t parent_pid;
  fd_array_member_t fd_array[MAX_NUM_FILES];
  uint32_t esp;
  uint32_t ebp;
  uint32_t active;
} pcb_t;

funcptrs rtc_fop;
funcptrs directory_fop;
funcptrs regular_fop;
funcptrs stdin_fop;
funcptrs stdout_fop;

void fd_array_init();

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
pcb_t* get_curr_pcb(uint32_t curr_pid);
pcb_t* curr_pcb;

#endif
