#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#ifndef ASM

#define PAGE_SIZE_4KB 4096
#define PAGE_SIZE_4MB 0x400000
#define TABLE_SIZE 1024
#define KERNEL_MEM 0x400000
#define VIDEO_MEM 0xB8000

#define PROGRAM_IMAGE_VIRTUAL_ADDR 0x08048000
#define PROGRAM_IMAGE_PHYSICAL_BASE_ADDR 0x800000
// index 32
#define PROGRAM_IMAGE_PD_IDX (PROGRAM_IMAGE_VIRTUAL_ADDR >> 22)
#define PROGRAM_IMAGE_OFFSET 0x00048000
#define PROGRAM_ENTRY_POINT 24
// index 33, right after the program image
#define PROGRAM_VIDEO_PD_IDX ((PROGRAM_IMAGE_VIRTUAL_ADDR + PAGE_SIZE_4MB) >> 22)
#define PROGRAM_VIDEO_VIRUTAL_ADDR (PROGRAM_IMAGE_VIRTUAL_ADDR + PAGE_SIZE_4MB + VIDEO_MEM)

#define KERNEL_STACK_ADDR 0x800000
#define USER_KERNEL_STACK_SIZE 0x2000
#define USER_STACK_VIRTUAL_ADDR 0x08000000 // 128 MB

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
void map_program(int32_t pid);
void unmap_program(int32_t pid);
void map_video_mem();
void unmap_video_mem();
void flush_tlb();

#endif
#endif
