#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <softPwm.h>
#include <pthread.h>

// define led output pins
#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3

void * on();
void * off();
static pthread_barrier_t barrier;

int main(int argc, char *argv[]) {

    wiringPiSetup();

    //Set LEDs pin to output
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);

    //Initialise pin for PWM output
    softPwmCreate(LED1, 0, 100);
    softPwmCreate(LED2, 0, 100);
    softPwmCreate(LED3, 0, 100);
    softPwmCreate(LED4, 0, 100);

    pthread_barrier_init(&barrier, NULL, 2);

    pthread_t lit;
    pthread_create(&lit, NULL, &on, NULL);

    pthread_t unlit;
    pthread_create(&unlit, NULL, &off, NULL);

    pthread_join(lit, NULL);
    pthread_join(unlit, NULL);

    pthread_barrier_destroy(&barrier);

}

void *on() {

    printf("On is started\n");
    int i;
    for(i = 0; i <= 100; i+= 10){
        softPwmWrite(LED1, i);
        softPwmWrite(LED2, i);
        softPwmWrite(LED3, i);
        softPwmWrite(LED4, i);
        usleep(200000);
    }
    printf("On is finished\n");
    pthread_barrier_wait(&barrier);
}

void *off() {
    pthread_barrier_wait(&barrier);
    printf("Off is started\n");
    int i;
    for(i = 100; i >= 0; i-= 10){
        softPwmWrite(LED1, i);
        softPwmWrite(LED2, i);
        softPwmWrite(LED3, i);
        softPwmWrite(LED4, i);
        usleep(200000);
    }
    printf("Off is finished\n");
}