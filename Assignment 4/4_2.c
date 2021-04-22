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
    struct Node* front; //First node in the queue
    struct Node* back; //Last node in the queue
    pthread_mutex_t mutex; //Used for mutual exclusion
};

void addToQueue();
void removeFromQueue();
int queueSize();
void initQueue();
void *adding();
void *removing();

int main() {

    struct Queue *queue = malloc(sizeof(struct Queue));

    initQueue(queue);

    pthread_t add1;
    pthread_t add2;

    pthread_create(&add1, NULL, &adding, queue);
    pthread_create(&add2, NULL, &adding, queue);

    pthread_join(add1, NULL);
    pthread_join(add2, NULL);

    printf("Size after adding: %d\n", queueSize(queue));

    pthread_t remove1;
    pthread_t remove2;

    pthread_create(&remove1, NULL, &removing, queue);
    pthread_create(&remove2, NULL, &removing, queue);

    pthread_join(remove1, NULL);
    pthread_join(remove2, NULL);

    printf("Final size: %d\n", queueSize(queue));
    pthread_mutex_destroy(&queue->mutex);

    free(queue);

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

void *adding(void *queue_i) {
    struct Queue *q = queue_i;
    int i = 0;
    for(i = 0; i < 100000; i++){
        addToQueue(q, 0, 0);
    }
    return NULL;
}

void *removing(void *queue_i){
    struct Queue *q = queue_i;
    int i = 0;
    int brightness;
    int duration;
    for(i = 0; i < 100000; i++){
        removeFromQueue(q, &brightness, &duration);
        if(brightness == -1 || duration == -1){
            printf("Brightness or duration is -1.\n");
        }
    }
    return NULL;
}

