#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"

#define PASS 1
#define FAIL 0
#define NUM_EXCEPTIONS 32
#define SYSCALL_INT 0x80
#define UNUSED_MEM 0x800000
#define PDE_SIZE 4

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
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
    clear();
    TEST_OUTPUT("idt_test", idt_test());
    TEST_OUTPUT("idt_test_values", idt_test_values());
    TEST_OUTPUT("test_values_in_paging_structs", test_values_in_paging_structs());
    TEST_OUTPUT("test_paging_can_access", test_paging_can_access());
    // TEST_OUTPUT("test_paging_cant_access", test_paging_cant_access());
    // TEST_OUTPUT("test_dereference_null", test_dereference_null());
    // TEST_OUTPUT("syscalls_test", syscalls_test());
    // TEST_OUTPUT("exception_divide_by_zero_test", divide_by_zero_test());
    // TEST_OUTPUT("exception_invalid_opcode", invalid_opcode_test());
    // TEST_OUTPUT("exception_overflow", overflow_test());

}
