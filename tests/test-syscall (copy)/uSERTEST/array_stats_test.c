/**
 * array_stats_test.c for Assignment 2, CMPT 300 Summer 2020
 * Name: Devansh Chopra
 * Student #: 301-275-491
 */

// Header files imported
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "array_stats_test.h"

// Sys Call number
#define _ARRAY_STATS_ 549 

// Helper functions for testing purposes 
static int executeSysCall(struct array_stats_s* testStruct, long* data, long size);
static void normalCases();
static void edgeCases();
static void checkerNormal(long dataUser[], struct array_stats_s computed, long size);
int numErrors;
int numFailed;

// Checks conditions
#define CHECK(condition) do{ \
    if (!(condition)) { \
        printf("ERROR: %s (@%d): failed condition \"%s\"\n", __func__, __LINE__, #condition); \
        numErrors++;\
    }\
} while(0)

int main (int argc, char *argv[]){

	// Checks a variety of normal cases (different sizes and different values)
	normalCases();

	// Checks a variety of edge cases (bad values, invalid array sizes, and invalid pointers)
	edgeCases();

	if (numFailed == 0) {
        // We got here?!? PASSED!
        printf("********************************\n");
        printf("          ALL PASSED\n");
        printf("********************************\n\n");
    } else {
        // We got here?!? FAILED!
        printf("********************************\n");
        printf("          TEST(s) FAILED\n");
        printf("********************************\n\n");
		numFailed = 0;
    }
    return 0;
}

// Executes array stats Sys call based on the inputs given and prints computed values
static int executeSysCall(struct array_stats_s* testStruct, long* data, long size){
	printf("\nDiving to kernel level\n\n");
	int result = syscall(_ARRAY_STATS_, testStruct, data, size);
	int error = errno;
	printf("\nRising to user level w/ result = %d\n\n", result);

	// Checks if there was an error from the Sys call
	if(result != 0){
		printf("Error is = %d\n", error);
	}
	else{
		error = 0;
		printf("MAX: %ld MIN: %ld SUM: %ld\n", testStruct->max, testStruct->min, testStruct->sum);
		
		// Making sure computed values are actually correct
		checkerNormal(data, *testStruct, size);
	}
	return error;
}

// Checks if the computed values from the Syscall are actually correct
static void checkerNormal(long dataUser[], struct array_stats_s computed, long size){
	int i;
	long min = LONG_MAX;
	long max = LONG_MIN;
	long sum = 0;
	long val;

	for(i = 0; i < size; i++){
		val = dataUser[i];
		sum += val;
		if(val > max){
			max = val;
		}
		if(val < min){
			min = val;
		}
	}

	CHECK(computed.max == max);
	CHECK(computed.min == min);
	CHECK(computed.sum == sum);
	return;
}

// Checks a variety of normal cases (different sizes and different values)
static void normalCases(){
	printf("__________________NORMAL CASES___________________\n");
	struct array_stats_s testStruct;
	int code;
	long data[] = {1,2,3};
	long data1[] = {8,2,3, 8, 6};
	long data2[] = {100,2,90, 4, 5, 7, 8, 1};
	long data3[] = {100000,-1,-1000000};
	long data4[] = {9};
	long data5[] = {8902,80, 22, 67};
	long data6[] = {10,20,30, 70, 90};
	long data7[] = {5, 5, 5, 5};
	long data8[] = {0, 0, 0};

	code = executeSysCall(&testStruct, data, 3);
	CHECK(code == 0);

	code = executeSysCall(&testStruct, data1, 5);
	CHECK(code == 0);

	code = executeSysCall(&testStruct, data2, 8);
	CHECK(code == 0);

	code = executeSysCall(&testStruct, data3, 3);
	CHECK(code == 0);

	code = executeSysCall(&testStruct, data4, 1);
	CHECK(code == 0);

	code = executeSysCall(&testStruct, data5, 4);
	CHECK(code == 0);

	code = executeSysCall(&testStruct, data6, 5);
	CHECK(code == 0);

	code = executeSysCall(&testStruct, data7, 4);
	CHECK(code == 0);

	code = executeSysCall(&testStruct, data8, 3);
	CHECK(code == 0);

	printf("\nNormal test complete\n");
    if (numErrors == 0) {
        // We got here?!? PASSED!
        printf("********************************\n");
        printf("           PASSED\n");
        printf("********************************\n\n");
    } else {
        // We got here?!? FAILED!
        printf("********************************\n");
        printf("          %d TEST FAILED\n", numErrors);
        printf("********************************\n\n");
		numFailed++;
    }
	numErrors = 0;
}

// Checks a variety of edge cases (bad values, invalid array sizes, and invalid pointers)
static void edgeCases(){
	printf("__________________EDGE CASES___________________\n");
	struct array_stats_s testStruct;
	int code;

	long data[] = {-7000,2,3};
	long data1[] = {0};
	long data3[] = {3,-1,-8};
	long data4[] = {6, 7};
	long data5[] = {9, 3};

	code = executeSysCall(&testStruct, data, 3);
	CHECK(code == 0);

	code = executeSysCall(&testStruct, data1, 0);
	CHECK(code == EINVAL);

	code = executeSysCall(&testStruct, data3, -7);
	CHECK(code == EINVAL);

	code = executeSysCall(&testStruct, data4, 90);
	CHECK(code == 0);

	code = executeSysCall(&testStruct, data5, 1);
	CHECK(code == 0);

	code = executeSysCall((void*)9, data, 3);
	CHECK(code == EFAULT);

	code = executeSysCall(&testStruct, NULL, 7);
	CHECK(code == EFAULT);

	code = executeSysCall(NULL, data4, 2);
	CHECK(code == EFAULT);

	code = executeSysCall(NULL, NULL, 7);
	CHECK(code == EFAULT);

	printf("\nEdge test complete\n");
    if (numErrors == 0) {
        // We got here?!? PASSED!
        printf("********************************\n");
        printf("           PASSED\n");
        printf("********************************\n\n");
    } else {
        // We got here?!? FAILED!
        printf("********************************\n");
        printf("          %d TEST FAILED\n", numErrors);
        printf("********************************\n\n");
		numFailed++;
    }
	numErrors = 0;
}