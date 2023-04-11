#include "task.h"
#include "paging.h"

int32_t curr_pid = -1;
pcb_t* curr_pcb = NULL;

void task_init() {
    int32_t i;
    for (i = 0; i < MAX_PID_COUNT; i++) {
        get_pcb(i)->active = 0;
    }
}

pcb_t* get_pcb(uint32_t pid) {
    if (pid < 0 || pid >= MAX_PID_COUNT) {
        return NULL;
    }
    return (pcb_t *) (KERNEL_STACK_ADDR - (pid + 1) * USER_KERNEL_STACK_SIZE);
}

int32_t get_new_pid() {
    int32_t i;
    for (i = 0; i < MAX_PID_COUNT; i++) {
        if (get_pcb(i)->active == 0) {
            get_pcb(i)->active = 1;
            return i;
        }
    }
    return -1;
}
