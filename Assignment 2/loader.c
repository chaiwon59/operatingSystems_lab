#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//Declaring functions
int objDump();
int objRemove();
int verify();

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        printf("loader <secured binary> <public key>\n");
        exit(0);
    }

    //Secured binary file we entered
    char *secured_binary = argv[1];

    //Public key we entered
    char *public_key = argv[2];

    //PID for dump, remove and wait
    pid_t dump_pid;
    pid_t remove_pid;
    pid_t wait_pid;

    //Status for dump and removing signature
    int dumpRemove_status = 0;

    dump_pid = fork();
    if(dump_pid == 0){
        objDump(secured_binary);
    }

    remove_pid = fork();
    if(remove_pid == 0){
        objRemove(secured_binary);
    }

    //Waits for dumping and removing to be done
    while ((wait_pid = wait(&dumpRemove_status)) > 0);

    //When finished dumping and removing, starts verifying
    int verify_status = 0;
    pid_t wait_pid2;

    pid_t verify_pid = fork();

    if(verify_pid == 0) {
        printf("verify done");
        verify(public_key);
    }

    //Waits for verification to be done
    while((wait_pid2 = wait(&verify_status)) > 0);

    //If not done, means it's not verified
    if(verify_status != 0) {
        printf("Not verified");
        system("rm loader-temp temp.text");
        exit(-1);
    }

    char program[] = "./";
    strncat(program, secured_binary, strlen(secured_binary));

    // run program (./secured_binary)
    system(program);

    // clean up temp files
    system("rm loader-temp temp.txt");

    return 0;
}

//Function for dumping the signature
int objDump(char *secured_binary) {
    char *shaDump[] = {"objcopy", "--dump-section", ".sha=temp.txt",
                            secured_binary, NULL};
    execvp(shaDump[0], shaDump);
    return -1;
}

//Function for removing the signature
int objRemove(char *secured_binary) {
    char *shaRemove[] = {"objcopy", "--remove-section", ".sha",
                         secured_binary, "loader-temp", NULL};
    execvp(shaRemove[0], shaRemove);
    return -1;
}

//Function for verification
int verify(char *public_key) {
    char *digest[] = {"openssl", "dgst", "-sha256", "-verify",
                            public_key, "-signature", "temp.txt",
                            "loader-temp", NULL};
    execvp(digest[0], digest);
    return -1;
}