#ifndef _PAGING_H
#define _PAGING_H

#ifndef ASM
#include "types.h"
#include "address.h"

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

page_directory_entry_t page_directory[TABLE_SIZE] __attribute__((aligned(PAGE_SIZE_4KB)));
page_table_entry_t page_table[TABLE_SIZE] __attribute__((aligned(PAGE_SIZE_4KB)));
page_table_entry_t vidmap_page_table[TABLE_SIZE] __attribute__((aligned(PAGE_SIZE_4KB)));

void initialize_paging();
void map_program(int32_t pid, uint8_t is_vidmapped, uint32_t owning_terminal_id, uint8_t is_terminal_active);
void unmap_program(int32_t pid);
void flush_tlb();

#endif
#endif
