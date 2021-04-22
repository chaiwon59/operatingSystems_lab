#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

// define led output pins
#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3

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

    int i;
    int number = atoi(argv[1]);
    int binary[4];

    if(number > 15){
        printf("Enter a number between 0 and 15!\n");
        return 0;
    }

    printf("%d\n", number);

    //Coverting decimal into binary
    for(i = 0; number > 0; i++){
        binary[i] = number % 2;
        number = number / 2;
    }

    //Prints the binary number
    for(i = i - 1; i >= 0; i--){
        printf("%d\n", binary[i]);
    }

    //If the binary is 1, turn on the LED
    if(binary[3] == 1){
        digitalWrite(LED1, HIGH);
    } else{
        digitalWrite(LED1, LOW);
    }

    if(binary[2] == 1){
        digitalWrite(LED2, HIGH);
    } else{
        digitalWrite(LED2, LOW);
    }

    if(binary[1] == 1){
        digitalWrite(LED3, HIGH);
    } else{
        digitalWrite(LED3, LOW);
    }

    if(binary[0] == 1){
        digitalWrite(LED4, HIGH);
    } else{
        digitalWrite(LED4, LOW);
    }

    return 0;
}