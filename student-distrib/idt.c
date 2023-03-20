#include "idt.h"
#include "x86_desc.h"

void setup_idt() {
    for(int i = 0; i < NUM_VEC; i++) {
        idt[i].present = 0;
        idt[i].seg_selector = KERNEL_CS;
        idt[i].dpl = 0;
        idt[i].size = 1;
        if(i == 0x80) idt[i].dpl = 3;
        
        idt[i].reserved0 = 0;
        idt[i].reserved1 = 0;
        idt[i].reserved2 = 0;
        idt[i].reserved3 = 0;
        idt[i].reserved4 = 0;
    }
}