#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
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

int exceptions_test() {
    TEST_HEADER;
    int result = PASS;
    int ans = 1 / 0;
    return result;
}

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
    return PASS;
}

// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
    clear();
	TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("exceptions_test", exceptions_test());
    TEST_OUTPUT("syscalls_test", syscalls_test());
}
