#ifndef TASK_H
#define TASK_H

#include "types.h"
#include "filesystem/filesys_interface.h"

#define MAX_FILE_COUNT 8
#define MAX_PID_COUNT 3
#define FILE_NAME_LEN 32

typedef struct pcb {
    int32_t pid;
    int32_t parent_pid;
    fd_array_member_t fd_array[MAX_FILE_COUNT];
    uint32_t esp;
    uint32_t eip;
    uint32_t ebp;
    uint8_t file_arg[FILE_NAME_LEN];
    uint32_t active;
} pcb_t;

extern int32_t curr_pid;
extern pcb_t* curr_pcb;

void task_init();
pcb_t* get_pcb(uint32_t pid);
int32_t get_new_pid();

#endif
