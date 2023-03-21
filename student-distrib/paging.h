#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#ifndef ASM


# define PAGE_SIZE 4096
# define TABLE_SIZE 1024
# define KERNEL_MEM 0x400000
# define VIDEO_MEM 0xB8000

typedef struct __attribute__((packed)) page_directory_entry_t {
    uint32_t present : 1;           // 0
    uint32_t read_write : 1;        // 1
    uint32_t user_supervisor : 1;   // 2
    uint32_t write_through : 1;     // 3
    uint32_t cache_disable : 1;     // 4
    uint32_t accessed : 1;          // 5
    uint32_t reserved : 1;          // 6
    uint32_t page_size : 1;         // 7
    uint32_t global_page : 1;       
    uint32_t available : 3;
    uint32_t page_table_addr : 20;
} page_directory_entry_t;


typedef struct __attribute__((packed)) page_table_entry_t {
    uint32_t present : 1;
    uint32_t read_write : 1;
    uint32_t user_supervisor : 1;
    uint32_t write_through : 1;
    uint32_t cache_disable : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t pt_attribute_index : 1;
    uint32_t global_page : 1;
    uint32_t available : 3;
    uint32_t page_addr : 20;
} page_table_entry_t;


page_directory_entry_t page_directory[TABLE_SIZE] __attribute__((aligned(PAGE_SIZE)));
page_table_entry_t page_table[TABLE_SIZE] __attribute__((aligned(PAGE_SIZE)));

extern void initialize_paging();


#endif
#endif
