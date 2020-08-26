// Structure to hold values returned by process_ancestors syscall
#ifndef _PROCESS_ANCESTORS_H
#define _PROCESS_ANCESTORS_H


// Syscall numbers
#if _LP64 == 1
	// 64 bit
	#define SYSCALL_CS300_TEST       548
	#define SYSCALL_ARRAY_STATS      549
	#define SYSCALL_PROCESS_ANCESTOR 550
#else
	// 32 bit
	#define SYSCALL_CS300_TEST       439
	#define SYSCALL_ARRAY_STATS      440
	#define SYSCALL_PROCESS_ANCESTOR 441
#endif


#define ANCESTOR_NAME_LEN 16
struct process_info {
	long pid;                     /* Process ID */
	char name[ANCESTOR_NAME_LEN]; /* Name of process */
	long state;                   /* Current process state */
	long uid;                     /* User ID of process */
	long nvcsw;                   /* # voluntary context switches */
	long nivcsw;                  /* # involuntary context switches */
	long num_children;            /* # children processes */
	long num_siblings;            /* # sibling processes */
};

#endif
