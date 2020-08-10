/**
 * myls.h for Assignment 4, CMPT 300 Summer 2020
 * Name: Devansh Chopra
 * Student #: 301-275-491
 */

#ifndef _MYLS_H_
#define _MYLS_H_
#include <stdbool.h>

int alphasort(const struct dirent **d1, const struct dirent **d2);
int checkHidden(char *name);
void biggestEntry(char *directory);
void printPermissions(__mode_t newMode);
char fileType(__mode_t newMode);
void optionL(char *directory, int iCheck, int lCheck);
void printSingleFile(char *directory, int iCheck, int lCheck);
void print_dir(char *directory, int iCheck, int lCheck);
void optionR(char *directory, int iCheck, int rCheck, int lCheck);

#endif