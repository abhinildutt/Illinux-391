#include "paging.h"

extern void loadPageDirectory(int);
extern void enablePaging();

void initialize_paging(){

    //set first entry in table (4mb with 4kb pages)
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
    page_directory[0].page_table_addr = ((uint32_t) page_table) /PAGE_SIZE;

    // set page table entries for video memory
    int i;
    for(i = 0; i < TABLE_SIZE ; i++){
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
        page_table[i].page_addr = i;
        if (i*PAGE_SIZE == VIDEO_MEM){
            page_table[i].present = 1;
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

    page_directory[1].page_table_addr = (KERNEL_MEM /PAGE_SIZE);

    // set rest of the entries to not present
    for(i = 2; i < TABLE_SIZE; i++){
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
        page_directory[i].page_table_addr =  0;
    }

    loadPageDirectory((int)page_directory);
    enablePaging();

}