// Simple Test application for the process_ancestor syscall.
// By Brian Fraser

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>

#include "process_ancestors.h"
#include "syscall_numbers.h"


/**
 * Prototypes
 */
static int do_syscall(struct process_info array[], long size, long *num_filled);
static void do_syscall_working(long size);
static void do_syscall_failing(char* msg, struct process_info array[], long size, long *num_filled, long ret_code);
static void test_internal(char *msg, struct process_info array[], long size, bool success, int lineNum, char* argStr);
static void test_print_summary(void);
static void sleep_ms(int duration_ms);
static void print_process_info_array(struct process_info array[], long size);
static int single_child_testing(int spawn_array[], int depth);
static int spawn_children_and_test_as_root(int spawn_array[], int depth);
void display_printk_help();


// Don't make these prototypes static so we can comment them out
void test_success();
void test_failures();
void test_pid();
void test_context_switch();
void test_name();
void test_state();
void test_children_siblings();
void test_user();


/***********************************************************
 * Main
 * - pass argument "nospawn" to disable spawning
 ***********************************************************/
int main(int argc, char *argv[])
{
	test_success();
	test_failures();
	test_pid();
	test_pid();             // Call twice to double its score
	test_context_switch();
	test_context_switch();  // Call twice to double its score
	test_name();
	test_name();            // Call twice to double its score
	test_state();

	if (argc > 1 && strcmp("nospawn", argv[1]) == 0) {
		printf("** Skipping test children siblings! ** \n");
	} else {
		test_children_siblings();
	}

	// Must be run last: changes uid.
	test_user();

	test_print_summary();
	return 0;
}






/***********************************************************
 * Custom testing framework
 ***********************************************************/
static const char *current_test_function = "";
#define print_test_header() do{ \
		current_test_function = __func__;                      \
	} while (0)
/*
#define print_test_header() do{ \
		current_test_function = __func__;                      \
		printf("\n\n");                                        \
		printf("*****************************************\n"); \
		printf("   Testing in funct '%s()'\n",  __func__);     \
		printf("*****************************************\n"); \
	} while (0)
*/    


// Track results:
static int numTests = 0;
static int numTestPassed = 0;

static int current_syscall_test_num = 0;
static int last_syscall_test_num_failed = -1;
static int num_syscall_tests_failed = 0;

// Macro to allow us to get the line number, and argument's text:
#define TEST(arg) test_internal("", NULL, 0, (arg), __LINE__, #arg)
#define TEST_MSG(msg, arg) test_internal((msg), NULL, 0, (arg), __LINE__, #arg)
#define TEST_MSGARRAY(msg, array, numfilled, arg) test_internal((msg), (array), (numfilled), (arg), __LINE__, #arg)

// Actual function used to check success/failure:
static void test_internal(char* msg, struct process_info array[], long num_filled, bool success, int lineNum, char* argStr)
{
	numTests++;
	if (!success) {
		if (current_syscall_test_num != last_syscall_test_num_failed) {
			last_syscall_test_num_failed = current_syscall_test_num;
			num_syscall_tests_failed++;
		}
		fprintf(stderr, "ERROR '%s' (#%d in function %s() line %d)\n", 
                msg, current_syscall_test_num, current_test_function, lineNum);
		fprintf(stderr, "     failed `%s`\n", argStr);
        print_process_info_array(array, num_filled);
	} else {
		numTestPassed++;
	}
}

static void test_print_summary(void)
{
	const int SCORE_PER_CHECK = 2;
	int num_unique_passed = current_syscall_test_num - num_syscall_tests_failed;

	printf("\nExecution finished.\n");
	// printf("  %2d/%2d individual TEST checks passed.\n", numTestPassed, numTests);
	// printf("  %2d/%2d tests FAILED.\n", numTests - numTestPassed, numTests);
	printf("  %2d/%2d unique test configurations passed\n", num_unique_passed, current_syscall_test_num);
	printf("  %2d/%2d score\n", num_unique_passed * SCORE_PER_CHECK, current_syscall_test_num * SCORE_PER_CHECK);
}




/***********************************************************
 * High-Level Tests & main()
 ***********************************************************/
void test_success()
{
	print_test_header();

	do_syscall_working(1);
	do_syscall_working(2);
	do_syscall_working(100);
}

void test_pid()
{
	print_test_header();

	const long SIZE = 30;
	const long SWAPPER_PID = 0;
	const long INIT_PID = 1;

	struct process_info *array = malloc(sizeof(*array) * SIZE);

	// Call syscall (assume succeeds)
	long num_filled = -1;
	do_syscall(array, SIZE, &num_filled);

	// Check that my PID is correct:
	long my_pid = (long) getpid();
	TEST_MSGARRAY("Testing PID correct", array, num_filled, my_pid == array[0].pid);

	// Check than nobody else has PID same as mine:
	for (int i = 1; i < num_filled; i++) {
		TEST_MSGARRAY("Testing nobody else has my PID", array, num_filled, my_pid != array[i].pid);
	}

	// Check that swapper and init are 0 and 1:
	int swapper_idx = num_filled - 1;
	TEST_MSGARRAY("Testing swapper's PID", array, num_filled, array[swapper_idx].pid == SWAPPER_PID);
	TEST_MSGARRAY("Testing init's PID", array, num_filled, array[swapper_idx-1].pid == INIT_PID);

	// Cleanup
	free(array);
}

void test_context_switch()
{
	print_test_header();

	const long SIZE = 30;

	// Get base line
	struct process_info array_start[SIZE];
	long num_filled = -1;
	do_syscall(array_start, SIZE, &num_filled);
	// TEST(result == 0 && num_filled > 0);
	// if (result != 0) {
	// 	return;
	// }
	// print_process_info_array(array_start, num_filled);

	// Sleep (voluntary context switch)
	sleep_ms(10);

	// Busy wait (involuntary context switch)
	const long BUSY_WAIT_COUNT = 100000000;
	volatile long count = 0;
	for (int i = 0; i < BUSY_WAIT_COUNT; i++) {
		count++;
	}

	// Get new reading
	struct process_info array_end[SIZE];
	do_syscall(array_end, SIZE, &num_filled);
	// TEST(result == 0 && num_filled > 0);
	// if (result != 0) {
	// 	return;
	// }
	// print_process_info_array(array_end, num_filled);

	// Check differences
	TEST_MSGARRAY("Testing involuntary context switches", array_end, num_filled, array_start[0].nivcsw < array_end[0].nivcsw);
	TEST_MSGARRAY("Testing voluntary context switches", array_end, num_filled, array_start[0].nvcsw < array_end[0].nvcsw);
}

void test_name()
{
	print_test_header();

	const long SIZE = 30;

	// Get base line
	struct process_info array[SIZE];
	long num_filled = -1;
	do_syscall(array, SIZE, &num_filled);
	// TEST(result == 0 && num_filled > 0);
	// if (result != 0) {
	// 	return;
	// }

	// Check my name:
	TEST_MSGARRAY("Testing my name", array, num_filled, strncmp(array[0].name, program_invocation_short_name, ANCESTOR_NAME_LEN - 1) == 0);

	// Check swapper and init:
	int swapper_idx = num_filled - 1;
	TEST_MSGARRAY("Testing swapper name", array, num_filled, strncmp(array[swapper_idx].name, "swapper/0", ANCESTOR_NAME_LEN) == 0);
	TEST_MSGARRAY("Testing init name", array, num_filled, strncmp(array[swapper_idx - 1].name, "init", ANCESTOR_NAME_LEN) == 0);
}

void test_state()
{
	print_test_header();

	const long SIZE = 30;
	#define TASK_RUNNING            0
	#define TASK_INTERRUPTIBLE      1
	#define TASK_UNINTERRUPTIBLE    2
	#define TASK_ZOMBIE             4
	#define TASK_STOPPED            8

	// Get base line
	struct process_info array[SIZE];
	long num_filled = -1;
	do_syscall(array, SIZE, &num_filled);
	// TEST(result == 0 && num_filled > 0);
	// if (result != 0) {
	// 	return;
	// }
	// print_process_info_array(array, num_filled);

	// Test that I am running
	TEST_MSGARRAY("Testing that I'm in running state", array, num_filled, array[0].state == TASK_RUNNING);

	// Test that all my ancestors are OK:
	for (int i = 1; i < num_filled; i++) {
		TEST_MSGARRAY("Testing ancestor is running, interrutptable, or uninterruptable", array, num_filled, 
				(array[i].state == TASK_RUNNING)
				|| (array[i].state == TASK_INTERRUPTIBLE)
				|| (array[i].state == TASK_UNINTERRUPTIBLE)
			);
	}
	// Test that not all are RUNNING:
	bool all_running = true;
	for (int i = 1; i < num_filled; i++) {
		if (array[i].state != TASK_RUNNING) {
			all_running = false;
		}
	}
	TEST_MSGARRAY("Testing that not all processes are running", array, num_filled, !all_running);
}

void test_failures()
{
	print_test_header();

	long num_filled = -1;
	struct process_info *array = malloc(10 * sizeof(struct process_info));

	// Test bad size:
	do_syscall_failing("Testing bad size (0)", array, 0, &num_filled, EINVAL);
	do_syscall_failing("Testing bad size (-1)", array, -1, &num_filled, EINVAL);

	// Test bad pointers to process:
	do_syscall_failing("Testing bad array to fill (NULL)", NULL, 10, &num_filled, EFAULT);
	do_syscall_failing("Testing bad array to fill (*1)", (void*)1, 10, &num_filled, EFAULT);

	// Test bad pointers to num filled
	do_syscall_failing("Testing bad num filled (NULL)", array, 10, NULL, EFAULT);
	do_syscall_failing("Testing bad num filled (*1)", array, 10, (void*)1, EFAULT);

	free(array);
}


void user_id_check(int expected_uid)
{
	// Get info from syscall
	const long SIZE = 30;
	struct process_info array[SIZE];
	long num_filled = -1;
	do_syscall(array, SIZE, &num_filled);
    // Assume it fills correctly.
	// TEST_MSG("Failed Syscall for changing User ID", result == 0 && num_filled > 0);
	// if (result != 0) {
	// 	return;
	// }
	// Check that current process matches UID:
	TEST_MSGARRAY("Incorrect USER ID", array, num_filled,
        array[0].uid == expected_uid);
}

/**
 * This test must be run as root because we change to another user.
 * If not running as root, this will crash the test program.
 */
#define UID_FOR_USER 1000
#define UID_FOR_ROOT 0
void test_user()
{
	print_test_header();
	// Check that I'm currently "root"
	assert(getuid() == UID_FOR_ROOT);

	// Check syscall for returning UID as root
	user_id_check(UID_FOR_ROOT);

	// Switch:
	if (setuid(UID_FOR_USER) != 0) {
	    printf("setuid() to %d failed", UID_FOR_USER);
	    assert(false);
	    exit(1);
	}
	assert(getuid() == UID_FOR_USER);

	// check getting correct UID:
	user_id_check(UID_FOR_USER);
}

void display_printk_help(void)
{
	printf("\n");
	printf("Help limiting spew from syscall's printk():\n");
	printf("  View current printk level (1st num): cat /proc/sys/kernel/printk\n");
	printf("  Turn on printk's:                    echo 7 > /proc/sys/kernel/printk\n");
	printf("  Turn off printk's:                   echo 1 > /proc/sys/kernel/printk\n");
	printf("  Store stderr to file (clean!):       ./process_ancestor_test 2> errors\n");
	printf("                                       cat errors\n");
	printf("\n");
}



/***********************************************************
 * syscall and test results
 ***********************************************************/
static int do_syscall(struct process_info array[], long size, long *num_filled)
{
	current_syscall_test_num++;
	// printf("\nTest %d: ..Diving to kernel level\n", current_syscall_test_num);
	int result = syscall(SYSCALL_PROCESS_ANCESTOR, array, size, num_filled);
	int my_errno = errno;
	// printf("..Rising to user level w/ result = %d", result);
	if (result < 0) {
		// printf("..Rising to user level w/ result = %d, errno = %d\n", result, my_errno);
	} else {
		// printf("..Rising to user level w/ result = %d\n", result);
		my_errno = 0;
	}

	return my_errno;

}
static void do_syscall_working(long size)
{
	const int FILL = 0xFFFFFFFFL;
	// Allocate 2 extra structs: one at beginning and at end.
	struct process_info array[size + 2];
	memset(array, FILL, (size + 2) * sizeof(struct process_info));

	// Have syscall fill starting at idx 1 (not 0!) to catch if overflowing the buffer.
	long num_filled = -1;
	int result = do_syscall(&array[1], size, &num_filled);
	// print_process_info_array(&array[1], num_filled);
	TEST_MSG("Testing return of a correct syscall", result == 0);
	TEST_MSG("Testing correct value in num_filled", num_filled > 0 && num_filled <= size);

	// Check that before the first element is unchanged
	struct process_info blank;
	memset(&blank, FILL, sizeof(blank));
	TEST_MSG("Testing did not overwrite memory before array", memcmp(&blank, &array[0], sizeof(blank)) == 0);
	// Check that after the last element is unchanged
	TEST_MSG("Testing did not overwrite memory after array", memcmp(&blank, &array[size+1], sizeof(blank)) == 0);
}
static void do_syscall_failing(char* msg, struct process_info array[], long size, long *num_filled, long ret_code)
{
	int result = do_syscall(array, size, num_filled);
	TEST_MSG(msg, result == ret_code);
}




/***********************************************************
 * Utility Functions
 ***********************************************************/
static void sleep_ms(int duration_ms)
{
	struct timespec req;
	req.tv_nsec = (duration_ms % 1000) * 1000 * 1000;
	req.tv_sec = (duration_ms / 1000);
	nanosleep(&req, NULL);
}

static void print_process_info_array(struct process_info array[], long size)
{
    if (array == NULL) {
        return;
    }

	sleep_ms(100);
	const int DEF_WIDTH = 7;
	const int NAME_WIDTH = ANCESTOR_NAME_LEN + 1;
	// Print Headers
	printf("%*s%*s %-*s%*s%*s%*s%*s%*s%*s\n",
			DEF_WIDTH, "Idx#",
			DEF_WIDTH, "PID",
			NAME_WIDTH, "Name",
			DEF_WIDTH, "State",
			DEF_WIDTH, "UID",
			DEF_WIDTH, "#VCSW",
			DEF_WIDTH, "#IVCSW",
			DEF_WIDTH, "#Child",
			DEF_WIDTH, "#Sib"
			);

	// Print Data
	for (int i = size - 1; i >= 0; i--) {
		printf("%*d%*ld %-*s%*ld%*ld%*ld%*ld%*ld%*ld\n",
				DEF_WIDTH, i,
				DEF_WIDTH, array[i].pid,
				NAME_WIDTH, array[i].name,
				DEF_WIDTH, array[i].state,
				DEF_WIDTH, array[i].uid,
				DEF_WIDTH, array[i].nvcsw,
				DEF_WIDTH, array[i].nivcsw,
				DEF_WIDTH, array[i].num_children,
				DEF_WIDTH, array[i].num_siblings
		);
	}
}






/***********************************************************
 * Children and Siblings: Fork processes
 ***********************************************************/
void test_children_siblings()
{
	print_test_header();

	TEST(spawn_children_and_test_as_root((int[]){1, 0}, 0) == 0);
	TEST(spawn_children_and_test_as_root((int[]){4, 0}, 0) == 0);
	TEST(spawn_children_and_test_as_root((int[]){4, 3, 2, 1, 0}, 0) == 0);
	// TEST(spawn_children_and_test_as_root((int[]){2, 2, 0}, 0) == 0);
	// TEST(spawn_children_and_test_as_root((int[]){5, 5, 5, 5, 5, 5, 5, 5, 0}, 0) == 0);
	TEST(spawn_children_and_test_as_root((int[]){50, 1, 50, 0}, 0) == 0);
	TEST(spawn_children_and_test_as_root((int[]){1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 4, 2, 5, 3, 2, 4, 5, 1, 3, 1, 2, 2, 3, 3, 4, 1, 0}, 0) == 0);
}


/*
   spawn_array is the number of siblings to have at each level.
    = {2, 3, 0} 
   gives:
 	  Parent -> 
	    A --> Children X, Y, Z
		B --> <no children>
*/
static void child_waits();
static void spawn_children(int spawn_array[], int depth) ;
static int wait_on_children(int num_spawned);
static int spawn_children_and_test_as_first_child(int spawn_array[], int depth);

#define TEST_NAME "/test_semaphore"
#define END_NAME "/end_semaphore"
static int spawn_children_and_test_as_root(int spawn_array[], int depth)
{
	int tests_failed = 0;

	// Create semaphores
	// -------------------------------------------------------------
	// NOTE: with a static linked binary sem_unlink segfaults
	// See bug: https://stackoverflow.com/a/47914897/3475174
	if (sem_unlink(TEST_NAME) != 0 && errno != ENOENT) {
		printf("ERROR: Unable to unlink the TEST semaphore!\n");
		exit(1);
	}
	if (sem_unlink(END_NAME) != 0 && errno != ENOENT) {
		printf("ERROR: Unable to unlink the END semaphore!\n");
		exit(1);
	}
	printf("Spawning children test {");
	for (int i = 0; spawn_array[i] != 0; i++) {
		if (i > 0) {
			printf(", ");
		}
		printf("%d", spawn_array[i]);
	}
	printf("}...\n");


	// ..shared (named) semaphores.
	sem_t *start_testing = sem_open(TEST_NAME, O_CREAT, 0644, 0);
	sem_t *ok_to_end = sem_open(END_NAME, O_CREAT, 0664, 0);

	// Spawn children
	// -------------------------------------------------------------
	spawn_children(spawn_array, depth);

	// Signal to all 1st children to start test
	// -------------------------------------------------------------
	sleep_ms(1000);
	// printf("-> Signaling to start testing\n");
	sem_post(start_testing);
	tests_failed += single_child_testing(spawn_array, depth);

	// Signal all children to exit
	sleep_ms(1000);
	// printf("-> Signaling all children to exit\n");
	sem_post(ok_to_end);

	// Wait for all my children to exit
	// -------------------------------------------------------------
	int num_children_spawned = spawn_array[depth];
	tests_failed += wait_on_children(num_children_spawned);

	// Cleanup semephores
	// -------------------------------------------------------------
	sem_close(ok_to_end);
	sem_close(start_testing);
	if (sem_unlink(TEST_NAME) != 0 && errno != ENOENT) {
		printf("ERROR: Unable to unlink the TEST semaphore at end!\n");
		exit(1);
	}
	if (sem_unlink(END_NAME) != 0 && errno != ENOENT) {
		printf("ERROR: Unable to unlink the END semaphore at end!\n");
		exit(1);
	}

	return tests_failed;
}


static void child_waits()
{
	// Wait on master to end
	sem_t *ok_to_end = sem_open(END_NAME, 0);
	sem_wait(ok_to_end);
	sem_post(ok_to_end);
	sem_close(ok_to_end);
}

static void spawn_children(int spawn_array[], int depth) 
{
	int num_to_spawn = spawn_array[depth];
	// printf("-> Depth %d spawning %d child processes.\n", depth, num_to_spawn);
	for (int i = 0; i < num_to_spawn; i++) {
		pid_t pid = fork();

		// Are we the child?
		if (pid == 0) {
			int num_child_tests_failed = 0;
			if (i == 0) {
				// First child keeps spawning
				num_child_tests_failed = spawn_children_and_test_as_first_child(spawn_array, depth + 1);
			} else {
				// Child waits
				child_waits();
			}
			// printf("  << Exiting child (level %d)\n", depth);
			exit(num_child_tests_failed);
		}
	}
}

static int wait_on_children(int num_spawned)
{
	int tests_failed = 0;
	for (int i = 0; i < num_spawned; i++) {
		int status = 0;
		pid_t pid_wait = wait(&status);
		if (WIFEXITED(status)) {
			// printf("  Process pid %d returned # %d.\n", pid_wait, WEXITSTATUS(status));
			tests_failed += WEXITSTATUS(status);
		} else {
			printf("---> ERROR Wait(): Process with pid %d returned 0x%x.\n", pid_wait, status);
			tests_failed ++;
		}
	}
	return tests_failed;
}

static int spawn_children_and_test_as_first_child(int spawn_array[], int depth)
{
	int tests_failed = 0;

	// Spawn children
	// -------------------------------------------------------------
	spawn_children(spawn_array, depth);
	

	// Wait on master to begin test
	// ..re-open semaphores in shared space:
	sem_t *start_testing = sem_open(TEST_NAME, 0);
	sem_wait(start_testing);
	sem_post(start_testing);
	sem_close(start_testing);


	// Test
	tests_failed = single_child_testing(spawn_array, depth);

	child_waits();

	// Wait for all my children to exit
	// -------------------------------------------------------------
	int num_children_spawned = spawn_array[depth];
	tests_failed += wait_on_children(num_children_spawned);

	return tests_failed;
}

static int single_child_testing(int spawn_array[], int depth)
{
	int count_failed = 0;

	// Create memory for results (+ any stack above us)
	const long SIZE = depth + 10;
	struct process_info *array = malloc(sizeof(*array) * SIZE);


	// Call syscall
	long num_filled = -1;
	int result = do_syscall(array, SIZE, &num_filled);
	if (result != 0) {
		printf("---> ERROR: Syscall failed with return value %d (errno %d)\n", result, errno);
		return 1;
	}

	// Check all levels above us are correct.
	for (int i = depth; i >= 0; i--) {
		int process_info_idx = depth - i;

		// Check siblings only for not at root of tree.
		if (i > 0) {
			// Don't count self as a sibling
			int num_siblings_expected = spawn_array[i - 1] - 1;
			int num_siblings_got = array[process_info_idx].num_siblings;

			bool exactly_equal = num_siblings_expected == num_siblings_got;
			bool counted_parent_too = num_siblings_expected == num_siblings_got + 1;
			if (!exactly_equal && !counted_parent_too) {
				printf("---> ERROR: Incorrect # siblings; %d process above current: exp %d == got %d\n",
						process_info_idx, num_siblings_expected, num_siblings_got);
				count_failed ++;
			}
		}

		// Check children
		int num_children_expected = spawn_array[i];
		int num_children_got = array[process_info_idx].num_children;
		// printf("   %d: Children @ depth %d: exp %d == got %d?\n",
		// 		depth, i, num_children_expected, num_children_got);
		if (num_children_expected != num_children_got) {
			printf("---> ERROR: Incorrect # children; %d process above current: exp %d == got %d\n",
					process_info_idx, num_children_expected, num_children_got);
			count_failed ++;
		}
	}

	// If we are at the bottom and have error, then print out data:
	if (spawn_array[depth] == 0 && count_failed > 0) {
		print_process_info_array(array, num_filled);
	}


	free(array);
	return count_failed;
}




