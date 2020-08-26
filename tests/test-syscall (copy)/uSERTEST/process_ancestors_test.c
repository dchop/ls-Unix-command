/**
 * process_ancestors_test.c for Assignment 2, CMPT 300 Summer 2020
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
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "process_ancestors_test.h"

// Sys Call number
#define _PROCESS_ANCESTORS_ 550
#define ANCESTOR_NAME_LEN 16

// Helper functions for testing purposes 
static int executeSysCall(struct process_info info_array[], long size, long* num_filled);
static void normalCases();
static void edgeCases();
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

	// Checks a variety of normal cases (different sizes)
	normalCases();

	// Checks a variety of edge cases (bad size, bad pointers, and more than 1 process)
	edgeCases();

	if (numFailed == 0) {
        // We got here?!? PASSED!
        printf("********************************\n");
        printf("          ALL PASSED\n");
        printf("********************************\n\n");
    } 
	else {
        // We got here?!? FAILED!
        printf("********************************\n");
        printf("          TEST(s) FAILED\n");
        printf("********************************\n\n");
		numFailed = 0;
    }
    return 0;
}

// Executes array process_ancestors Sys call based on the inputs given and prints computed values
static int executeSysCall(struct process_info taskArray[], long size, long* num_filled){
	printf("\nDiving to kernel level\n\n");
	int result = syscall(_PROCESS_ANCESTORS_, taskArray, size, num_filled);
	int error = errno;
	printf("\nRising to user level w/ result = %d\n\n", result);

	// Checks if there was an error from the Sys call
	if(result != 0){
		printf("Error code is = %d\n", error);
	}
	else{
		error = 0;
		printf("Idx#	PID	Name			State	UID	#VCSW	#IVCSW	#Child	#Sib\n");
		for(int i=0; i<*num_filled; i++)
		{
			printf("%4d	%4ld	%-20s	%4ld	%ld	%ld	%ld	%ld	%ld\n",
					i, taskArray[i].pid, taskArray[i].name, taskArray[i].state, taskArray[i].uid, 
					taskArray[i].nvcsw, taskArray[i].nivcsw, taskArray[i].num_children, taskArray[i].num_siblings);
		}
	}
	return error;
}

// Checks a variety of normal cases (different sizes)
static void normalCases(){
	printf("__________________NORMAL CASES___________________\n");
	long num_filled;
	long size = 5;
	int code;

	struct process_info taskStruct[size];
	code = executeSysCall(taskStruct, size, &num_filled);
	CHECK(code == 0);

	size = 2;
	struct process_info taskStruct1[size];
	code = executeSysCall(taskStruct1, size, &num_filled);
	CHECK(code == 0);

	size = 40;
	struct process_info taskStruct2[size];
	code = executeSysCall(taskStruct2, size, &num_filled);
	CHECK(code == 0);

	size = 1000;
	struct process_info taskStruct3[size];
	code = executeSysCall(taskStruct3, size, &num_filled);
	CHECK(code == 0);

	size = 1;
	struct process_info taskStruct4[size];
	code = executeSysCall(taskStruct4, size, &num_filled);
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

// Checks a variety of edge cases (bad size, bad pointers, and more than 1 process)
static void edgeCases(){
	printf("__________________EDGE CASES___________________\n");
	long num_filled;
	int code;

	struct process_info *taskStruct;
	code = executeSysCall(taskStruct, 0, &num_filled);
	CHECK(code == EINVAL);

	struct process_info *taskStruct1;
	code = executeSysCall(taskStruct1, -5, &num_filled);
	CHECK(code == EINVAL);

	code = executeSysCall((void*)9, 3, &num_filled);
	CHECK(code == EFAULT);

	code = executeSysCall(NULL, 14, &num_filled);
	CHECK(code == EFAULT);

	struct process_info *taskStruct3;
	code = executeSysCall(taskStruct3, 22, NULL);
	CHECK(code == EFAULT);

	int val = fork();
	int size = 2;
	struct process_info taskStruct4[size];
	if(val == 0){
		code = executeSysCall(taskStruct4, size, &num_filled);
		CHECK(code == 0);
	}

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