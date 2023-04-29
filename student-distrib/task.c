#include "task.h"
#include "address.h"

int32_t curr_pid = -1;
pcb_t* curr_pcb = NULL;

/* 
 * task_init
 *   DESCRIPTION: Initialize the tasking system by setting all the PCBs to inactive
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void task_init() {
    pcb_t* pcb;
    int32_t i;
    for (i = 0; i < MAX_PID_COUNT; i++) {
        pcb = get_pcb(i);
        pcb->active = 0;
        pcb->terminal_id = -1;
        pcb->is_vidmapped = 0;
    }
}

/* 
 * get_pcb
 *   DESCRIPTION: Get the PCB of the process with the given pid
 *   INPUTS: pid -- the pid of the process
 *   OUTPUTS: none
 *   RETURN VALUE: the PCB of the process with the given pid
 *   SIDE EFFECTS: none */
pcb_t* get_pcb(uint32_t pid) {
    if (pid < 0 || pid >= MAX_PID_COUNT) {
        return NULL;
    }
    return (pcb_t *) (KERNEL_STACK_ADDR - (pid + 1) * USER_KERNEL_STACK_SIZE);
}

/* 
 * get_new_pid
 *   DESCRIPTION: Get the pid of a new process by finding the first inactive PCB and setting it to active
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: the pid of a new process
 *   SIDE EFFECTS: none */
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
