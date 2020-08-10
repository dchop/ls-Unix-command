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

int main(int argc, char* args[])
{
    int iCheck = 0;
    int rCheck = 0;
    int lCheck = 0;
    char directoryPath[4096];
    int checkOpt;
    int countOptions = 0;
    int countArgs = 0;
    struct stat dirStat;

    getcwd(directoryPath, sizeof(directoryPath));

    // char *temp1;
    // char* tempArr[argc];
    // char* toPrintDirsFirst[argc];
    // int j = 0;

    int startFile = -1;
    char *optionArr[argc];
    optionArr[0] = args[0];

    for (int i = 1; i < argc; i++){
        if(args[i][0] == '-' && (args[i][1] == 'l' || args[i][1] == 'R' || args[i][1] == 'i')){
            optionArr[i] = args[i];
            countOptions++;
        }
        else if(args[i][0] == '-' && (args[i][1] != 'l' || args[i][1] != 'R' || args[i][1] != 'i')){
            printf("Error: Option is invalid\n");
        }
        else{
            startFile = i;
            break;
        }
    }

    char *argsArr[argc];
    if(startFile != -1){
        for(int i = 0; startFile < argc; i++, startFile++){
            argsArr[i] = args[startFile];
            countArgs++;
        }
    }

    while ((checkOpt = getopt(countOptions + 1, optionArr, "iRl")) != -1)
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
                return 1;
    }

    if(startFile == -1 && rCheck == 0){
        optionL(".", iCheck, lCheck);
    }

    if (startFile == -1 && rCheck==1){
        optionR(".", iCheck, rCheck, lCheck);
    }

    char *temp;
    for (int i = 0; i < countArgs; i++){
        for(int j = i+1; j < countArgs; j++){
            if(strcmp(argsArr[i], argsArr[j]) > 0){
                temp = argsArr[i];
                argsArr[i] = argsArr[j];
                argsArr[j] = temp;
            }
        }
    }

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
            return 2;
        }
    }

    if (countOfFiles < countArgs){
        printf("\n");
    }

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
            return 2;
        }
    }

    // if (optind == argc && rCheck == 1){
    //     optionR(".", iCheck, rCheck, lCheck);
    // }
    // else if(optind == argc){
    //         optionL(".", iCheck, lCheck);
    //     }
    // for (; optind < argc; optind++) {
    //     if (rCheck == 1){
    //         optionR(args[optind], iCheck, rCheck, lCheck);
    //     }
    //     else{
    //         optionL(args[optind], iCheck, lCheck);
    //     }
    // }

    return 0;
}