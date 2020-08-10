/**
 * main.c for Assignment 4, CMPT 300 Summer 2020
 * Name: Devansh Chopra
 * Student #: 301-275-491
 */

// Header files imported
#include <stdio.h>
#include <stdlib.h>	
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <ftw.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include "myLs.h"

// Heloper sort function
char *sortingFunction(char *Arr[], int length){
    char *temp;
    for (int i = 0; i < length; i++){
        for(int j = i+1; j < length; j++){
            if(strcmp(Arr[i], Arr[j]) > 0){
                temp = Arr[i];
                Arr[i] = Arr[j];
                Arr[j] = temp;
            }
        }
    }
    return *Arr;
}

int main(int argc, char* args[])
{
    // Helper variables
    int iCheck = 0;
    int rCheck = 0;
    int lCheck = 0;
    char *optionArr[argc];
    optionArr[0] = args[0];
    char directoryPath[4096];
    int checkOpt;
    int countOptions = 0;
    int countArgs = 0;
    struct stat dirStat;

    getcwd(directoryPath, sizeof(directoryPath));

    int startFile = -1;

    // Creating the options array
    for (int i = 1; i < argc; i++){
        if(args[i][0] == '-' && (args[i][1] == 'l' || args[i][1] == 'R' || args[i][1] == 'i')){
            optionArr[i] = args[i];
            countOptions++;
        }
        else if(args[i][0] == '-' && (args[i][1] != 'l' || args[i][1] != 'R' || args[i][1] != 'i')){
            printf("Error: Option is invalid\n");
            exit(1);
        }
        else{
            startFile = i;
            break;
        }
    }

    // Creating the arguments array
    char *argsArr[argc];
    if(startFile != -1){
        for(int j = 0; startFile < argc; j++, startFile++){
            argsArr[j] = args[startFile];
            countArgs++;
        }
    }

    // Checking for the options in the user input
    while ((checkOpt = getopt(countOptions+1, optionArr, "iRl")) != -1)
        switch (checkOpt) {
            case 'i':
                iCheck = 1;
                break;
            case 'R':
                rCheck = 1;
                break;
            case 'l':
                lCheck = 1;
                break;
            default:
                printf("Error: Option is invalid");
                exit(1);
    }

    // Special paths
    if(startFile == -1 && rCheck == 0){
        optionL(".", iCheck, lCheck);
    }

    // Special paths
    if (startFile == -1 && rCheck==1){
        optionR(".", iCheck, rCheck, lCheck);
    }

    char *temp;
    // Sorting the input arguments
    for (int i = 0; i < countArgs; i++){
        for(int j = i+1; j < countArgs; j++){
            if(strcmp(argsArr[i], argsArr[j]) > 0){
                temp = argsArr[i];
                argsArr[i] = argsArr[j];
                argsArr[j] = temp;
            }
        }
    }

    // Printing the files first
    int countOfFiles = 0;
    for (int i = 0; i < countArgs; i++){
        int t = lstat(argsArr[i], &dirStat);
        if (t != -1){
            if(!(S_ISDIR(dirStat.st_mode))){
                optionL(argsArr[i], iCheck, lCheck);
                countOfFiles++;
            }
        }
        else{
            printf("Error: invalid access\n");
            exit(1);
        }
    }

    if (countOfFiles < countArgs){
        printf("\n");
    }

    // Printing the dirs second
    for (int i = 0; i < countArgs; i++){
        int t = lstat(argsArr[i], &dirStat);
        if (t != -1){
            if(S_ISDIR(dirStat.st_mode)){
                if(rCheck == 1){
                    optionR(argsArr[i], iCheck, rCheck ,lCheck);
                        if( i +1 != countArgs){
                            printf("\n");
                        }
                }
                else{
                        // printf("\n");
                        // Checking for special characters
                        if(strstr(argsArr[i], " ") || strstr(argsArr[i], "!") || strstr(argsArr[i], "$") || strstr(argsArr[i], ",") || strstr(argsArr[i], "^") || strstr(argsArr[i], "&") || strstr(argsArr[i], "(") || strstr(argsArr[i], ")")){
                            printf("\'%s\': \n", argsArr[i]);
                        }
                        else{
                            printf("%s: \n", argsArr[i]);
                        }
                        print_dir(argsArr[i], iCheck, lCheck);
                        if( i +1 != countArgs){
                            printf("\n");
                        }
                    
                }
            }
        }
        else{
            printf("Error: invalid access\n");
            exit(1);
        }
    }

    return 0;
}