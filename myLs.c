/**
 * myLs.c for Assignment 4, CMPT 300 Summer 2020
 * Name: Devansh Chopra
 * Student #: 301-275-491
 */

// Header files imported
#include <stdio.h>
#include <stdlib.h>	
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>
#include <dirent.h>
#include <ftw.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <libgen.h>
#include "myLs.h"

// Built in alphasort to be used in scandir
int alphasort(const struct dirent **d1, const struct dirent **d2);

// Helper variables
static int width1 = 0;
static int width2 = 0;
static int maxNodeCount = 0;

// Function that checks for all hidden files
int checkHidden(char *name){
    if(!strcmp(name, ".") || !strcmp(name, ".")){
        return 1;
    }
    else if(strlen(name) && name[0] == '.'){
        return 2;
    }
    else{
        return 0;
    }
}

// Function that sets the formatting widths for printing
void biggestEntry(char *directory){

    DIR *dir;
    int maxCount = 0;
    int maxScount = 0;
    char newSize[50];
    char maxNode[50] = {'\0'};
    char newStringSize[50] = {'\0'};
    struct dirent * entry;
    struct stat allStat;
    char wholeDir[4096] = {'\0'};

    width1 = 0;
    width2 = 0;
    maxNodeCount = 0;

    dir = opendir(directory);

    if(dir != NULL){
        // Reading the contents of a directory
        while (entry = readdir(dir)){
            if (checkHidden(entry->d_name) == 1 || checkHidden(entry->d_name) == 2){
                continue;
            }
            int counter = 0;
            int currNodeCount = 0;
            int stringCount = 0;
            memset(newSize, 0, sizeof newSize);
            strcat(wholeDir, directory);
            strcat(wholeDir, "/");
            strcat(wholeDir, entry->d_name);
            lstat(wholeDir, &allStat);
            sprintf(newSize, "%ld", allStat.st_size);
            sprintf(maxNode, "%ld", allStat.st_ino);
            for (size_t i = 0; i < strlen(maxNode); i++){
                currNodeCount++;
                if(maxNodeCount < currNodeCount){
                    maxNodeCount = currNodeCount;
                }
            }
            for(size_t i = 0; i< strlen(newSize); i++){
                counter++;
                if (maxCount < counter){
                    maxCount = counter;
                }
            }
            sprintf(newStringSize, "%ld", allStat.st_nlink);
            for(size_t j = 0; j < strlen(newStringSize); j++){
                stringCount++;
                if(maxScount < stringCount){
                    maxScount = stringCount;
                }
            }
            strcpy(wholeDir, "");
        }
        closedir(dir);
    }

    // Setting spacing widths
    if(maxScount > width1){
        width1 = maxScount;
    }
    if(maxCount > width2){
        width2 = maxCount;
    }
    
    return;
}

// Function for printing the permissions for a file/directory
void printPermissions(__mode_t newMode){
    // Permisstions for user
    if ((newMode & S_IRUSR) == S_IRUSR){
        printf("r");
    }
    else {
        printf("-");
        }
    if ((newMode & S_IWUSR) == S_IWUSR){
        printf("w");
    }
    else {printf("-");}
    if ((newMode & S_IXUSR) == S_IXUSR){
        printf("x");
    }
    else {printf("-");}
    // Permissions for group
    if ((newMode & S_IRGRP) == S_IRGRP){
        printf("r");
    }
    else {printf("-");}
    if ((newMode & S_IWGRP) == S_IWGRP){
        printf("w");
    }
    else {printf("-");}
    if ((newMode & S_IXGRP) == S_IXGRP){
        printf("x");
    }
    else {printf("-");}
    // Permissions for other
    if ((newMode & S_IROTH) == S_IROTH){
        printf("r");
    }
    else {printf("-");}
    if ((newMode & S_IWOTH) == S_IWOTH){
        printf("w");
    }
    else {printf("-");}
    if ((newMode & S_IXOTH) == S_IXOTH){
        printf("x");
    }
    else {printf("-");}
}

// Function for printing the file type of a file
char fileType(__mode_t newMode){
    switch(newMode & S_IFMT){
        case S_IFCHR:
            printf("c");
            return 'c';
        case S_IFBLK:
            printf("b");
            return 'b';
        case S_IFIFO:
            printf("p");
            return 'p';
        case S_IFDIR:
            printf("d");
            return 'd';
        case S_IFLNK:
            printf("l");
            return 'l';
        case S_IFSOCK:
            printf("s");
            return 's';
        case S_IFREG:
            printf("-");
            return '-';
        default:
            printf("?");
            return '?';
    }
}

// Function that takes care of ., .. and ~ directories
void optionL(char *directory, int iCheck, int lCheck){

    // Local variables
    char temp[4096] = {'\0'};
    char timeBuff[50] = {'\0'};
    struct tm *newTime;
    time_t curTimer;
    struct group *grp;
    struct passwd *pwd;
    struct stat curStat;
    pwd = getpwuid(geteuid());
    biggestEntry(directory);
    char symbolic_link[1024] = {'\0'};
    struct dirent **namelist;
    int n = 0;
    n = scandir(directory, &namelist, 0, alphasort);

        // if(n != -1){
        //     if(strstr(directory, " ") || strstr(directory, "!") || strstr(directory, "$") || strstr(directory, ",") || strstr(directory, "^") || strstr(directory, "&") || strstr(directory, "(") || strstr(directory, ")")){
        //         printf("\'%s\': \n", directory);
        //     }
        //     else{
        //     printf("%s: \n", directory);
        //     }
        // }
        
    for (int j = 0; j<n; j++){
        // Skipping the hidden files
        if (checkHidden(namelist[j]->d_name) == 1 || checkHidden(namelist[j]->d_name) == 2){
            continue;
        }
        char stub[4096];
        strcpy(stub, directory);
        if(strcmp("/", directory)){
            strcat(stub, "/");}
        strcat(stub, namelist[j]->d_name);
        strcat(stub, "\0");

        if(lstat(stub, &curStat)==-1){
            printf("Error in print_dir on file: [%s]\n", stub);
            printf("%s\n", strerror(errno));
            return 1;
        }

        // Creating the time buffer
        curTimer = curStat.st_mtime;
        newTime = localtime(&curTimer);
        strftime(timeBuff, 50, "%b %2d %4Y %2H:%2M", newTime);
        if (timeBuff[4] == '0'){
            timeBuff[4] = ' ';
        }
        grp = getgrgid(curStat.st_gid);

        // When the options are -i and -l
        if (iCheck == 1 && lCheck == 1){
            printf("%*llu ", maxNodeCount, curStat.st_ino);
            char k = fileType(curStat.st_mode);
            // Checking for special characters
            if (strstr(namelist[j]->d_name, " ") || strstr(namelist[j]->d_name, "!") || strstr(namelist[j]->d_name, "$") || strstr(namelist[j]->d_name, ",") || strstr(namelist[j]->d_name, "^") || strstr(namelist[j]->d_name, "&") || strstr(namelist[j]->d_name, "(") || strstr(namelist[j]->d_name, ")")){
                // Checking for symlinks
                if( k == 'l'){
                    readlink(stub, symbolic_link, 1024);
                    printPermissions(curStat.st_mode);
                    if (strstr(symbolic_link, " ") || strstr(symbolic_link, "!") || strstr(symbolic_link, "$") || strstr(symbolic_link, ",") || strstr(symbolic_link, "^") || strstr(symbolic_link, "&") || strstr(symbolic_link, "(") || strstr(symbolic_link, ")")){
                        printf(" %*ld %s %s %*ld %s \'%s\' -> \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name, symbolic_link);
                        strcpy(temp, "");  
                    }
                    else{ 
                        printf(" %*ld %s %s %*ld %s \'%s\' -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name, symbolic_link);
                        strcpy(temp, "");
                    }
                }
                else{
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name);
                    strcpy(temp, "");
                }
            }
            else{
                // Checking for symlinks
                if( k == 'l'){
                    readlink(stub, symbolic_link, 1024);
                    printPermissions(curStat.st_mode);
                    // Checking for special characters
                    if (strstr(symbolic_link, " ") || strstr(symbolic_link, "!") || strstr(symbolic_link, "$") || strstr(symbolic_link, ",") || strstr(symbolic_link, "^") || strstr(symbolic_link, "&") || strstr(symbolic_link, "(") || strstr(symbolic_link, ")")){
                        printf(" %*ld %s %s %*ld %s %s -> \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name, symbolic_link);
                        strcpy(temp, "");
                    }
                    else{
                        printf(" %*ld %s %s %*ld %s %s -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name, symbolic_link);
                        strcpy(temp, "");
                    }
                }
                else{
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name);
                    strcpy(temp, "");
                }
            }
        }
        // When the option is -i
        else if (iCheck == 1){
            // Checking for special characters
            if (strstr(namelist[j]->d_name, " ") || strstr(namelist[j]->d_name, "!") || strstr(namelist[j]->d_name, "$") || strstr(namelist[j]->d_name, ",") || strstr(namelist[j]->d_name, "^") || strstr(namelist[j]->d_name, "&") || strstr(namelist[j]->d_name, "(") || strstr(namelist[j]->d_name, ")")){
                printf("%*llu \'%s\'  \n", maxNodeCount, curStat.st_ino, namelist[j]->d_name);
                strcpy(temp, "");
            }
            else{
                printf("%*ld %s  \n", maxNodeCount, curStat.st_ino, namelist[j]->d_name);
                strcpy(temp, "");
            }
        }
        // When the option is -l
        else if (lCheck == 1){
            char k = fileType(curStat.st_mode);
            if (strstr(namelist[j]->d_name, " ") || strstr(namelist[j]->d_name, "!") || strstr(namelist[j]->d_name, "$") || strstr(namelist[j]->d_name, ",") || strstr(namelist[j]->d_name, "^") || strstr(namelist[j]->d_name, "&") || strstr(namelist[j]->d_name, "(") || strstr(namelist[j]->d_name, ")")){
                // Checking for symlinks
                if (k == 'l'){ 
                    readlink(stub, symbolic_link, 1024);                                          
                    printPermissions(curStat.st_mode);
                    // Checking for special characters
                    if(strstr(symbolic_link, " ") || strstr(symbolic_link, "!") || strstr(symbolic_link, "$") || strstr(symbolic_link, ",") || strstr(symbolic_link, "^") || strstr(symbolic_link, "&") || strstr(symbolic_link, "(") || strstr(symbolic_link, ")")){
                        printf(" %*ld %s %s %*ld %s \'%s\' -> \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name, symbolic_link);
                        strcpy(temp, ""); 
                    }
                    else{
                        printf(" %*ld %s %s %*ld %s \'%s\' -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name, symbolic_link);
                        strcpy(temp, ""); 
                    }
                }
                else{
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name);
                    strcpy(temp, "");
                }
            }
            else{
                // Checking for symlinks
                if (k == 'l'){
                    readlink(stub, symbolic_link, 1024);
                    printPermissions(curStat.st_mode);
                    // Checking for special characters
                    if(strstr(symbolic_link, " ") || strstr(symbolic_link, "!") || strstr(symbolic_link, "$") || strstr(symbolic_link, ",") || strstr(symbolic_link, "^") || strstr(symbolic_link, "&") || strstr(symbolic_link, "(") || strstr(symbolic_link, ")")){
                        printf(" %*ld %s %s %*ld %s %s -> \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name, symbolic_link);
                        strcpy(temp, "");
                    }
                    else{
                        printf(" %*ld %s %s %*ld %s %s -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name, symbolic_link);
                        strcpy(temp, "");
                    }
                }
                else{
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name);
                    strcpy(temp, "");
                }
            }
        }
        else{
            // Checking for special characters
            if (strstr(namelist[j]->d_name, " ") || strstr(namelist[j]->d_name, "!") || strstr(namelist[j]->d_name, "$") || strstr(namelist[j]->d_name, ",") || strstr(namelist[j]->d_name, "^") || strstr(namelist[j]->d_name, "&") || strstr(namelist[j]->d_name, "(") || strstr(namelist[j]->d_name, ")")){
                printf("\'%s\'\n", namelist[j]->d_name);
            }
            else{
            printf("%s\n", namelist[j]->d_name);
            }
        }
    }
    if ( n!=-1){
            // printf("\n");
            free(namelist);
    }
    
    if (n == -1){
        printSingleFile(directory, iCheck, lCheck);
    }      
} 

// Function for printing a single file and its meta data
void printSingleFile(char *directory, int iCheck, int lCheck){

    // Local variables
    char temp[4096];
    char timeBuff[50];
    struct tm *newTime;
    time_t curTimer;
    struct group *grp;
    struct passwd *pwd;
    struct stat curStat;
    pwd = getpwuid(geteuid());
    biggestEntry(directory);
    char symbolic_link[1024] = {'\0'};

    if (checkHidden(directory) == 1 || checkHidden(directory) == 2){
        return;
    }

    char stub[4096];
    strcpy(stub, directory);
    strcat(stub, "\0");

    if(lstat(stub, &curStat)==-1){
        printf("Error in print_dir on file: [%s]\n", stub);
        printf("%s\n", strerror(errno));
        return 1;
    }

    // Setting up time buffer
    curTimer = curStat.st_mtime;
    newTime = localtime(&curTimer);
    strftime(timeBuff, 50, "%b %2d %4Y %2H:%2M", newTime);
    if (timeBuff[4] == '0'){
        timeBuff[4] = ' ';
    }
    grp = getgrgid(curStat.st_gid);

    // When the options are -i and -l
    if (iCheck == 1 && lCheck == 1){
        printf("%*llu ", maxNodeCount, curStat.st_ino);
        char k = fileType(curStat.st_mode);
        // Checking for special characters
        if(strstr(directory, " ") || strstr(directory, "!") || strstr(directory, "$") || strstr(directory, ",") || strstr(directory, "^") || strstr(directory, "&") || strstr(directory, "(") || strstr(directory, ")")){
            // Checking for symlinks
            if(k == 'l'){
                readlink(stub, symbolic_link, 1024);
                printPermissions(curStat.st_mode);
                // Checking for special characters
                if (strstr(symbolic_link, " ") || strstr(symbolic_link, "!") || strstr(symbolic_link, "$") || strstr(symbolic_link, ",") || strstr(symbolic_link, "^") || strstr(symbolic_link, "&") || strstr(symbolic_link, "(") || strstr(symbolic_link, ")")){
                    printf(" %*ld %s %s %*ld %s \'%s\' -> \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory, symbolic_link);
                    strcpy(temp, "");
                }
                else{
                    printf(" %*ld %s %s %*ld %s \'%s\' -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory, symbolic_link);
                    strcpy(temp, "");
                }

            }
            else {
                printPermissions(curStat.st_mode);
                printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory);
                strcpy(temp, "");
            }
        }
        else{
            // Checking for symlinks
            if(k == 'l'){
                readlink(stub, symbolic_link, 1024);
                printPermissions(curStat.st_mode);
                // Checking for special characters
                if (strstr(symbolic_link, " ") || strstr(symbolic_link, "!") || strstr(symbolic_link, "$") || strstr(symbolic_link, ",") || strstr(symbolic_link, "^") || strstr(symbolic_link, "&") || strstr(symbolic_link, "(") || strstr(symbolic_link, ")")){
                    printf(" %*ld %s %s %*ld %s %s -> \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory, symbolic_link);
                    strcpy(temp, "");
                }
                else{
                    printf(" %*ld %s %s %*ld %s %s -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory, symbolic_link);
                    strcpy(temp, "");
                }

            }
            else {
                printPermissions(curStat.st_mode);
                printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory);
                strcpy(temp, "");
            } 
        }
    }
    // When the option is -i
    else if (iCheck == 1){
        // Checking for special characters
        if(strstr(directory, " ") || strstr(directory, "!") || strstr(directory, "$") || strstr(directory, ",") || strstr(directory, "^") || strstr(directory, "&") || strstr(directory, "(") || strstr(directory, ")")){
            printf("%*llu \'%s\'  \n", maxNodeCount, curStat.st_ino, directory);
            strcpy(temp, "");
        }
        else{
            printf("%*ld %s  \n", maxNodeCount, curStat.st_ino, directory);
            strcpy(temp, "");
        }   
    }
    // When the option is -l
    else if (lCheck == 1){
        char k = fileType(curStat.st_mode);
        // Checking for special characters
        if(strstr(directory, " ") || strstr(directory, "!") || strstr(directory, "$") || strstr(directory, ",") || strstr(directory, "^") || strstr(directory, "&") || strstr(directory, "(") || strstr(directory, ")")){
            // Checking for symlinks
            if(k == 'l'){
                readlink(stub, symbolic_link, 1024);
                printPermissions(curStat.st_mode);
                // Checking for special characters
                    if (strstr(symbolic_link, " ") || strstr(symbolic_link, "!") || strstr(symbolic_link, "$") || strstr(symbolic_link, ",") || strstr(symbolic_link, "^") || strstr(symbolic_link, "&") || strstr(symbolic_link, "(") || strstr(symbolic_link, ")")){
                    printf(" %*ld %s %s %*ld %s \'%s\' -> \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory, symbolic_link);
                    strcpy(temp, "");
                    }
                    else{
                    printf(" %*ld %s %s %*ld %s \'%s\' -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory, symbolic_link);
                    strcpy(temp, "");
                    }
            }
            else {
                printPermissions(curStat.st_mode);
                printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory);
                strcpy(temp, "");
            }
        }
        else{
            // Checking for symlinks
            if(k == 'l'){
                readlink(stub, symbolic_link, 1024);
                printPermissions(curStat.st_mode);
                // Checking for special characters
                if (strstr(symbolic_link, " ") || strstr(symbolic_link, "!") || strstr(symbolic_link, "$") || strstr(symbolic_link, ",") || strstr(symbolic_link, "^") || strstr(symbolic_link, "&") || strstr(symbolic_link, "(") || strstr(symbolic_link, ")")){
                    printf(" %*ld %s %s %*ld %s %s -> \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory, symbolic_link);
                    strcpy(temp, "");
                }
                else{
                    printf(" %*ld %s %s %*ld %s %s -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory, symbolic_link);
                    strcpy(temp, "");
                }
            }
            else {
                printPermissions(curStat.st_mode);
                printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory);
                strcpy(temp, "");
            } 
        }
    }
    else{
        // Checking for special characters
        if (strstr(directory, " ") || strstr(directory, "!") || strstr(directory, "$") || strstr(directory, ",") || strstr(directory, "^") || strstr(directory, "&") || strstr(directory, "(") || strstr(directory, ")")){
            printf("\'%s\'\n", directory);
        }
        else{
            printf("%s\n", directory);
        }
    }
}

// Function for printing contents of a directory
void print_dir(char *directory, int iCheck, int lCheck){

    // Local variables
    struct stat curStat;
    struct dirent **namelist;
    struct stat dirStat;
    int n = 0;
    struct tm *newTime;
    char temp[4096];
    time_t curTimer;
    char timeBuff[50];
    struct group *grp;
    struct passwd *pwd;
    char symbolic_link[1024] = {'\0'};
    pwd = getpwuid(getuid());
    biggestEntry(directory);
    n = scandir(directory, &namelist, NULL, alphasort);

    if (n < 0){
        if(lstat(directory, &dirStat) == -1){
            printf("Error in print_dir on file");
        }
        else{
            printf("%lu\t%s\n ", dirStat.st_size, directory);
        }
    }
    else{
        for (int i = 0; i < n; i++){
            // Skip hidden files
            if(!(strcmp(".", namelist[i]->d_name) && strcmp("..",namelist[i]->d_name))){ 
                free(namelist[i]);
                continue;
            }
            if (checkHidden(namelist[i]->d_name) == 1 || checkHidden(namelist[i]->d_name) == 2){
                    free(namelist[i]);
                    continue;
            }

            char stub[4096];
            strcpy(stub, directory);
            if(strcmp("/", directory)){
                strcat(stub, "/");
            }
            strcat(stub, namelist[i]->d_name);
            strcat(stub, "\0");

            if(lstat(stub, &curStat)==-1){
                printf("Error in print_dir on file: [%s]\n", stub);
                printf("%s\n", strerror(errno));
                return 1;
            }

            curTimer = curStat.st_mtime;
            newTime = localtime(&curTimer);
            strftime(timeBuff, 50, "%b %2d %4Y %2H:%2M", newTime);
            if (timeBuff[4] == '0'){
                timeBuff[4] = ' ';
            }
            grp = getgrgid(curStat.st_gid);

            // When the options are -i and -l
            if (iCheck == 1 && lCheck == 1){
                printf("%*llu ", maxNodeCount, curStat.st_ino);
                char k = fileType(curStat.st_mode);
                // Checking for special characters
                if (strstr(namelist[i]->d_name, " ") || strstr(namelist[i]->d_name, "!") || strstr(namelist[i]->d_name, "$") || strstr(namelist[i]->d_name, ",") || strstr(namelist[i]->d_name, "^") || strstr(namelist[i]->d_name, "&") || strstr(namelist[i]->d_name, "(") || strstr(namelist[i]->d_name, ")")){
                    // Checking for symlinks
                    if( k == 'l'){
                        readlink(stub, symbolic_link, 1024);
                        printPermissions(curStat.st_mode);
                        // Checking for special characters
                        if (strstr(symbolic_link, " ") || strstr(symbolic_link, "!") || strstr(symbolic_link, "$") || strstr(symbolic_link, ",") || strstr(symbolic_link, "^") || strstr(symbolic_link, "&") || strstr(symbolic_link, "(") || strstr(symbolic_link, ")")){
                            printf(" %*ld %s %s %*ld %s \'%s\' -> \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name, symbolic_link);
                            strcpy(temp, "");
                        }
                        else{
                            printf(" %*ld %s %s %*ld %s \'%s\' -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name, symbolic_link);
                            strcpy(temp, "");
                        }
                    }
                    else{
                        printPermissions(curStat.st_mode);
                        printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name);
                        strcpy(temp, "");
                    }
                }
                else{
                    // Checking for symlinks
                    if( k == 'l'){
                        readlink(stub, symbolic_link, 1024);
                        printPermissions(curStat.st_mode);
                        // Checking for special characters
                        if (strstr(symbolic_link, " ") || strstr(symbolic_link, "!") || strstr(symbolic_link, "$") || strstr(symbolic_link, ",") || strstr(symbolic_link, "^") || strstr(symbolic_link, "&") || strstr(symbolic_link, "(") || strstr(symbolic_link, ")")){
                            printf(" %*ld %s %s %*ld %s %s -> \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name, symbolic_link);
                            strcpy(temp, "");
                        }
                        else{
                            printf(" %*ld %s %s %*ld %s %s -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name, symbolic_link);
                            strcpy(temp, "");
                        }
                    }
                    else{
                        printPermissions(curStat.st_mode);
                        printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name);
                        strcpy(temp, "");
                    }
                }
            }
            // When the option is -i
            else if (iCheck == 1){
                // Checking for special characters
                if (strstr(namelist[i]->d_name, " ") || strstr(namelist[i]->d_name, "!") || strstr(namelist[i]->d_name, "$") || strstr(namelist[i]->d_name, ",") || strstr(namelist[i]->d_name, "^") || strstr(namelist[i]->d_name, "&") || strstr(namelist[i]->d_name, "(") || strstr(namelist[i]->d_name, ")")){
                    printf("%*llu \'%s\'  \n", maxNodeCount, curStat.st_ino, namelist[i]->d_name);
                    strcpy(temp, "");
                }
                else{
                    printf("%*ld %s  \n", maxNodeCount, curStat.st_ino, namelist[i]->d_name);
                    strcpy(temp, "");
                }
            }
            // When the option is -l
            else if (lCheck == 1){
                char k = fileType(curStat.st_mode);
                if (strstr(namelist[i]->d_name, " ") || strstr(namelist[i]->d_name, "!") || strstr(namelist[i]->d_name, "$") || strstr(namelist[i]->d_name, ",") || strstr(namelist[i]->d_name, "^") || strstr(namelist[i]->d_name, "&") || strstr(namelist[i]->d_name, "(") || strstr(namelist[i]->d_name, ")")){
                    // Checking for symlinks
                    if (k == 'l'){ 
                        readlink(stub, symbolic_link, 1024);                                          
                        printPermissions(curStat.st_mode);
                        // Checking for special characters
                        if (strstr(symbolic_link, " ") || strstr(symbolic_link, "!") || strstr(symbolic_link, "$") || strstr(symbolic_link, ",") || strstr(symbolic_link, "^") || strstr(symbolic_link, "&") || strstr(symbolic_link, "(") || strstr(symbolic_link, ")")){
                            printf(" %*ld %s %s %*ld %s \'%s\' -> \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name, symbolic_link);
                            strcpy(temp, ""); 
                        }
                        else{
                            printf(" %*ld %s %s %*ld %s \'%s\' -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name, symbolic_link);
                            strcpy(temp, ""); 
                        }
                    }
                    else{
                        printPermissions(curStat.st_mode);
                        printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name);
                        strcpy(temp, "");
                    }
                }
                else{
                    // Checking for symlinks
                    if (k == 'l'){
                        readlink(stub, symbolic_link, 1024);
                        printPermissions(curStat.st_mode);
                        // Checking for special characters
                        if (strstr(symbolic_link, " ") || strstr(symbolic_link, "!") || strstr(symbolic_link, "$") || strstr(symbolic_link, ",") || strstr(symbolic_link, "^") || strstr(symbolic_link, "&") || strstr(symbolic_link, "(") || strstr(symbolic_link, ")")){
                            printf(" %*ld %s %s %*ld %s %s -> \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name, symbolic_link);
                            strcpy(temp, "");
                        }
                        else{
                            printf(" %*ld %s %s %*ld %s %s -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name, symbolic_link);
                            strcpy(temp, "");
                        }
                    }
                    else{
                        printPermissions(curStat.st_mode);
                        printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name);
                        strcpy(temp, "");
                    }
                }
            }
            else{
                // Checking for special characters
                if (strstr(namelist[i]->d_name, " ") || strstr(namelist[i]->d_name, "!") || strstr(namelist[i]->d_name, "$") || strstr(namelist[i]->d_name, ",") || strstr(namelist[i]->d_name, "^") || strstr(namelist[i]->d_name, "&") || strstr(namelist[i]->d_name, "(") || strstr(namelist[i]->d_name, ")")){
                    printf("\'%s\'\n", namelist[i]->d_name);
                }
                else{
                printf("%s\n", namelist[i]->d_name);
                }
            }
            free(namelist[i]);
        }
    }

    if(n != -1){
        free(namelist);
    }
}

// Function for printing all directories recursively
void optionR(char *directory, int iCheck, int rCheck, int lCheck){
    
    // Checking for special characters
    if (strstr(directory, " ") || strstr(directory, "!") || strstr(directory, "$") || strstr(directory, ",") || strstr(directory, "^") || strstr(directory, "&") || strstr(directory, "(") || strstr(directory, ")")){
        printf("\'%s\': \n", directory);
    }
    else{
        printf("%s: \n", directory);
    }

    print_dir(directory, iCheck, lCheck);
    struct dirent **namelist;
    struct stat stat_struct;
    int n = 0;
    n = scandir(directory, &namelist, NULL, alphasort);
    
    if (n < 0){
        if(lstat(directory, &stat_struct) == -1){
            printf("Error in print_dir on file");
        }
        else{
            printf("%lu\t%s\n ", stat_struct.st_size, directory);
        }
    }

    for (int i = 0; i < n; i++){
        // Skipping files
        if(!(strcmp(".", namelist[i]->d_name) && strcmp("..",namelist[i]->d_name))){ 
            free(namelist[i]);
            continue;
        }
        // Skipping files
        if (checkHidden(namelist[i]->d_name) == 1 || checkHidden(namelist[i]->d_name) == 2){
                free(namelist[i]);
                continue;
            }

        char stub[4096];
        strcpy(stub, directory);

        if(strcmp("/", directory)){
            strcat(stub, "/");
        }
        strcat(stub, namelist[i]->d_name);
        strcat(stub, "\0");

        if(lstat(stub, &stat_struct)==-1){
            printf("Error in print_dir on file: [%s]\n", stub);
            printf("%s\n", strerror(errno));
            free(namelist[i]);
            return 1;
        }
        
        // Check if its a directory and do recursion
        if(S_ISDIR(stat_struct.st_mode)){
            printf("\n");
            optionR(stub, iCheck, rCheck, lCheck);
        }
        free(namelist[i]);
    }

    if (n != 1){
     free(namelist);
    }
}   
