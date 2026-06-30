#include "event_queue.h"
#include <string.h>

#define EVENT_QUEUE_LOG(...)

void initializeQueue(EventQueue_t* queue) {
    pthread_mutex_init(&queue->mutex, NULL);
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
}

void destroyQueue(EventQueue_t* queue) {
    pthread_mutex_destroy(&queue->mutex);
}

bool isQueueEmpty(EventQueue_t* queue) {
    pthread_mutex_lock(&queue->mutex);
    bool empty = (queue->size == 0);
    pthread_mutex_unlock(&queue->mutex);
    return empty;
}

bool isQueueFull(EventQueue_t* queue) {
    pthread_mutex_lock(&queue->mutex);
    bool full = (queue->size == MAX_QUEUE_SIZE);
    pthread_mutex_unlock(&queue->mutex);
    return full;
}

int enqueueEvent(EventQueue_t* queue, const Event_t* event) {
    pthread_mutex_lock(&queue->mutex);
    if (queue->size >= MAX_QUEUE_SIZE) {
        pthread_mutex_unlock(&queue->mutex);
        EVENT_QUEUE_LOG("Queue is full. Cannot enqueue event with ID: %d\n", event->id);
        return -1;
    }
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->events[queue->rear] = *event;
    queue->size++;
    EVENT_QUEUE_LOG("Enqueued event with ID: %d\n", event->id);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

bool dequeueEvent(EventQueue_t* queue, Event_t* out) {
    pthread_mutex_lock(&queue->mutex);
    if (queue->size == 0) {
        pthread_mutex_unlock(&queue->mutex);
        EVENT_QUEUE_LOG("Queue is empty. Returning false.\n");
        return false;
    }
    *out = queue->events[queue->front];
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    queue->size--;
    EVENT_QUEUE_LOG("Dequeued event with ID: %d\n", out->id);
    pthread_mutex_unlock(&queue->mutex);
    return true;
}
