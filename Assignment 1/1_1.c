#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

//Main function
int main() {

    //Character array for name
    char name[] = "ChaiwonPark";
    //int for student number
    int studentNo = 4813790;
    //Getting the ID of the process that runs my program
    pid_t id = getpid();

    //Printing variables
    printf("%s\n", name);
    printf("%d\n", studentNo);
    printf("%lun\n", id);
    return 0;

}
