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
