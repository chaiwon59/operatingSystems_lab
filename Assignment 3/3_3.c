#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <ctype.h>


// define led output pins
#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3


struct input {
    int num;
    int end;
};


void * readFile();
void * controlLED();
void * inputListener();
int main(int argc, char *argv[]) {

    if(argc < 1) {
        printf("Enter a number between 0 and 15!\n");
        return 0;
    }

    wiringPiSetup();

    //Set LEDs pin to output
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);

    sleep(1);

    struct input *input = malloc(sizeof(struct input));


    input->num = atoi(argv[1]);
    input->end = 0;

    if(input->num > 15) {
        printf("Enter a number between 0 and 15!\n");
        exit(1);
    }

    pthread_t input_listener;
    pthread_create(&input_listener, NULL, &inputListener, input);
    pthread_t control_LED;
    pthread_create(&control_LED, NULL, &controlLED, input);
    pthread_t file_listener;
    pthread_create(&file_listener, NULL, &readFile, &input->end);

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);

    pthread_join(file_listener, NULL);
    pthread_join(input_listener, NULL);
    free(input);

}

void *inputListener(void *curr){
    struct input *input = (struct input *)curr;
    int new_num;
    while(1) {
        scanf("%d", &new_num);
        if(new_num == -1){
            input->end = 1;
            return NULL;
        } else if(new_num > 15 || new_num < 0){
            printf("Enter a number between 0 and 15!\n");
        } else{
            input->num = new_num;
        }
    }
}

void *readFile(void *input){
    int *end = (int *)input;
//    sleep(1);
//    struct input *input = (struct input *)curr;
//    int *end = curr->end;
    FILE *file = fopen("data.txt", "r");
    if(file == NULL){
        printf("Unable to open file!\n");
        exit(1);
    }
//    double temp[256];
    char *line = NULL;
    size_t len = 0;
    double sum = 0;
    double temp;
    clock_t start_time = clock();
    while(*end == 0 && getline(&line, &len, file) != -1){
        temp = atof(line);
        sum += temp;
    }
    clock_t end_time = clock();
    if(*end == 0){
        printf("Result, %lf\n", sum);
        double time_taken = (double) (end_time - start_time) / CLOCKS_PER_SEC;
        printf("Time taken, %lf\n", time_taken);
    }

    free(line);
//    printf("done\n");
    fclose(file);

    return 0;
}

void *controlLED(void *current){
    struct input *input = (struct input *)current;
    int *binary;

    int curr = 0;
    while(!input->end) {
        if (input->num != curr && input->num >= 0 && input->num <= 15) {
            if (input->num > curr) {
                curr++;
            } else {
                curr--;
            }    //If the binary is 1, turn on the LED

            if(curr & (1<<3))
                digitalWrite(LED4, HIGH);
            else
                digitalWrite(LED4, LOW);
            if(curr & (1<<2))
                digitalWrite(LED3, HIGH);
            else
                digitalWrite(LED3, LOW);
            if(curr & (1<<1))
                digitalWrite(LED2, HIGH);
            else
                digitalWrite(LED2, LOW);
            if(curr & (1<<0))
                digitalWrite(LED1, HIGH);
            else
                digitalWrite(LED1, LOW);
            usleep(200000);
        }
//        free(binary);
//        curr = input->num;
    }
//    free(binary);
}