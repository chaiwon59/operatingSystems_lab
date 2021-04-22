#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>

//Function declaration
void createDir();
void listFiles();

//Main function
int main() {
    if(fork() == 0){
        //child process
        printf("Child Process\n");
        createDir();
        listFiles();
    } else{
        //parent process
        printf("Parent Process\n");
        wait(NULL);
        exit(0);
    }
}

//Create a directory in the directory in which the program is executed
void createDir() {
    //System call
    system("mkdir newDir");

    //System call
    system("ls -a");

    //C function creates a new directory with the name "newDir" and with the mode 0777,
    // which means that allows the widest possible access
    mkdir("newDir", 0777);
}

//List all files and directories in the current directory
void listFiles() {

    //Contains constructs that facilitate directory traversing
    struct dirent *de;

    //DIR pointer variable dr acts as the directory handle for opendir() function
    DIR *dr = opendir(".");

    //If it's NULL, the program quits
    if(dr == NULL) {
        printf("Error\n");
    }

    //Loops as long as teh value returned from readdir() isn't NULL
    //For each item found, de->d_name prints its name
    while((de = readdir(dr)) != NULL) {
        printf("%s\n", de->d_name);
    }
    closedir(dr);
}
