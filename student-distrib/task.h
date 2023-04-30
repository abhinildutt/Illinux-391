#ifndef TASK_H
#define TASK_H

#include "types.h"
#include "filesystem/filesys_interface.h"

#define MAX_FILE_COUNT 8
#define MAX_PID_COUNT 6
#define FILE_NAME_LEN 32

typedef struct pcb {
    int32_t pid;                                // pid
    int32_t parent_pid;                         // parent's pid (-1 if none)
    fd_array_member_t fd_array[MAX_FILE_COUNT]; // file descriptor array
    uint32_t esp;                               // ESP stored for context switches
    uint32_t eip;                               // EIP of the task
    uint32_t ebp;                               // EBP stored for context switches
    uint8_t file_arg[FILE_NAME_LEN];            // launch argument
    uint32_t active;                            // whether the task is active
    uint32_t terminal_id;                       // terminal the task is runnning on
    uint8_t is_vidmapped;                       // whether vidmap was called
} pcb_t;

extern int32_t curr_pid;
extern pcb_t* curr_pcb;

void task_init();
pcb_t* get_pcb(uint32_t pid);
int32_t get_new_pid();

#endif
