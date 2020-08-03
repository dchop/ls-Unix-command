// Test application for the array_stats syscall.

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "array_stats.h"
#include "syscall_numbers.h"


/**
 * Prototypes
 */
void test_positive_few(void);
void test_negative_few(void);
void test_many(void);
void test_bad_addr(void);
static void do_syscall_working(long data[], long size);
static void do_syscall_failing(struct array_stats *stats, long data[], long size, long ret_code);
static void test_internal(_Bool success, int lineNum, char* argStr);
static void test_print_summary(void);
// Track results:
static int numTests = 0;
static int numTestPassed = 0;

static int current_syscall_test_num = 0;
static int last_syscall_test_num_failed = -1;
static int num_syscall_tests_failed = 0;
static int simple_test_num = 0;
static int num_simple_tests_failed = 0;
static int complex_test_num = 0;
static int num_complex_tests_failed = 0;

/***********************************************************
 * main()
 ***********************************************************/
int main(int argc, char *argv[])
{
	test_positive_few(); //5
	test_negative_few(); //4
	test_many(); //1

	test_bad_addr(); //10

	test_print_summary();
	return 0;
}


/***********************************************************
 * Testing routines for specific test
 ***********************************************************/
void test_positive_few()
{
	int total = current_syscall_test_num;
	int failed = num_syscall_tests_failed;
	do_syscall_working((long[]){1}, 1);
	do_syscall_working((long[]){1, 2}, 2);
	do_syscall_working((long[]){1, 2, 3}, 3);
	do_syscall_working((long[]){0, 2, 4, 6}, 4);
	do_syscall_working((long[]){5, 3, 4, 1}, 4);
	total = current_syscall_test_num - total;
	failed = num_syscall_tests_failed - failed;
	simple_test_num += total;
	num_simple_tests_failed += failed;


}
void test_negative_few()
{
	int total = current_syscall_test_num;
	int failed = num_syscall_tests_failed;
	do_syscall_working((long[]){-1}, 1);
	do_syscall_working((long[]){-1, -2}, 2);
	do_syscall_working((long[]){-1, 2, 3}, 3);
	do_syscall_working((long[]){0, -2, 4, -6}, 4);
	total = current_syscall_test_num - total;
	failed = num_syscall_tests_failed - failed;
	simple_test_num += total;
	num_simple_tests_failed += failed;
}

void fill(long data[], long size)
{
	for (int i = 0; i < size; i++) {
		data[i] = rand();
		if (i % 2 == 0) {
			data[i] *= -1;
		}
	}
}
#define MEG (1024*1024)
void test_many()
{
	int total = current_syscall_test_num;
	int failed = num_syscall_tests_failed;
	long size = MEG * 5;
	long *data = malloc(sizeof(data[0]) * size);
	fill(data, size);
	do_syscall_working(data, size);
	free(data);
	total = current_syscall_test_num - total;
	failed = num_syscall_tests_failed - failed;
	simple_test_num += total;
	num_simple_tests_failed += failed;
}

void test_bad_addr()
{
	struct array_stats stats;
	int total = current_syscall_test_num;
	int failed = num_syscall_tests_failed;
	// 0 or negative sizes
	do_syscall_failing(&stats, (long[]){1}, 0, EINVAL);
	// Bad data pointers
	do_syscall_failing(&stats, NULL, 1, EFAULT);
	total = current_syscall_test_num - total;
	failed = num_syscall_tests_failed - failed;
	simple_test_num += total;
	num_simple_tests_failed += failed;



	total = current_syscall_test_num;
	failed = num_syscall_tests_failed;
	// 0 or negative sizes
	do_syscall_failing(&stats, (long[]){1}, -1, EINVAL);
	do_syscall_failing(&stats, (long[]){1}, -10000, EINVAL);

	// Bad data pointers
	do_syscall_failing(&stats, (long*)1LL, 1, EFAULT);
	do_syscall_failing(&stats, (long*)123456789012345689LL, 1, EFAULT);

	// Bad size (read off end)
	do_syscall_failing(&stats, (long[]){1}, 10*MEG, EFAULT);

	// Bad stats pointers, or read-only memory
	do_syscall_failing(NULL, (long[]){1}, 1, EFAULT);
	do_syscall_failing((void*)1, (long[]){1}, 1, EFAULT);
	do_syscall_failing((void*)test_bad_addr, (long[]){1}, 1, EFAULT);

	total = current_syscall_test_num - total;
	failed = num_syscall_tests_failed - failed;
	complex_test_num += total;
	num_complex_tests_failed += failed;
}



/***********************************************************
 * Custom testing framework
 ***********************************************************/



// Macro to allow us to get the line number, and argument's text:
#define TEST(arg) test_internal((arg), __LINE__, #arg)

// Actual function used to check success/failure:
static void test_internal(_Bool success, int lineNum, char* argStr)
{
	numTests++;
	if (!success) {
		if (current_syscall_test_num != last_syscall_test_num_failed) {
			last_syscall_test_num_failed = current_syscall_test_num;
			num_syscall_tests_failed++;
		}
		printf("-------> ERROR %4d: test on line %d failed: %s\n",
				numTestPassed, lineNum, argStr);
	} else {
		numTestPassed++;
	}
}

static void test_print_summary(void)
{
	printf("\nExecution finished.\n");
	printf("%4d/%d tests passed.\n", numTestPassed, numTests);
	printf("%4d/%d tests FAILED.\n", numTests - numTestPassed, numTests);
	printf("%4d/%d unique syscall testing configurations FAILED.\n", num_syscall_tests_failed, current_syscall_test_num);
	printf("%4d/%d unique simple syscall testing configurations FAILED.\n", num_simple_tests_failed, simple_test_num);
	printf("%4d/%d unique complex syscall testing configurations FAILED.\n", num_complex_tests_failed, complex_test_num);
	int simple_score = ((simple_test_num - num_simple_tests_failed) * 24)/simple_test_num;
	int complex_score = ((complex_test_num - num_complex_tests_failed) * 16)/complex_test_num;
	printf("simple score: %d/24.\n", simple_score);
	printf("complex score: %d/16.\n", complex_score);
	printf("total score: %d/40.\n", simple_score + complex_score);


}


/***********************************************************
 * Routines to double check array answers
 ***********************************************************/
static long find_max(long data[], long size)
{
	long max = data[0];
	for (int i = 0; i < size; i++) {
		if (data[i] > max) {
			max = data[i];
		}
	}
	return max;
}
static long find_min(long data[], long size)
{
	long min = data[0];
	for (int i = 0; i < size; i++) {
		if (data[i] < min) {
			min = data[i];
		}
	}
	return min;
}
static long find_sum(long data[], long size)
{
	long sum = 0;
	for (int i = 0; i < size; i++) {
		sum += data[i];
	}
	return sum;
}

/***********************************************************
 * Functions to actually make the syscall and test results
 ***********************************************************/
static int do_syscall(struct array_stats *stats, long *data, long size)
{
	current_syscall_test_num++;
	printf("\nTest %d: ..Diving to kernel level\n", current_syscall_test_num);
	int result = syscall(SYSCALL_ARRAY_STATS, stats, data, size);
	int my_errno = errno;
	printf("..Rising to user level w/ result = %d", result);
	if (result < 0) {
		printf(", errno = %d", my_errno);
	} else {
		my_errno = 0;
	}
	printf("\n");
	return my_errno;

}
static void do_syscall_working(long data[], long size)
{
	struct array_stats stats;
	int result = do_syscall(&stats, data, size);
	printf("Stats: min = %ld, max = %ld, sum = %ld\n",
			stats.min, stats.max, stats.sum);

	TEST(result == 0);
	TEST(stats.min == find_min(data, size));
	TEST(stats.max == find_max(data, size));
	TEST(stats.sum == find_sum(data, size));
}
static void do_syscall_failing(struct array_stats *stats, long data[], long size, long ret_code)
{
	int result = do_syscall(stats, data, size);
	TEST(result == ret_code);
}

