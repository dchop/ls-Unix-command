#include <stdio.h>
#include <stdlib.h>	
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>
#include <dirent.h>	// Directory operations
#include <ftw.h>	// file-tree walk
#include <sys/types.h>
#include <pwd.h>	// For User id
#include <grp.h>	// For Group id
#include <errno.h>
#include <libgen.h>     // basename()

#include "myLs.h"

int alphasort_case_insensitive(const struct dirent ** a, const struct dirent **b) 
{
    return(strcasecmp((*(const struct dirent **)a)->d_name,
                (*(const struct dirent **)b)->d_name));
}
int alphasort(const struct dirent **d1, const struct dirent **d2);

static int width1 = 0;
static int width2 = 0;
static int maxNodeCount = 0;

static char currDir[4096];
static char rDir[10][50];
static int k = 0;

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

void normalLs(char *directory){

	DIR * dir;
	struct dirent * entry;

	dir = opendir(directory);
    // char str[10][50], temp[50];
    // int i = 0;


    struct dirent **namelist;
    int i,n;
    char *path;


    n = scandir(directory, &namelist, 0, alphasort);
    for (int i = 0; i<n; i++){
        if (checkHidden(namelist[i]->d_name) == 1 || checkHidden(namelist[i]->d_name) == 2){
                continue;
            }
        printf("%s\n", namelist[i]->d_name);
    }
    // if (n < 0)
    //     perror("scandir");
    // else {
    //     for (i = 0; i < n; i++) {
    //         path = namelist[i]->d_name;
    //         if (path[0] == "."){
    //             continue;
    //         }
    //         printf("%s\n", namelist[i]->d_name);
    //         free(namelist[i]);
    //         }
    //     }
    // free(namelist);

	// if (dir != NULL){
	// 	while (entry = readdir(dir)){
    //         if (checkHidden(entry->d_name) == 1 || checkHidden(entry->d_name) == 2){
    //             continue;
    //         }
	// 		// printf("%s  \n",entry->d_name);
    //         strncpy(str[i], entry->d_name, 10);
    //         i++;
	// 	}
		
	// 	closedir(dir);
	// }	
	// else{
	// 	perror("Could not open the directory");
	// }

    // // For printing in lexicographical order

    // for(i=0;i<9;i++){
    //     for(int j=i+1;j<10;j++){
    //         if(strcmp(str[i],str[j])>0){
    //              strcpy(temp,str[i]); 
    //              strcpy(str[i],str[j]);
    //              strcpy(str[j],temp);
    //         }
    //     }
    // }
    // printf("\nIn lexicographical order: \n");

    // for(i=0;i<9;i++){
    //     puts(str[i]);
    // }
}

void biggestEntry(char *directory){

    DIR *dir;
    int maxCount = 0;
    int maxScount = 0;
    char newSize[50];
    char maxNode[50];
    char newStringSize[50];
    struct dirent * entry;
    struct stat allStat;
    char wholeDir[4096];

    width1 = 0;
    width2 = 0;
    maxNodeCount = 0;

    dir = opendir(directory);

    if(dir != NULL){
        while (entry = readdir(dir)){
            if (checkHidden(entry->d_name) == 1 || checkHidden(entry->d_name) == 2){
                continue;
            }
            int counter = 0;
            int currNodeCount = 0;
            int stringCount = 0;
            strcat(wholeDir, directory);
            strcat(wholeDir, "/");
            strcat(wholeDir, entry->d_name);
            stat(wholeDir, &allStat);
            sprintf(newSize, "%ld", allStat.st_size);
            sprintf(maxNode, "%ld", allStat.st_ino);
            for (int i = 0; i < strlen(maxNode); i++){
                currNodeCount++;
                if(maxNodeCount < currNodeCount){
                    maxNodeCount = currNodeCount;
                }
            }
            for(int i = 0; i< strlen(newSize); i++){
                counter++;
                if (maxCount < counter){
                    maxCount = counter;
                }
            }
            sprintf(newStringSize, "%ld", allStat.st_nlink);
            for(int j = 0; j < strlen(newStringSize); j++){
                stringCount++;
                if(maxScount < stringCount){
                    maxScount = stringCount;
                }
            }
            strcpy(wholeDir, "");
        }
        closedir(dir);
    }
    width1 = maxScount;
    width2 = maxCount;
    // sprintf(max1, "%d", maxScount);
    // sprintf(max2, "%d", maxCount);
    return;
}

void printPermissions(__mode_t newMode){
    // printf((newMode & S_IRUSR) ? "r" : "-");
    // printf("HI");
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

void *fileType(__mode_t newMode){
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

void optionL(char *directory, int iCheck, int rCheck, int lCheck){
    DIR * dir;
    char temp[4096];
    struct dirent *entry;
    char timeBuff[50];
    struct tm *newTime;
    time_t curTimer;
    struct group *grp;
    struct passwd *pwd;
    struct stat curStat;
    pwd = getpwuid(geteuid());
    biggestEntry(directory);
    char link[64] = {0};
    char symbolic_link[1024] = {'\0'};
    // char str[10][50], temp1[50];
    int i =0;

    struct dirent **namelist;
    int n;
    n = scandir(directory, &namelist, 0, alphasort);

    // if (strcmp(options, "i") == 0){


    //     for (int j = 0; j<n; j++){
    //             if (checkHidden(namelist[j]->d_name) == 1 || checkHidden(namelist[j]->d_name) == 2){
    //                 continue;
    //             }
    //             strcat(temp, directory);
    //             strcat(temp, "/");
    //             strcat(temp, namelist[j]->d_name);
    //             stat(temp, &curStat);
    //             printf("%*ld %s  \n", maxNodeCount, curStat.st_ino, namelist[j]->d_name);
    //             strcpy(temp, "");
    //         }
    // }

    // else if (strcmp(options, "l") == 0){
    //     for (int j = 0; j<n; j++){
    // // if(dir != NULL){
    // //     while (entry = readdir(dir)){
    //         if (checkHidden(namelist[j]->d_name) == 1 || checkHidden(namelist[j]->d_name) == 2){
    //             continue;
    //         }
    //         strcat(temp, directory);
    //         strcat(temp, "/");
    //         strcat(temp, namelist[j]->d_name);
    //         stat(temp, &curStat);
    //         curTimer = curStat.st_mtime;
    //         newTime = localtime(&curTimer);
    //         strftime(timeBuff, 50, "%b %2d %4Y %2H:%2M", newTime);
    //         grp = getgrgid(curStat.st_gid);
    //         printf( (S_ISDIR(curStat.st_mode)) ? "d" : "-");
    //         printPermissions(curStat.st_mode);
    //         printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name);
    //         strcpy(temp, "");

    //     }
    // }

    // else if (strcmp(options, "il") == 0){
        if(n != -1){
            printf("%s: \n", directory);
        }
        
            for (int j = 0; j<n; j++){
                if (checkHidden(namelist[j]->d_name) == 1 || checkHidden(namelist[j]->d_name) == 2){
                    continue;
                }
                char file[4096];
                strcpy(file, directory);
                if(strcmp("/", directory))
                    strcat(file, "/");
                strcat(file, namelist[j]->d_name);
                strcat(file, "\0");

        // error
                if(lstat(file, &curStat)==-1){
                    printf("Error in print_dir on file: [%s]\n", file);
                    printf("%s\n", strerror(errno));
                    return 1;
                }

                curTimer = curStat.st_mtime;
                newTime = localtime(&curTimer);
                strftime(timeBuff, 50, "%b %2d %4Y %2H:%2M", newTime);
                grp = getgrgid(curStat.st_gid);


                if (iCheck == 1 && lCheck == 1){
                    // char *ret;
                    // ret = strstr(namelist[j]->d_name, " ");
                    // if (ret){
                    //     printf("%*ld ", maxNodeCount, curStat.st_ino);
                    //     printf( (S_ISDIR(curStat.st_mode)) ? "d" : "-");
                    //     printPermissions(curStat.st_mode);
                    //     printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name);
                    //     strcpy(temp, "");
                    // }
                    // else{
                    //     printf("%*ld ", maxNodeCount, curStat.st_ino);
                    //     printf( (S_ISDIR(curStat.st_mode)) ? "d" : "-");
                    //     printPermissions(curStat.st_mode);
                    //     printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name);
                    //     strcpy(temp, "");
                    // }
                    char *ret;
                    ret = strstr(namelist[i]->d_name, " ");
                    printf("%*ld ", maxNodeCount, curStat.st_ino);
                    char k = fileType(curStat.st_mode);
                    if (ret){
                        if( k == 'l'){
                            readlink(file, symbolic_link, 1024);
                            printPermissions(curStat.st_mode);
                            printf(" %*ld %s %s %*ld %s \'%s\' -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name, symbolic_link);
                            strcpy(temp, "");
                        }
                        else{
                            printPermissions(curStat.st_mode);
                            printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name);
                            strcpy(temp, "");
                        }
                    }
                    else{
                        if( k == 'l'){
                            readlink(file, symbolic_link, 1024);
                            printPermissions(curStat.st_mode);
                            printf(" %*ld %s %s %*ld %s %s -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name, symbolic_link);
                            strcpy(temp, "");
                        }
                        else{
                            printPermissions(curStat.st_mode);
                            printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name);
                            strcpy(temp, "");
                        }
                    }
                }
                else if (iCheck == 1){
                    char *ret;
                    ret = strstr(namelist[j]->d_name, " ");
                    if (ret){
                        printf("%*ld \'%s\'  \n", maxNodeCount, curStat.st_ino, namelist[j]->d_name);
                        strcpy(temp, "");
                    }
                    else{
                        printf("%*ld %s  \n", maxNodeCount, curStat.st_ino, namelist[j]->d_name);
                        strcpy(temp, "");
                    }
                }
                else if (lCheck == 1){
                    char *ret;
                    ret = strstr(namelist[j]->d_name, " ");
                    char k = fileType(curStat.st_mode);
                    if (ret){
                        if (k == 'l'){ 
                            readlink(file, symbolic_link, 1024);                                          
                            printPermissions(curStat.st_mode);
                            printf(" %*ld %s %s %*ld %s \'%s\' -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name, symbolic_link);
                            strcpy(temp, ""); 
                        }
                        else{
                        // printf( (S_ISDIR(curStat.st_mode)) ? "d" : "-");
                        printPermissions(curStat.st_mode);
                        printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name);
                        strcpy(temp, "");
                        }
                    }
                    else{
                        if (k == 'l'){
                            readlink(file, symbolic_link, 1024);
                            printPermissions(curStat.st_mode);
                            printf(" %*ld %s %s %*ld %s %s -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name, symbolic_link);
                            strcpy(temp, "");
                        }
                        else{
                        // printf( (S_ISDIR(curStat.st_mode)) ? "d" : "-");
                        // printf( (S_ISLNK(curStat.st_mode)) ? readlink(directory, symbolic_link, 64) : "-");
                        // printf("THis link is: %s\n", symbolic_link);
                        printPermissions(curStat.st_mode);
                        printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[j]->d_name);
                        strcpy(temp, "");
                        }
                    }
                }
                else{
                    char *ret;
                    ret = strstr(namelist[j]->d_name, " ");
                    if (ret){
                        // printf("This has a space \n");
                        printf("\'%s\'\n", namelist[j]->d_name);
                    }
                    else{
                    printf("%s\n", namelist[j]->d_name);
                    }
                }

        }
        if ( n!=-1){
                printf("\n");
        }
        
        if (n == -1){
            printSingleFile(directory, iCheck, rCheck, lCheck);
        }
    // }
        // closedir(dir);
} 

void printSingleFile(char *directory, int iCheck, int rCheck, int lCheck){
    DIR * dir;
    char temp[4096];
    struct dirent *entry;
    char timeBuff[50];
    struct tm *newTime;
    time_t curTimer;
    struct group *grp;
    struct passwd *pwd;
    struct stat curStat;
    pwd = getpwuid(geteuid());
    biggestEntry(directory);
    char symbolic_link[1024] = {'\0'};
    // char str[10][50], temp1[50];
    int i =0;
    

    struct dirent **namelist;
    int n;
    n = scandir(directory, &namelist, 0, alphasort);
            if (checkHidden(directory) == 1 || checkHidden(directory) == 2){
                return;
            }
            // strcat(temp, directory);
            // // strcat(temp, "/");
            // // strcat(temp, namelist->d_name);
            // stat(temp, &curStat);
            // curTimer = curStat.st_mtime;
            // newTime = localtime(&curTimer);
            // strftime(timeBuff, 50, "%b %2d %4Y %2H:%2M", newTime);
            // grp = getgrgid(curStat.st_gid);

                char file[4096];
                strcpy(file, directory);
                // if(strcmp("/", directory))
                //     strcat(file, "/");
                // strcat(file, namelist[i]->d_name);
                strcat(file, "\0");

        // error
                if(lstat(file, &curStat)==-1){
                    printf("Error in print_dir on file: [%s]\n", file);
                    printf("%s\n", strerror(errno));
                    return 1;
                }

                curTimer = curStat.st_mtime;
                newTime = localtime(&curTimer);
                strftime(timeBuff, 50, "%b %2d %4Y %2H:%2M", newTime);
                grp = getgrgid(curStat.st_gid);


            if (iCheck == 1 && lCheck == 1){
                char *ret;
                ret = strstr(directory, " ");
                printf("%*ld ", maxNodeCount, curStat.st_ino);
                char k = fileType(curStat.st_mode);
                if(ret){
                    if(k == 'l'){
                    readlink(file, symbolic_link, 1024);
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s \'%s\' -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory, symbolic_link);
                    strcpy(temp, "");
                    }
                    else {
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory);
                    strcpy(temp, "");
                    }
                }
                else{
                    if(k == 'l'){
                    readlink(file, symbolic_link, 1024);
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s %s -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory, symbolic_link);
                    strcpy(temp, "");
                    }
                    else {
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory);
                    strcpy(temp, "");
                    } 
                }
            }
            else if (iCheck == 1){
                    printf("%*ld %s  \n", maxNodeCount, curStat.st_ino, directory);
                    strcpy(temp, "");
            }
            else if (lCheck == 1){
                char *ret;
                ret = strstr(directory, " ");
                char k = fileType(curStat.st_mode);
                if(ret){
                    if(k == 'l'){
                    readlink(file, symbolic_link, 1024);
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s \'%s\' -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory, symbolic_link);
                    strcpy(temp, "");
                    }
                    else {
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory);
                    strcpy(temp, "");
                    }
                }
                else{
                    if(k == 'l'){
                    readlink(file, symbolic_link, 1024);
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s %s -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory, symbolic_link);
                    strcpy(temp, "");
                    }
                    else {
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, directory);
                    strcpy(temp, "");
                    } 
                }
            }
            else{
                char *ret;
                ret = strstr(directory, " ");
                if (ret){
                    // printf("This has a space \n");
                    printf("\'%s\'\n", directory);
                }
                else{
                printf("%s\n", directory);
                }
            }
}

void print_dir(char *directory, int iCheck, int rCheck, int lCheck){
    struct stat curStat;
    struct dirent **namelist;
    struct stat stat_struct;
    int n;
    int length = 1024;
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
        if(lstat(directory, &stat_struct) == -1){
            printf("Error in print_dir on file");
        }
        else{
            printf("%lu\t%s\n ", stat_struct.st_size, directory);
        }
    }
    else{
        for (int i = 0; i < n; i++){
            if(!(strcmp(".", namelist[i]->d_name) && strcmp("..",namelist[i]->d_name))){ 
                free(namelist[i]);
                continue;
            }
            if (checkHidden(namelist[i]->d_name) == 1 || checkHidden(namelist[i]->d_name) == 2){
                    continue;
                }
                char file[4096];
                strcpy(file, directory);
                if(strcmp("/", directory))
                    strcat(file, "/");
                strcat(file, namelist[i]->d_name);
                strcat(file, "\0");

        // error
                if(lstat(file, &curStat)==-1){
                    printf("Error in print_dir on file: [%s]\n", file);
                    printf("%s\n", strerror(errno));
                    return 1;
                }

                curTimer = curStat.st_mtime;
                newTime = localtime(&curTimer);
                strftime(timeBuff, 50, "%b %2d %4Y %2H:%2M", newTime);
                grp = getgrgid(curStat.st_gid);


            if (iCheck == 1 && lCheck == 1){
                char *ret;
                ret = strstr(namelist[i]->d_name, " ");
                printf("%*ld ", maxNodeCount, curStat.st_ino);
                char k = fileType(curStat.st_mode);
                if (ret){
                    if( k == 'l'){
                        readlink(file, symbolic_link, 1024);
                        printPermissions(curStat.st_mode);
                        printf(" %*ld %s %s %*ld %s \'%s\' -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name, symbolic_link);
                        strcpy(temp, "");
                    }
                    else{
                        printPermissions(curStat.st_mode);
                        printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name);
                        strcpy(temp, "");
                    }
                }
                else{
                    if( k == 'l'){
                        readlink(file, symbolic_link, 1024);
                        printPermissions(curStat.st_mode);
                        printf(" %*ld %s %s %*ld %s %s -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name, symbolic_link);
                        strcpy(temp, "");
                    }
                    else{
                        printPermissions(curStat.st_mode);
                        printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name);
                        strcpy(temp, "");
                    }
                }
            }
            else if (iCheck == 1){
                char *ret;
                ret = strstr(namelist[i]->d_name, " ");
                if (ret){
                    printf("%*ld \'%s\'  \n", maxNodeCount, curStat.st_ino, namelist[i]->d_name);
                    strcpy(temp, "");
                }
                else{
                    printf("%*ld %s  \n", maxNodeCount, curStat.st_ino, namelist[i]->d_name);
                    strcpy(temp, "");
                }
            }
            else if (lCheck == 1){
                char *ret;
                ret = strstr(namelist[i]->d_name, " ");
                char k = fileType(curStat.st_mode);
                if (ret){
                    if (k == 'l'){ 
                        readlink(file, symbolic_link, 1024);                                          
                        printPermissions(curStat.st_mode);
                        printf(" %*ld %s %s %*ld %s \'%s\' -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name, symbolic_link);
                        strcpy(temp, ""); 
                    }
                    else{
                    // printf( (S_ISDIR(curStat.st_mode)) ? "d" : "-");
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s \'%s\'\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name);
                    strcpy(temp, "");
                    }
                }
                else{
                    if (k == 'l'){
                        readlink(file, symbolic_link, 1024);
                        printPermissions(curStat.st_mode);
                        printf(" %*ld %s %s %*ld %s %s -> %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name, symbolic_link);
                        strcpy(temp, "");
                    }
                    else{
                    // printf( (S_ISDIR(curStat.st_mode)) ? "d" : "-");
                    // printf( (S_ISLNK(curStat.st_mode)) ? readlink(directory, symbolic_link, 64) : "-");
                    // printf("THis link is: %s\n", symbolic_link);
                    printPermissions(curStat.st_mode);
                    printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name);
                    strcpy(temp, "");
                    }
                }
            }
            else{
                char *ret;
                ret = strstr(namelist[i]->d_name, " ");
                if (ret){
                    // printf("This has a space \n");
                    printf("\'%s\'\n", namelist[i]->d_name);
                }
                else{
                printf("%s\n", namelist[i]->d_name);
                }
            }
        }
    }
}


void optionR(char *directory, int iCheck, int rCheck, int lCheck){
    
    if (strstr(directory, " ")){
        printf("\'%s\': \n", directory);
    }
    else{
        printf("%s: \n", directory);
    }
    print_dir(directory, iCheck, rCheck, lCheck);
    struct stat curStat;
    struct dirent **namelist;
    struct stat stat_struct;
    int n;
    int length = 1024;
        struct tm *newTime;
    char temp[4096];
    time_t curTimer;
    char timeBuff[50];
    struct group *grp;
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    // biggestEntry(directory);
    n = scandir(directory, &namelist, NULL, alphasort);
    

    if (n < 0){
        if(lstat(directory, &stat_struct) == -1){
            printf("Error in print_dir on file");
        }
        else{
            printf("%lu\t%s\n ", stat_struct.st_size, directory);
        }
    }
    // print_dir(directory, iCheck, rCheck, lCheck);
    // printf("\n%s: ")
    for (int i = 0; i < n; i++){
        if(!(strcmp(".", namelist[i]->d_name) && strcmp("..",namelist[i]->d_name))){ 
            free(namelist[i]);
            continue;
        }
        if (checkHidden(namelist[i]->d_name) == 1 || checkHidden(namelist[i]->d_name) == 2){
                continue;
            }
        char file[length*4];
        strcpy(file, directory);
                // Don't add if it's '/'
        if(strcmp("/", directory)){
            strcat(file, "/");
        }
        strcat(file, namelist[i]->d_name);
        strcat(file, "\0");

        // Check the file
        if(lstat(file, &stat_struct)==-1){
            printf("Error in print_dir on file: [%s]\n", file);
            printf("%s\n", strerror(errno));
            free(namelist[i]);
            return 1;
        }
        
        // if it's a directory, time to dig a little deeper
        if(S_ISDIR(stat_struct.st_mode)){
            // printf("%s: \n", directory);
            // print_dir(file, iCheck, rCheck, lCheck);
            printf("\n");
            optionR(file, iCheck, rCheck, lCheck);
        }
        free(namelist[i]);
        // else{
    }
    free(namelist);
}   
        //     strcat(temp, directory);
        //     strcat(temp, "/");
        //     strcat(temp, namelist[i]->d_name);
        //     stat(temp, &curStat);
        //     curTimer = curStat.st_mtime;
        //     newTime = localtime(&curTimer);
        //     strftime(timeBuff, 50, "%b %2d %4Y %2H:%2M", newTime);
        //     grp = getgrgid(curStat.st_gid);


        //     if (iCheck == 1 && lCheck == 1){
        //         printf("%*ld ", maxNodeCount, curStat.st_ino);
        //         printf( (S_ISDIR(curStat.st_mode)) ? "d" : "-");
        //         printPermissions(curStat.st_mode);
        //         printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name);
        //         strcpy(temp, "");
        //     }
        //     else if (iCheck == 1){
        //             printf("%*ld %s  \n", maxNodeCount, curStat.st_ino, namelist[i]->d_name);
        //             strcpy(temp, "");
        //     }
        //     else if (lCheck == 1){
        //         printf( (S_ISDIR(curStat.st_mode)) ? "d" : "-");
        //         printPermissions(curStat.st_mode);
        //         printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, namelist[i]->d_name);
        //         strcpy(temp, "");
        //     }
        //     else{
        //         printf("%s\n", namelist[i]->d_name);
        //     }
        // }

    // if(strcmp(options, "R") == 0){
    //     normalLs(directory);
    // }
    // else if(strcmp(options, "iR") == 0){
    //     optionL(directory, iCheck, rCheck, lCheck);
    // }
    // else if(strcmp(options, "lR") == 0){
    //     optionL(directory, "l");
    // }
    //     else if(strcmp(options, "ilR") == 0){
    //     optionL(directory, "il");
    // }
    
    // if (S_ISDIR(curStat.st_mode)) {
    //     printDir(directory);
    // }
    // else{
    //     normalPrint(directory);
    // }
    // optionL(directory, iCheck, rCheck, lCheck);
    // DIR *dir;
    // DIR *subDir;
    // int ret;
    // struct dirent *entry;
    // char wholeDir[4096];
    // dir = opendir(directory);
    // printf("\n");
    // strcpy(currDir, directory);
    // while((entry = readdir(dir)) != NULL){
    //     if (checkHidden(entry->d_name) == 1 || checkHidden(entry->d_name) == 2){
    //             continue;
    //         }
    //     if (entry->d_type == DT_DIR){
    //     strcpy(wholeDir, directory);
    //     strcat(wholeDir, "/");
    //     strcat(wholeDir, entry->d_name);
    //      if ((subDir = opendir(wholeDir)) == NULL) {
    //                 ret = errno;
    //                 fprintf(stderr, "Cannot open '%s': %s\n", entry->d_name, strerror(ret));
    //                 return 0;
    //         }
    //     // strcpy(rDir[k], wholeDir);
    //     // k++;
    //     // print entries in subdir
    //     optionR(wholeDir, iCheck, rCheck, lCheck);
    //     }
    //     // if ((ret = optionR(wholeDir)) != 0) {
    //     //     printf("UnixLs: cannot access '%s': %s",wholeDir,strerror(ret));
    //     // }
        

    // }
    // closedir(subDir);
    
// void sortR(char toSort[]){
//     char temp[50];
//        // storing strings in the lexicographical order
//    for (int i = 0; i < 10; ++i) {
//       for (int j = i + 1; j < 5; ++j) {

//          // swapping strings if they are not in the lexicographical order
//          if (strcmp(toSort[i], toSort[j]) > 0) {
//             strcpy(temp, toSort[i]);
//             strcpy(toSort[i], toSort[j]);
//             strcpy(toSort[j], temp);
//          }
//       }
//    }

//    printf("\nIn the lexicographical order: \n");
//    for (int i = 0; i < 10; ++i) {
//       fputs(toSort[i], stdout);
//    }
// }

