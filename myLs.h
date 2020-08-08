#ifndef _MYLS_H_
#define _MYLS_H_
#include <stdbool.h>

int checkHidden(char *name);
void biggestEntry(char *directory);
void printPermissions(__mode_t newMode);
void *fileType(__mode_t newMode);
void optionL(char *directory, int iCheck, int rCheck, int lCheck);
void printSingleFile(char *directory, int iCheck, int rCheck, int lCheck);
void print_dir(char *directory, int iCheck, int rCheck, int lCheck);
void optionR(char *directory, int iCheck, int rCheck, int lCheck);

#endif