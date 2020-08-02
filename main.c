#include <stdio.h>
#include <stdlib.h>	
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>	// Directory operations
#include <ftw.h>	// file-tree walk
#include <sys/types.h>
#include <pwd.h>	// For User id
#include <grp.h>	// For Group id
#include "myLs.h"

int main(int argc, char* args[])
{

    char directoryPath[4096];

    getcwd(directoryPath, sizeof(directoryPath));

    // for (int i = 0; i<argc; i++){
    //     printf("length is: %d - %s\n", i, args[i]);
    // }

    // for (int j = 1; j < argc; j++){
    //     printf("args are: %s\n", args[j]);
    //     if (args[j] != "")
    // }


    if (args[1] == NULL && argc == 1){
        // printf("path: %s\n\n", directoryPath);
        normalLs(directoryPath);

    }

    if ( argc == 2 && args[1][0]){
        if(args[1][1] == 'l'){
        // printf("HI");
        optionLi(directoryPath);
        // printf("path: %s\n\n", directoryPath);
        // normalLs(directoryPath);
        }
    }


    return 0;
}