#ifndef _ADDRESS_H
#define _ADDRESS_H

#define PAGE_SIZE_4KB 4096
#define PAGE_SIZE_4MB 0x400000
#define TABLE_SIZE 1024
#define KERNEL_MEM 0x400000
#define VIDEO_MEM 0xB8000
// 0xB8
#define VIDEO_MEM_INDEX (VIDEO_MEM >> 12)
// 3 terminals max!
#define VIDEO_MEM_BACKGROUND_START_ADDR (VIDEO_MEM + PAGE_SIZE_4KB)
#define VIDEO_MEM_BACKGROUND_START_INDEX (VIDEO_MEM_BACKGROUND_START_ADDR >> 12)
#define VIDEO_MEM_BACKGROUND_END_INDEX (VIDEO_MEM_BACKGROUND_START_INDEX + 2)

#define PROGRAM_IMAGE_OFFSET 0x00048000
#define PROGRAM_IMAGE_VIRTUAL_BASE_ADDR 0x08000000
#define PROGRAM_IMAGE_VIRTUAL_ADDR (PROGRAM_IMAGE_VIRTUAL_BASE_ADDR + PROGRAM_IMAGE_OFFSET)
#define PROGRAM_IMAGE_PHYSICAL_BASE_ADDR 0x800000
// index 32
#define PROGRAM_IMAGE_PD_IDX (PROGRAM_IMAGE_VIRTUAL_ADDR >> 22)
#define PROGRAM_ENTRY_POINT 24
#define PROGRAM_VIDEO_VIRTUAL_ADDR (PROGRAM_IMAGE_VIRTUAL_BASE_ADDR + PAGE_SIZE_4MB + VIDEO_MEM)
// index 33, right after the program image
#define PROGRAM_VIDEO_PD_IDX (PROGRAM_VIDEO_VIRTUAL_ADDR >> 22)

#define KERNEL_STACK_ADDR 0x800000
#define USER_KERNEL_STACK_SIZE 0x2000
#define USER_STACK_VIRTUAL_ADDR 0x08000000 // 128 MB

#endif