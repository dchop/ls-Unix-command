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
    char *options;

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
        if(args[1][1] == 'l' && args[1][2] == NULL){
            options = "l";
            // printf("%s\n", options);
            optionL(directoryPath, options);
            // printf("path: %s\n\n", directoryPath);
            // normalLs(directoryPath);
        }
        else if(args[1][1] == 'i' && args[1][2] == NULL){
            options = "i";
            optionL(directoryPath, options);
        }
        else if(args[1][1] == 'R' && args[1][2] == NULL){
            options = "R";
            optionR(".", options);
        }
        else if((args[1][2] == 'R' && args[1][1] == 'i') || (args[1][2] == 'i' && args[1][1] == 'R')){
            options = "iR";
            optionR(".", options);
        }
        else if((args[1][2] == 'R' && args[1][1] == 'l') || (args[1][2] == 'l' && args[1][1] == 'R')){
            options = "lR";
            optionR(".", options);
        }
        else if((args[1][2] == 'l' && args[1][1] == 'i') || (args[1][2] == 'i' && args[1][1] == 'l')){
            options = "il";
            optionL(directoryPath, options);
        }
    }


    return 0;
}