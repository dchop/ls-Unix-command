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

#include "myLs.h"

// int alphasort_case_insensitive(const struct dirent ** a, const struct dirent **b) 
// {
//     return(strcasecmp((*(const struct dirent **)a)->d_name,
//                 (*(const struct dirent **)b)->d_name));
// }

static int width1 = 0;
static int width2 = 0;

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
    char str[10][50], temp[50];
    // int i = 0;


    struct dirent **namelist;
    int i,n;
    char *path;


    // n = scandir(directory, &namelist, 0, alphasort_case_insensitive);
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

	if (dir != NULL){
		while (entry = readdir(dir)){
            if (checkHidden(entry->d_name) == 1 || checkHidden(entry->d_name) == 2){
                continue;
            }
			// printf("%s  \n",entry->d_name);
            strncpy(str[i], entry->d_name, 10);
            i++;
		}
		
		closedir(dir);
	}	
	else{
		perror("Could not open the directory");
	}

    // For printing in lexicographical order

    for(i=0;i<9;i++){
        for(int j=i+1;j<10;j++){
            if(strcmp(str[i],str[j])>0){
                 strcpy(temp,str[i]); 
                 strcpy(str[i],str[j]);
                 strcpy(str[j],temp);
            }
        }
    }
    printf("\nIn lexicographical order: \n");

    for(i=0;i<9;i++){
        puts(str[i]);
    }
}

void biggestEntry(char *directory){

    DIR *dir;
    int maxCount = 0;
    int maxScount = 0;
    char newSize[50];
    char newStringSize[50];
    struct dirent * entry;
    struct stat allStat;
    char wholeDir[4096];

    dir = opendir(directory);

    if(dir != NULL){
        while (entry = readdir(dir)){
            if (checkHidden(entry->d_name) == 1 || checkHidden(entry->d_name) == 2){
                continue;
            }
            int counter = 0;
            int stringCount = 0;
            strcat(wholeDir, directory);
            strcat(wholeDir, "/");
            strcat(wholeDir, entry->d_name);
            stat(wholeDir, &allStat);
            sprintf(newSize, "%ld", allStat.st_size);
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
    printf((newMode & S_IRUSR) ? "r" : "-");
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

void optionLi(char *directory){
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
    // int width1, width2 = biggestEntry(directory);
    // width1= 5;
    // width2 = 1;


    dir = opendir(directory);

    if(dir != NULL){
        while (entry = readdir(dir)){
            if (checkHidden(entry->d_name) == 1 || checkHidden(entry->d_name) == 2){
                continue;
            }
            strcat(temp, directory);
            strcat(temp, "/");
            strcat(temp, entry->d_name);
            stat(temp, &curStat);
            curTimer = curStat.st_mtime;
            newTime = localtime(&curTimer);
            strftime(timeBuff, 50, "%b %2d %4Y %2H:%2M", newTime);
            grp = getgrgid(curStat.st_gid);
            printf( (S_ISDIR(curStat.st_mode)) ? "d" : "-");
            printPermissions(curStat.st_mode);
            printf(" %*ld %s %s %*ld %s %s\n", width1, curStat.st_nlink, pwd->pw_name, grp->gr_name, width2, curStat.st_size, timeBuff, entry->d_name );
            strcpy(temp, "");

        }
        closedir(dir);
    }
}   
