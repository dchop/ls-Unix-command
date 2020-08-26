// Define the array_stats struct for the array_stats syscall.
#ifndef _ARRAY_STATS_H_
#define _ARRAY_STATS_H_


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

struct array_stats{
	long min;
	long max;
	long sum;
};

#endif
