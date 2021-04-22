#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <softPwm.h>
#include <pthread.h>
#include <unistd.h>

// define led output pins
#define LED1 7
#define LED2 0
#define LED3 2
#define LED4 3

struct Node {
    int brithness, duration; //Stored valeus
    struct Node* next; //Next node in line
};

struct Queue {
    int led;
    struct Node* front; //First node in the queue
    struct Node* back; //Last node in the queue
    pthread_mutex_t mutex; //Used for mutual exclusion
};

const int LEDList[4] = {LED1, LED2, LED3, LED4};

struct Queue* queueList[4];

void addToQueue();
void removeFromQueue();
int queueSize();
void initQueue();
void *fetch();
void *lit();
void *commandLine();


int main(int argc, char *argv[]) {

    wiringPiSetup();

    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);

    softPwmCreate(LED1, 0, 100);
    softPwmCreate(LED2, 0, 100);
    softPwmCreate(LED3, 0, 100);
    softPwmCreate(LED4, 0, 100);

    int i;
    for(i=0; i<4; i++){
        struct Queue* q = malloc(sizeof *q);
        initQueue(q);
        queueList[i] = q;
    }

    pthread_t fetchThread;
    pthread_create(&fetchThread, NULL, fetch, NULL);

    pthread_t litThread0;
    pthread_t litThread1;
    pthread_t litThread2;
    pthread_t litThread3;

    pthread_create(&litThread0, NULL, lit, queueList[0]);
    pthread_create(&litThread1, NULL, lit, queueList[1]);
    pthread_create(&litThread2, NULL, lit, queueList[2]);
    pthread_create(&litThread3, NULL, lit, queueList[3]);

//    pthread_t inputThread;
//    pthread_create(&inputThread, NULL, commandLine, NULL);

    pthread_join(fetchThread, NULL);
//    pthread_join(inputThread, NULL);
    pthread_join(litThread0, NULL);
    pthread_join(litThread1, NULL);
    pthread_join(litThread2, NULL);
    pthread_join(litThread3, NULL);


//    free(cInput);
    free(queueList[0]);
    free(queueList[1]);
    free(queueList[2]);
    free(queueList[3]);

}
//Adds a new node to the back of the queue
void addToQueue(struct Queue* queue, int brightness, int duration) {

    struct Node *newNode = malloc(sizeof(struct Node));
    struct Node *currBack;
    newNode->brithness = brightness;
    newNode->duration = duration;
    newNode->next = NULL;

    pthread_mutex_lock(&queue->mutex);

    if(queue->back == NULL){
        queue->front = newNode;
        queue->back = newNode;
    } else{
        currBack = queue->back;
        currBack->next = newNode;
        queue->back = newNode;
    }

    pthread_mutex_unlock(&queue->mutex);

}

// Removes a node from the front of the queue
// The memory of the removed node has to be freed
// Returns the values of the removed node via pointers
// If the queue is empty return -1 for both the brightness and the duration
void removeFromQueue(struct Queue* queue, int* pBrightness, int* pDuration) {

    pthread_mutex_lock(&queue->mutex);
    struct Node *frontNode = queue->front;

    if(queue->front == NULL){
        *pBrightness = -1;
        *pDuration = -1;
    } else{
        frontNode = queue->front;
        *pBrightness = frontNode->brithness;
        *pDuration = frontNode->duration;
        queue->front = frontNode->next;
        free(frontNode);
    }

    pthread_mutex_unlock(&queue->mutex);
}

//Returns the length of the queue
int queueSize(struct Queue* queue) {

    int size = 0;
    struct Node *curr = queue->front;
    while(curr != NULL){
        curr = curr->next;
        size++;
    }

    return size;

}

//Initializes the values of an already allocated queue struct
void initQueue(struct Queue* queue) {
    queue->front = NULL;
    queue->back = NULL;
    pthread_mutex_init(&queue->mutex, NULL);
}

void *lit(void* input) {
    struct Queue *queue = (struct Queue *) input;

    int brightness = 0;
    int duration = 0;

    while (1) {
        while(queueSize(queue) > 0){
            removeFromQueue(queue, &brightness, &duration);
            softPwmWrite(LEDList[queue->led], brightness);
            usleep(duration * 1000);
        }
        if(queueSize(queue) == 0){
            softPwmWrite(LEDList[queue->led], 0);
        }
    }
}


void *fetch() {

    int led, brightness, duration;

    while (scanf("%d %d %d\n", &led, &brightness, &duration) != EOF) {
        if (led >= 0 && led < 4) {
            addToQueue(queueList[led], brightness, duration);
            queueList[led]->led = led;
        } else {
            break;
        }
    }
    return NULL;
}



