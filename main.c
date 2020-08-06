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
    int iCheck = 0;
    int rCheck = 0;
    int lCheck = 0;
    char directoryPath[4096];
    char *options;
    int optionChecker;

    getcwd(directoryPath, sizeof(directoryPath));

    // for (int i = 0; i<argc; i++){
    //     printf("length is: %d - %s\n", i, args[i]);
    // }

    // for (int j = 1; j < argc; j++){
    //     printf("args are: %s\n", args[j]);
    //     if (args[j] != "")
    // }


    // if (args[1] == NULL && argc == 1){
    //     // printf("path: %s\n\n", directoryPath);
    //     normalLs(".");

    // }

    while ((optionChecker = getopt(argc, args, "iRl")) != -1)
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
                optionL(".", iCheck, rCheck, lCheck);
            }
        for (; optind < argc; optind++) {
            // printf("The argc is: %s\n", args[optind]);
            // if (!strcmp(args[optind], ".") && strlen(".") == strlen(args[optind])) {
            //     printf("the .\n");
            //     optionL(".", iCheck, rCheck, lCheck);
            // }
            
            // if (!strcmp(args[optind], "..") && strlen("..") == strlen(args[optind])) {
            //     printf("the ..\n");
            //     option(".", iCheck, rCheck, lCheck);
            // }
            // // else if (!strcmp(args[optind], "~") && strlen("~") == strlen(args[optind])) {
            // //     printf("the ~\n");
            // //     struct passwd *pwd = getpwuid(getuid());
            // //     normalLs(pwd->pw_dir);
            // else{
            if (rCheck == 1){
                optionR(args[optind], iCheck, rCheck, lCheck);
            }
            else{
                optionL(args[optind], iCheck, rCheck, lCheck);
            }
            // }
            // }
        }
    // if (!iCheck && !rCheck && !lCheck){
    //     normalLs(directoryPath);
    // }
    
    // printf("%d %d %d\n", iCheck, rCheck, lCheck);
    // printf("%d: \n", optind);

    // if (iCheck == 1 && rCheck ==1 && lCheck ==1){
    //         options = "ilR";
    //         optionR(".", iCheck, rCheck, lCheck);
    // }
    // else if (iCheck == 1 && rCheck == 1){
    //         options = "iR";
    //         optionR(".", iCheck, rCheck, lCheck);
    // }
    // // else if (iCheck == 1 && lCheck == 1){
    // //         options = "il";
    // //         optionL(directoryPath, options);
    // // }
    // else if (rCheck == 1 && lCheck == 1){
    //         options = "lR";
    //         optionR(".", iCheck, rCheck, lCheck);
    // }
    // // else if (iCheck == 1){
    // //         options = "i";
    // //         optionL(directoryPath, options);
    // // }
    // else if (rCheck == 1){
    //         options = "R";
    //         optionR(".", iCheck, rCheck, lCheck);
    // }
    // else if (lCheck == 1){
    //         options = "l";
    //         optionL(directoryPath, options);
    // }
    // if ( argc == 2 && args[1][0]){
    //     if(args[1][1] == 'l' && args[1][2] == NULL){
    //         options = "l";
    //         // printf("%s\n", options);
    //         optionL(directoryPath, options);
    //         // printf("path: %s\n\n", directoryPath);
    //         // normalLs(directoryPath);
    //     }
    //     else if(args[1][1] == 'i' && args[1][2] == NULL){
    //         options = "i";
    //         optionL(directoryPath, options);
    //     }
    //     else if(args[1][1] == 'R' && args[1][2] == NULL){
    //         options = "R";
    //         optionR(".", options);
    //     }
    //     else if((args[1][2] == 'R' && args[1][1] == 'i') || (args[1][2] == 'i' && args[1][1] == 'R')){
    //         options = "iR";
    //         optionR(".", options);
    //     }
    //     else if((args[1][2] == 'R' && args[1][1] == 'l') || (args[1][2] == 'l' && args[1][1] == 'R')){
    //         options = "lR";
    //         optionR(".", options);
    //     }
    //     else if((args[1][2] == 'l' && args[1][1] == 'i') || (args[1][2] == 'i' && args[1][1] == 'l')){
    //         options = "il";
    //         optionL(directoryPath, options);
    //     }
    // }


    return 0;
}