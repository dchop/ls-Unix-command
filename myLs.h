#ifndef _MYLS_H_
#define _MYLS_H_
#include <stdbool.h>



void normalLs(char *directory);
void biggestEntry(char *directory);
void printPermissions(__mode_t newMode);

void optionL(char *directory, int iCheck, int rCheck, int lCheck);
void optionR(char *directory, int iCheck, int rCheck, int lCheck);;
void sortR(char toSort[]);


#endif