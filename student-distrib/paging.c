#include "paging.h"
#include "address.h"
#include "lib.h"

extern void loadPageDirectory(int);
extern void enablePaging();

/*
 * initialize_paging
 *   DESCRIPTION: Initializes paging by setting up the page directory and page table
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets up page directory and page table
 */
void initialize_paging() {
    // set first entry in table (4mb with 4kb pages)
    page_directory[0].present = 1;
    page_directory[0].read_write = 1;
    page_directory[0].user_supervisor = 0;
    page_directory[0].write_through = 0;
    page_directory[0].cache_disable = 1;
    page_directory[0].accessed = 0;
    page_directory[0].reserved = 0;
    page_directory[0].page_size = 0;
    page_directory[0].global_page = 0;
    page_directory[0].available = 0;
    page_directory[0].page_table_addr = ((uint32_t) page_table) / PAGE_SIZE_4KB;

    // set page table entries for video memory
    int i;
    for (i = 0; i < TABLE_SIZE; i++){
        page_table[i].present = 0;
        page_table[i].read_write = 1;
        page_table[i].user_supervisor = 0;
        page_table[i].write_through = 0;
        page_table[i].cache_disable = 1;
        page_table[i].accessed = 0;
        page_table[i].dirty = 0;
        page_table[i].pt_attribute_index = 0;
        page_table[i].global_page = 0;
        page_table[i].available = 0;
        page_table[i].page_addr = 0;
        // || i == VIDEO_PERM_MEM_INDEX
        if (i == VIDEO_MEM_INDEX || i == VIDEO_PERM_MEM_INDEX) {
            page_table[i].present = 1;
            page_table[i].page_addr = VIDEO_MEM_INDEX;
            page_table[i].cache_disable = 0;
        } else if (i >= VIDEO_MEM_BACKGROUND_START_INDEX && i <= VIDEO_MEM_BACKGROUND_END_INDEX) {
            page_table[i].present = 1;
            page_table[i].page_addr = i;
            page_table[i].cache_disable = 0;
        }
    }

    // set second entry in table to kernel memory
    page_directory[1].present = 1;
    page_directory[1].read_write = 1;
    page_directory[1].user_supervisor = 0;
    page_directory[1].write_through = 0;
    page_directory[1].cache_disable = 1;
    page_directory[1].accessed = 0;
    page_directory[1].reserved = 0;
    page_directory[1].page_size = 1;
    page_directory[1].global_page = 0;
    page_directory[1].available = 0;
    page_directory[1].page_table_addr = KERNEL_MEM / PAGE_SIZE_4KB;

    // set rest of the entries to not present
    for (i = 2; i < TABLE_SIZE; i++) {
        page_directory[i].present = 0;
        page_directory[i].read_write = 1;
        page_directory[i].user_supervisor = 0;
        page_directory[i].write_through = 0;
        page_directory[i].cache_disable = 1;
        page_directory[i].accessed = 0;
        page_directory[i].reserved = 0;
        page_directory[i].page_size = 1;
        page_directory[i].global_page = 0;
        page_directory[i].available = 0;
        page_directory[i].page_table_addr = 0;
    }

    // set up vidmap page directory entry
    page_directory[PROGRAM_VIDEO_PD_IDX].present = 0;
    page_directory[PROGRAM_VIDEO_PD_IDX].read_write = 1;
    page_directory[PROGRAM_VIDEO_PD_IDX].user_supervisor = 1;
    page_directory[PROGRAM_VIDEO_PD_IDX].write_through = 0;
    page_directory[PROGRAM_VIDEO_PD_IDX].cache_disable = 1;
    page_directory[PROGRAM_VIDEO_PD_IDX].accessed = 0;
    page_directory[PROGRAM_VIDEO_PD_IDX].reserved = 0;
    page_directory[PROGRAM_VIDEO_PD_IDX].page_size = 0;
    page_directory[PROGRAM_VIDEO_PD_IDX].global_page = 0;
    page_directory[PROGRAM_VIDEO_PD_IDX].available = 0;
    page_directory[PROGRAM_VIDEO_PD_IDX].page_table_addr = ((uint32_t) vidmap_page_table) / PAGE_SIZE_4KB;

    // set up vidmap page table
    for (i = 0; i < TABLE_SIZE; i++) {
        vidmap_page_table[i].present = 0;
        vidmap_page_table[i].read_write = 1;
        vidmap_page_table[i].user_supervisor = 1;
        vidmap_page_table[i].write_through = 0;
        vidmap_page_table[i].cache_disable = 1;
        vidmap_page_table[i].accessed = 0;
        vidmap_page_table[i].dirty = 0;
        vidmap_page_table[i].pt_attribute_index = 0;
        vidmap_page_table[i].global_page = 0;
        vidmap_page_table[i].available = 0;
        vidmap_page_table[i].page_addr = 0;
        if (i == VIDEO_MEM_INDEX) { // physical
            vidmap_page_table[i].present = 1;
            vidmap_page_table[i].page_addr = VIDEO_MEM_INDEX;
            vidmap_page_table[i].cache_disable = 0;
        }
    }

    loadPageDirectory((int) page_directory);
    enablePaging();
}

/* 
 * map_program
 *   DESCRIPTION: Maps a program to a page directory entry (maps virtual address to physical address)
 *   INPUTS: pid - the process id of the program
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: maps a program to a page directory entry
 */
void map_program(int32_t pid, uint8_t is_vidmapped, uint32_t owning_terminal_id, uint8_t is_terminal_displayed) {
    // Per the docs, the first user-level program (the shell) should be loaded at physical 8 MB,
    // and the second user-level program, when it is executed by the shell, should be loaded at
    // physical 12 MB
    uint32_t physical_addr = PROGRAM_IMAGE_PHYSICAL_BASE_ADDR + PAGE_SIZE_4MB * pid;
    page_directory[PROGRAM_IMAGE_PD_IDX].present = 1;
    page_directory[PROGRAM_IMAGE_PD_IDX].read_write = 1;
    page_directory[PROGRAM_IMAGE_PD_IDX].user_supervisor = 1;
    page_directory[PROGRAM_IMAGE_PD_IDX].write_through = 0;
    page_directory[PROGRAM_IMAGE_PD_IDX].cache_disable = 0;
    page_directory[PROGRAM_IMAGE_PD_IDX].accessed = 0;
    page_directory[PROGRAM_IMAGE_PD_IDX].reserved = 0;
    page_directory[PROGRAM_IMAGE_PD_IDX].page_size = 1;
    page_directory[PROGRAM_IMAGE_PD_IDX].global_page = 0;
    page_directory[PROGRAM_IMAGE_PD_IDX].available = 0;
    page_directory[PROGRAM_IMAGE_PD_IDX].page_table_addr = physical_addr / PAGE_SIZE_4KB;

    // Mapping video memory (physical & virtual)
    page_directory[PROGRAM_VIDEO_PD_IDX].present = is_vidmapped == 1;
    if (is_terminal_displayed) {
        // Direct access to video memory (with VIDEO_MEM)
        page_table[VIDEO_MEM_INDEX].page_addr = VIDEO_MEM_INDEX;
        // Virtual memory access to video memory (with PROGRAM_VIDEO_VIRTUAL_ADDR)
        vidmap_page_table[VIDEO_MEM_INDEX].page_addr = VIDEO_MEM_INDEX;
    } else {
        // Redirect to background video memory
        page_table[VIDEO_MEM_INDEX].page_addr = VIDEO_MEM_BACKGROUND_START_INDEX + owning_terminal_id;
        vidmap_page_table[VIDEO_MEM_INDEX].page_addr = VIDEO_MEM_BACKGROUND_START_INDEX + owning_terminal_id;
    }
    flush_tlb();
    // printf("mapping %d (vidmap=%d, tid=%d, displayed=%d)\n", pid, is_vidmapped, owning_terminal_id, is_terminal_displayed);
}

/* 
 * unmap_program
 *   DESCRIPTION: Unmaps a program from a page directory entry (unmaps virtual address to physical address)
 *   INPUTS: pid - the process id of the program
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: unmaps a program from a page directory entry
 */
void unmap_program(int32_t pid) {
    page_directory[PROGRAM_IMAGE_PD_IDX].present = 0;
    flush_tlb();
}

/* 
 * flush_tlb
 *   DESCRIPTION: Flushes the TLB
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: flushes the TLB
 */
void flush_tlb() {
    // https://wiki.osdev.org/TLB
    asm volatile(
        "movl %%cr3, %%eax  \n"
        "movl %%eax, %%cr3  \n"
        :
        :
        : "memory", "cc"
    );
}
