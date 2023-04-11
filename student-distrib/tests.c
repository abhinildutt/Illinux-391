#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "interrupt_handlers/syscalls_def.h"
#include "interrupt_handlers/exception.h"
#include "interrupt_handlers/idt.h"
#include "filesys.h"
#include "devices/rtc.h"
#include "devices/keyboard.h"
#include "devices/terminal.h"


#define PASS 1
#define FAIL 0
#define NUM_EXCEPTIONS 32
#define SYSCALL_INT 0x80
#define UNUSED_MEM 0x800000
#define PDE_SIZE 4

#define MAX_RTC_FREQ 1024
#define RESET_FREQ 2

/* format these macros as you see fit */
#define TEST_HEADER     \
    printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)   \
    printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
    /* Use exception #15 for assertions, otherwise
       reserved by Intel */
    asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 32 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test() {
    TEST_HEADER;

    int i;
    int result = PASS;
    for (i = 0; i < 10; ++i){
        if ((idt[i].offset_15_00 == NULL) && 
            (idt[i].offset_31_16 == NULL)){
            assertion_failure();
            result = FAIL;
        }
    }

    return result;
}

/* IDT Test - Test Values
 * 
 * Asserts that first 32 IDT entries are set to correct values
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test_values() {
    TEST_HEADER;

    int i;
    int result = PASS;
    for (i = 0; i < NUM_VEC; ++i){
        if (idt[i].seg_selector != KERNEL_CS) {
            assertion_failure();
            result = FAIL;
        }
        if (idt[i].present != 1) {
            assertion_failure();
            result = FAIL;
        }

        if (idt[i].size != 1) {
            assertion_failure();
            result = FAIL;
        }

        // Check if its a interrupt gate or trap date
        if (i < NUM_EXCEPTIONS) {
            if (idt[i].reserved3 != 1) {
                assertion_failure();
                result = FAIL;
            }
        } else {
            if (idt[i].reserved3 != 0) {
                assertion_failure();
                result = FAIL;
            }
        }

        // Check ring 0 for kernel calls, ring 3 for syscalls
        if (i == SYSCALL_INT) {
            if (idt[i].dpl != 3) {
                assertion_failure();
                result = FAIL;
            }
        } else {
            if (idt[i].dpl != 0) {
                assertion_failure();
                result = FAIL;
            }
        }
    }

    return result;
}


/* Syscall Test
 * 
 * Asserts that syscalls are handled
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Syscall handling
 * Files: syscall.c/h
 */
int syscalls_test() {
    TEST_HEADER;
    int i;
    int ret;
    for (i = 2; i <= 10; i++) {
        asm volatile
        (
            "int $0x80"
            : "=a" (ret)
            : "0"(i), "b"(NULL), "c"(NULL), "d"(NULL)
            : "memory"
        );
    }
    // Test halt syscall (1) separately since it halts the program
    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(1), "b"(68), "c"(NULL), "d"(NULL)
        : "memory"
    );
    return FAIL;
}

/* Exception Test - Divide by Zero
 * 
 * Asserts that divide by zero exception is handled
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Exception handling
 * Files: exception.c/h
 */
int divide_by_zero_test() {
    TEST_HEADER;
    int result = FAIL;
    int zero = 0;
    int ans = 1 / zero;
    return result + ans - ans;
}


/* Exception Test - Invalid Opcode Test
 * 
 * Asserts that invalid opcode exception is handled
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Exception handling
 * Files: exception.c/h
 */
int invalid_opcode_test() {
    TEST_HEADER;
    int result = FAIL;

    /* The instruction tries to access a non-existent control register (for example, mov cr6, eax). */
    asm volatile
    (
        "mov %%cr6, %%eax"
        :
        :
        : "eax"
    );

    return result;
}

/* Exception Test - Overflow Test
 * 
 * Asserts that overflow exception is handled
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Exception handling
 * Files: exception.c/h
 */
int overflow_test() {
    TEST_HEADER;
    int result = FAIL;

    /* Create overflow instruction that will cause exception */
        
    asm volatile ("                                               \n\
        movl $0x7FFFFFFF, %%eax                                   \n\
        addl $0x1, %%eax                                          \n\
        into                                                      \n\
        "                                                           \
        :                                                   \
        :                                                   \
        : "eax"                                             \
    );  

    return result;
}

/* Dereference Null Test
 * 
 * Asserts that dereferencing a null pointer causes a page fault
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Page Fault Handling
 * Files: paging.c/h
 */
int test_dereference_null() {
    TEST_HEADER;
    int result = FAIL;
    int* ptr = NULL;
    *ptr = 0;
    return result;
}

/* Paging - Dereference Test
 * 
 * Asserts that we can access memory that is present
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Page Fault Handling
 * Files: paging.c/h
 */
int test_paging_can_access() {
    TEST_HEADER;
    int result = PASS;

    /* Test beginning of kernel memory */
    int* ptr = (int*)KERNEL_MEM;
    *ptr = 0x12345678;
    if (*ptr != 0x12345678) {
        assertion_failure();
        result = FAIL;
    }

    /* Test end of kernel memory */
    ptr = (int*)(UNUSED_MEM - PDE_SIZE);
    *ptr = 0x12345678;
    if (*ptr != 0x12345678) {
        assertion_failure();
        result = FAIL;
    }

    /* Test beginning of video memory */
    ptr = (int*)VIDEO_MEM;
    *ptr = 0x12345678;
    if (*ptr != 0x12345678) {
        assertion_failure();
        result = FAIL;
    }

    /* Test end of video memory */
    ptr = (int*)(VIDEO_MEM + PAGE_SIZE - PDE_SIZE);
    *ptr = 0x12345678;
    if (*ptr != 0x12345678) {
        assertion_failure();
        result = FAIL;
    }

    return result;
}

/* Paging Test - Can't Access
 * 
 * Asserts that we can't access memory that's not present
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Page Fault Handling
 * Files: paging.c/h */
int test_paging_cant_access() {

    int result = PASS;

    /* Test can't access memory that's not present */
    int* ptr = (int*)UNUSED_MEM;
    *ptr = 0x12345678;
    if (*ptr == 0x12345678) {
        assertion_failure();
        result = FAIL;
    }

    /* Test can't access just before kernel memory */
    ptr = (int*)(KERNEL_MEM - PDE_SIZE);
    *ptr = 0x12345678;
    if (*ptr == 0x12345678) {
        printf("Failed to catch access to just before kernel memory");
        result = FAIL;
    }

    /* Test can't access just before video memory */
    ptr = (int*)(VIDEO_MEM - PDE_SIZE);
    *ptr = 0x12345678;
    if (*ptr == 0x12345678) {
        printf("Failed to catch access to just before video memory");
        result = FAIL;
    }

    /* Test can't access just after video memory */
    ptr = (int*)(VIDEO_MEM + PAGE_SIZE);
    *ptr = 0x12345678;
    if (*ptr == 0x12345678) {
        printf("Failed to catch access to just after video memory");
        result = FAIL;
    }

    return result;
}


/* Paging Test - Values in Paging Structs
 * 
 * Asserts that the values in the paging structs are correct
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging
 * Files: paging.c/h */
int test_values_in_paging_structs() {
    int result = PASS;

    int i; /* Go through each entry in the page directory */

    /* Test values of first two page directory entries */
    if (page_directory[0].present != 1) {
        printf("First entry in page directory is not present");
        result = FAIL;
    }
    if (page_directory[0].read_write != 1) {
        printf("First entry in page directory is not read/write");
        result = FAIL;
    }
    if (page_directory[0].cache_disable != 1) {
        printf("First entry in page directory is not cached");
        result = FAIL;
    }
    if (page_directory[1].present != 1) {
        printf("Second entry in page directory is not present");
        result = FAIL;
    }
    if (page_directory[1].read_write != 1) {
        printf("Second entry in page directory is not read/write");
        result = FAIL;
    }
    if (page_directory[1].cache_disable != 1) {
        printf("Second entry in page directory is not cached");
        result = FAIL;
    }
    
    /* Test rest of the page directory entries to not present */
    for (i = 2; i < TABLE_SIZE; i++) {
        if (page_directory[i].present != 0) {
            printf("Entry %d in page directory is present", i);
            result = FAIL;
        }
    }

    /* Test all page table entries except video memory to not present */
    for (i = 0; i < TABLE_SIZE; i++) {
        if (i * PAGE_SIZE == VIDEO_MEM) {
            if (page_table[i].present != 1) {
                printf("Entry %d in page table is not present", i);
                result = FAIL;
            }
        } else {
            if (page_table[i].present != 0) {
                printf("Entry %d in page table is present", i);
                result = FAIL;
            }
        }
    }

    return result;
}


/* Checkpoint 2 tests */

/* RTC Test - Changing Frequency
 * 
 * Asserts that we can change the frequency of the RTC
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: RTC
 * Files: rtc.c/h */
int test_changing_rtc_freq() {
    // Tests changing RTC frequencies, receiving RTC interrupts at each possible frequency
    // Tests if we can change the rate of the RTC clock using the write function and
    // that the read function returns after an interrupt has occured
    int result = PASS;

    const uint8_t* filename = (uint8_t*)"rtc";
    int32_t fd = rtc_open(filename);

    int freq;
    int num_bytes_written = 4;
    int i;

    for (freq = 2; freq <= MAX_RTC_FREQ; freq *= 2) {
        if (rtc_write(fd, &freq, num_bytes_written) == -1) {
            printf("Failed to change RTC frequency to %d", freq);
            result = FAIL;
        }

        for (i = 0; i <= freq; i++) {
            if (rtc_read(fd, NULL, 0) == -1) {
                printf("Failed to receive RTC interrupt at frequency %d", freq);
                result = FAIL;
            }
        }

        clear();
        interrupt_counter = 0;
    }  

    // Test bad buffer pointer to write
    if (rtc_write(fd, NULL, num_bytes_written) != -1) {
        printf("RTC write succeeded with bad buffer pointer");
        result = FAIL;
    }  

    // Set frequency back to 2
    freq = 2;
    rtc_write(fd, &freq, num_bytes_written);

    return result;
}

/* RTC Test - Helper Functions
 * 
 * Asserts that the helper functions for the RTC work
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: RTC
 * Files: rtc.c/h */
int test_rtc_helper_funcs() {
    int result = PASS;

    const uint8_t* filename = (uint8_t*)"rtc";
    rtc_open(filename);

    // Test frequency that's not a power of 2
    if (set_rtc_freq(3) != -1) {
        printf("RTC set frequency succeeded with invalid frequency");
        result = FAIL;
    }

    // Test frequency that's too high
    if (set_rtc_freq(MAX_RTC_FREQ + 1) != -1) {
        printf("RTC set frequency succeeded with invalid frequency");
        result = FAIL;
    }

    // // Test frequency that's too low
    if (set_rtc_freq(1) != -1) {
        printf("RTC set frequency succeeded with invalid frequency");
        result = FAIL;
    }

    // Test rate is 15 when desired frequency is 2
    if (freq_to_rate(2) != 15) {
        printf("RTC set frequency failed to set rate to 15 for frequency 2");
        result = FAIL;
    }

    return result;
}

/* Filesystem Test - Ls
    * 
    * Asserts that we can list out every file name, file type, and file size in the fsdir directory
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Filesystem
    * Files: filesys.c/h */
int test_filesys_ls() {
    int result = PASS;

    // List out every file name, file type, and file size in the fsdir directory
    const uint8_t* filename = (uint8_t*)".";
    int32_t fd = dir_open(filename);

    int buffer_size = FILE_NAME_LEN + FILE_TYPE_SIZE + FILE_SIZE_SIZE;
    uint8_t buf[buffer_size];
    int32_t num_bytes_read = buffer_size;
    int i;
    int num_files = 17;
    for (i = 0; i < num_files; i++) {
        if (dir_read(fd, buf, num_bytes_read) == -1) {
            printf("Failed to read directory entry");
            result = FAIL;
        }
        // printf("File name: %s File type: %d File size: %d\n", buf, *(buf + FILE_NAME_LEN), *(uint32_t*)(buf + FILE_NAME_LEN + FILE_TYPE_SIZE));

        printf("File name: ");
        // Use putc to print file name
        int j;
        for (j = 0; j < FILE_NAME_LEN; j++) {
            putc(buf[j]);
        }

        // Use putc to print file type
        printf(", File type: ");
        putc(*(buf + FILE_NAME_LEN) + '0');

        // Use putc to print file size
        printf(", File size: %d\n", *(uint32_t*)(buf + FILE_NAME_LEN + FILE_TYPE_SIZE));
    }

    return result;
}

/* Filesystem Test - Cat
    * 
    * Asserts that we can read the contents of a small file
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Filesystem
    * Files: filesys.c/h */
int test_filesys_small_cat() {
    int result = PASS;

    // Read contents of frame0.txt file in fsdir directory
    const uint8_t* directory = (uint8_t*)".";
    int32_t fd = dir_open(directory);

    int buffer_size = 187;
    uint8_t buf[buffer_size];
    int32_t num_bytes_read = buffer_size;

    const uint8_t* filename = (uint8_t*)"frame0.txt";
    file_open(filename);
    if (file_read(fd, buf, num_bytes_read) == -1) {
        printf("Failed to read file");
        result = FAIL;
    } else {
        int i;
        for (i = 0; i < num_bytes_read; i++) {
            putc(buf[i]);
        }
    }

    return result;
}

/* Filesystem Test - Cat
    * 
    * Asserts that we can read the contents of a large file
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Filesystem
    * Files: filesys.c/h */
int test_filesys_large_cat() {
    int result = PASS;

    // Read contents of verylargetextwithverylongname.tx file in fsdir directory
    const uint8_t* directory = (uint8_t*)".";
    int32_t fd = dir_open(directory);

    int buffer_size = 5277;
    uint8_t buf[buffer_size];
    int32_t num_bytes_read = buffer_size;

    const uint8_t* filename1 = (uint8_t*)"verylargetextwithverylongname.txt";
    if (file_open(filename1) != -1) {
        printf("Opened file that doesn't exist");
        result = FAIL;
    }

    const uint8_t* filename2 = (uint8_t*)"verylargetextwithverylongname.tx";
    file_open(filename2);
    if (file_read(fd, buf, num_bytes_read) == -1) {
        printf("Failed to read file");
        result = FAIL;
    } else {
        int i;
        for (i = 0; i < num_bytes_read; i++) {
            putc(buf[i]);
        }
    }

    return result;
}


/* Filesystem Test - Cat
    * 
    * Asserts that we can read the contents of a file that's not in the fsdir directory
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Filesystem
    * Files: filesys.c/h */
int test_filesys_executable_cat() {
    int result = PASS;

    // Read contents of grep file in fsdir directory
    const uint8_t* directory = (uint8_t*)".";
    int32_t fd = dir_open(directory);

    int buffer_size = 6149;
    uint8_t buf[buffer_size];
    int32_t num_bytes_read = buffer_size;

    const uint8_t* filename = (uint8_t*)"grep";
    file_open(filename);
    if (file_read(fd, buf, num_bytes_read) == -1) {
        printf("Failed to read file");
        result = FAIL;
    } else {
        int i;
        for (i = 0; i < num_bytes_read; i++) {
            putc(buf[i]);
        }
    }
    return result;
}


/* Filesystem Test - Garbage/bad input
    * 
    * Asserts that system doesn't fail after receiving garbage/bad input
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Filesystem
    * Files: filesys.c/h */
int test_filesys_bad_input() {
    int result = PASS;

    // Test bad fs_start_addr to fs_init
    boot_block_t* boot_block_ptr_cpy = boot_block_ptr;
    inode_t* inode_ptr_cpy = inode_ptr;
    data_block_t* data_block_ptr_cpy = data_block_ptr;
    fs_init(NULL);
    if (boot_block_ptr != boot_block_ptr_cpy || inode_ptr != inode_ptr_cpy || data_block_ptr != data_block_ptr_cpy) {
        printf("Filesystem init succeeded with bad fs_start_addr");
        result = FAIL;
    }

    // Test read_dentry_by_name with bad file name and bad dentry
    dentry_t dentry;
    if (read_dentry_by_name(NULL, &dentry) != -1) {
        printf("Filesystem read_dentry_by_name succeeded with bad file name");
        result = FAIL;
    }

    // Test read_dentry_by_index with bad index and bad dentry
    if (read_dentry_by_index(-1, &dentry) != -1) {
        printf("Filesystem read_dentry_by_index succeeded with bad index");
        result = FAIL;
    }

    // Test read_data with bad inode and buffer
    if (read_data(-1, 0, NULL, 0) != -1) {
        printf("Filesystem read_data succeeded with bad inode and buffer");
        result = FAIL;
    }

    const uint8_t* directory = (uint8_t*)".";
    int32_t fd = dir_open(directory);
    int32_t num_bytes_read = 10;

    // Test dir_read with bad buffer
    if (dir_read(fd, NULL, num_bytes_read) != -1) {
        printf("Filesystem dir_read succeeded with bad buffer");
        result = FAIL;
    }

    // Test dir_open with bad filename
    if (dir_open(NULL) != -1) {
        printf("Filesystem dir_open succeeded with bad filename");
        result = FAIL;
    }

    return result;

}


/* Terminal Test - Read
    * 
    * Asserts that we can read from the terminal
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Terminal
    * Files: terminal.c/h */
int terminal_read_test(void) {
    printf("this is terminal read test\n");

    char buf[KBUFFER_SIZE];
    int32_t i, size;

    printf("testing full read > ");
    size = term_read(NULL, buf, KBUFFER_SIZE);
    printf("you typed: ");
    for(i = 0; i < size; i++) {
        putc(buf[i]);
    }
    printf("\n==================\nsize = %d\n", size);

    printf("testing read again (type a) > ");
    size = term_read(NULL, buf, KBUFFER_SIZE);
    printf("you typed: ");
    for(i = 0; i < size; i++) {
        putc(buf[i]);
    }
    printf("\n==================\nsize = %d\n", size);
    return PASS;
}

/* Terminal Test - Read/Write
    * 
    * Asserts that we can read and write from the terminal
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Terminal
    * Files: terminal.c/h */
int terminal_read_write_test(void) {
    printf("this is terminal read-write test\n");

    char buf[KBUFFER_SIZE];
    int32_t size;

    printf("testing read > ");
    size = term_read(NULL, buf, KBUFFER_SIZE);

    printf("you typed: ");
    int written;
    written = term_write(NULL, buf, size);
    printf("\n==================\nsize = %d\n", written);
    
    if (written == -1) {
        return FAIL;
    }
    return PASS;
}

/* Terminal Test - Write
    * 
    * Asserts that we can write to the terminal
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Terminal
    * Files: terminal.c/h */
int terminal_write_test(void) {
    printf("this is terminal write test\n");

    char buf[128] = "hello world?";
    int written;
    written = term_write(NULL, buf, 12);
    printf("\n==================\nsize = %d\n", written);
    if (written == -1) {
        return FAIL;
    }
    return PASS;
}

/* Terminal Test - Null
    * 
    * Asserts that we can't read/write to a null buffer
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Terminal
    * Files: terminal.c/h */
int terminal_null_test(void) {
    printf("this is terminal null test\n");

    printf("testing read to NULL buffer > ");
    int size;
    size = term_read(NULL, NULL, KBUFFER_SIZE);
    if (size != -1) {
        printf("read was successful, should have failed\n");
        return FAIL;
    }

    putc('\n');

    int written;
    written = term_write(NULL, NULL, 100);
    if (written != -1) {
        printf("write was successful, should have failed\n");
        return FAIL;
    }
    return PASS;
}

/* Checkpoint 3 tests */

/* Syscall Test - Open
    * 
    * Asserts that we can get a pcb and open a file in that task and see that the file descriptor is in use
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Syscall
    * Files: syscall.c/h */
int syscalls_open_test() {
    TEST_HEADER;
    int ret;
    printf("-------------------SYSCALL OPEN TEST----------------------\n");
    const uint8_t* filename = (uint8_t*)"frame0.txt";
    int pid = 0;
    pcb_t* curr_pcb = get_curr_pcb(pid);

    int open_arg = 5;

    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(open_arg), "b"(filename), "c"(NULL), "d"(NULL)
        : "memory"
    );
    
    if(ret != -1) {
        printf("fd number : %d \n", ret);

        int i;
        for(i = 0; i < MAX_NUM_FILES; i++) {
            printf("| index %d flag | %d | \n", i, curr_pcb->fd_array[i].flags);
        }

        return PASS;
    }

    return FAIL;
}

/* Syscall Test - Close
    * 
    * Asserts that we can get a pcb and close a file in that task and see that the file descriptor is no longer in use
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Syscall
    * Files: syscall.c/h */
int syscalls_close_test() {
    TEST_HEADER;
    int ret;

    printf("-------------------SYSCALL CLOSE TEST----------------------\n");
    const int8_t fd = 2;
    int close_arg = 6;

    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(close_arg), "b"(fd), "c"(NULL), "d"(NULL)
        : "memory"
    );
    
    if(ret != -1) {
        printf("close status : %d \n", ret);
        int i;
        for(i = 0; i < MAX_NUM_FILES; i++) {
            printf("| index %d flag | %d | \n", i, curr_pcb->fd_array[i].flags);
        }
        return PASS;
    }

    return FAIL;
}

/* Syscall Test - Read
    * 
    * Asserts that we can get a pcb and read a file in that task and see that the file descriptor is in use
    * Also read file to the display
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Syscall
    * Files: syscall.c/h */
int syscalls_read_test() {
    TEST_HEADER;
    int ret;

    printf("-------------------SYSCALL READ TEST----------------------\n");
    const uint8_t* filename = (uint8_t*)"frame0.txt";
    int open_arg = 5;

    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(open_arg), "b"(filename), "c"(NULL), "d"(NULL)
        : "memory"
    );
    
    const int8_t fd = ret;
    char buf[KBUFFER_SIZE];
    
    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(3), "b"(fd), "c"(buf), "d"(KBUFFER_SIZE)
        : "memory"
    );
    
    if(ret != -1) {
        printf("size : %d \n", ret);
        printf("contents:\n");
        int i;
        for(i = 0; i < ret; i++) {
            putc(buf[i]);
        }
        printf("\n==================\nsize = %d\n", ret);
        return PASS;
    }

    return FAIL;
}

/* Syscall Test - Read Write
    * 
    * Asserts that when we write, stdout is updated and when we read a file, the fd_array is updated
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Syscall
    * Files: syscall.c/h */
int syscalls_read_write_test() {
    TEST_HEADER;
    int ret;

    printf("-------------------SYSCALL READ-WRITE TEST----------------------\n");
    const uint8_t* filename = (uint8_t*)"frame0.txt";
    
    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(5), "b"(filename), "c"(NULL), "d"(NULL)
        : "memory"
    );
    
    if(ret == -1) return FAIL;

    const int8_t fd = ret;
    char buf[KBUFFER_SIZE];

    int read_arg = 3;
    
    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(read_arg), "b"(fd), "c"(buf), "d"(KBUFFER_SIZE)
        : "memory"
    );

    int write_arg = 4;
    
    if(ret != -1) {
        printf("size : %d \n", ret);
        printf("contents:\n");
        int size = ret;
        asm volatile
        (
            "int $0x80"
            : "=a" (ret)
            : "0"(write_arg), "b"(1), "c"(buf), "d"(size) // stdout
            : "memory"
        );
        return PASS;
    }

    return FAIL;
}

/* Syscall Test - STD Read Write
    * 
    * Asserts that we can read and write to the standard input and output
    * Inputs: None
    * Outputs: PASS/FAIL
    * Side Effects: None
    * Coverage: Syscall
    * Files: syscall.c/h */
int syscalls_std_read_write_test() {
    TEST_HEADER;
    int ret;

    printf("-------------------SYSCALL STD READ-WRITE TEST----------------------\n");
    char buf[KBUFFER_SIZE];
    
    int read_arg = 3;
    int write_arg = 4;

    asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(read_arg), "b"(0), "c"(buf), "d"(KBUFFER_SIZE)
        : "memory"
    );
    
    if(ret != -1) {
        printf("size : %d \n", ret);
        printf("contents:\n");
        int size = ret;
        asm volatile
        (
            "int $0x80"
            : "=a" (ret)
            : "0"(write_arg), "b"(1), "c"(buf), "d"(size) // stdout
            : "memory"
        );
        return PASS;
    }

    return FAIL;
}
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests() {
    // Checkpoint 1 tests
    // TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("idt_test_values", idt_test_values());
    // TEST_OUTPUT("test_values_in_paging_structs", test_values_in_paging_structs());
    // TEST_OUTPUT("test_paging_can_access", test_paging_can_access());
    // TEST_OUTPUT("test_paging_cant_access", test_paging_cant_access());
    // TEST_OUTPUT("test_dereference_null", test_dereference_null());
    // TEST_OUTPUT("syscalls_test", syscalls_test());
    // TEST_OUTPUT("exception_divide_by_zero_test", divide_by_zero_test());
    // TEST_OUTPUT("exception_invalid_opcode", invalid_opcode_test());
    // TEST_OUTPUT("exception_overflow", overflow_test());


    // Checkpoint 2 tests
    // TEST_OUTPUT("test_changing_rtc_freq", test_changing_rtc_freq());
    // TEST_OUTPUT("test_rtc_helper_funcs", test_rtc_helper_funcs());
    // TEST_OUTPUT("test_filesys_ls", test_filesys_ls());
    // TEST_OUTPUT("test_filesys_small_cat", test_filesys_small_cat());
    // TEST_OUTPUT("test_filesys_bad_input", test_filesys_bad_input());
    // TEST_OUTPUT("test_filesys_large_cat", test_filesys_large_cat());
    // TEST_OUTPUT("test_filesys_executable_cat", test_filesys_executable_cat());

    // TEST_OUTPUT("terminal_read_test", terminal_read_test());
    // TEST_OUTPUT("terminal_read_write_test", terminal_read_write_test());
    // TEST_OUTPUT("terminal_write_test", terminal_write_test());
    // TEST_OUTPUT("terminal_null_test", terminal_null_test());

    // Checkpoint 3 tests
    TEST_OUTPUT("test_syscall_open", syscalls_open_test());
    // TEST_OUTPUT("test_syscall_close", syscalls_close_test());
    // TEST_OUTPUT("test_syscall_read", syscalls_read_test());
    // TEST_OUTPUT("test_syscall_read_write", syscalls_read_write_test());
    // TEST_OUTPUT("test_syscall_std_read_write", syscalls_std_read_write_test());

}
