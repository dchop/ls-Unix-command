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
    int optionChecker;

    getcwd(directoryPath, sizeof(directoryPath));

    char *temp1;
    char* tempArr[argc];

    for (int i = 0; i < argc; i++){
        if(args[i][0] == '-' && args[i][1] != 'l' && args[i][1] != 'R' && args[i][1] != 'i'){
            temp1 = malloc(strlen(args[i]) + 3);
            strcpy(temp1, args[i]);
            size_t len = strlen("./");
            memmove(temp1 + len, temp1, strlen(temp1) +1);
            memcpy(temp1, "./", len);
            tempArr[i] = temp1;
        }
        else{
            tempArr[i] = args[i];
        }
    }

    while ((optionChecker = getopt(argc, tempArr, "iRl")) != -1)
        switch (optionChecker) {
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
                return 1;
    }

    if (optind == argc && rCheck == 1){
        optionR(".", iCheck, rCheck, lCheck);
    }
    else if(optind == argc){
            optionL(".", iCheck, lCheck);
        }
    for (; optind < argc; optind++) {
        if (rCheck == 1){
            optionR(args[optind], iCheck, rCheck, lCheck);
        }
        else{
            optionL(args[optind], iCheck, lCheck);
        }
    }

    return 0;
}