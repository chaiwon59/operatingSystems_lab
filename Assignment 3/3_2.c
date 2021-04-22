#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
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

int* toBinary();
void * controlLED();
void * inputListener();
int isNumber();
//static int curr = 0;
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


    if(input->num > 15 || isalpha(input->num) || input->num < 0) {
        printf("Enter a number between 0 and 15!\n");
        exit(1);
    }

    pthread_t input_listener;
    pthread_create(&input_listener, NULL, &inputListener, input);

    pthread_t control_LED;
    pthread_create(&control_LED, NULL, &controlLED, input);
//    pthread_join(control_LED, NULL);

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);

    pthread_join(input_listener, NULL);
//    pthread_join(control_LED, NULL);
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
        } else if(new_num > 15 || new_num < 0 || isalpha(new_num)){
            printf("Enter a number between 0 and 15!\n");
        } else{
            input->num = new_num;
        }
    }
}


void *controlLED(void *current){
    struct input *input = (struct input *)current;
    int *binary;

    int curr = 0;
    while(!input->end) {
//        if(input->num < 0 || input->num > 15){
//
//        }

        if (input->num != curr && input->num >= 0 && input->num <= 15 && !isalpha(input->num)) {
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

int isNumber(char *input){
    int i;
    for(i = 0; input[i] != '\0'; i++){
        if(isalpha(input[i]))
            return 0;
    }
    return 1;
}