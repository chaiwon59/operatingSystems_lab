#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <pthread.h>

//Function declaration
void *createDirAndListFiles();

int main() {
    pthread_t thread_id;
    //Create thread
    pthread_create(&thread_id, NULL, createDirAndListFiles, NULL);
    //Wait for thread to finish
    pthread_join(thread_id, NULL);
    exit(0);
}


//Function to create directory and list all the files
//It will be executed as a thread when its name is specified in pthread_create()
void *createDirAndListFiles() {

    sleep(1);
    system("mkdir newDirThread");
    system("mkdir newDirThread");
    system("ls -a");
    mkdir("newDir", 0777);

    struct dirent *de;
    DIR *dr = opendir(".");

    if(dr == NULL) {
        printf("Error\n");
    }

    while((de = readdir(dr)) != NULL) {
        printf("%s\n", de->d_name);
    }
    closedir(dr);

}

/*
 * Differences between process and thread
 * Threads are not independent of one other, but processes are independent of
 * one another. Each process executes independently, but a thread ca read, write or modify
 * data of another thread.
 */