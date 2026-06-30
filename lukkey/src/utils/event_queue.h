/*------------------------------------------------------------------------------
 * @file    EVENT_QUEUE.H
 * @author  FIRMWARE TEAM 
 * @date    2023/06/21 21:59:27
 * @brief   THIS FILE PROVIDES ALL THE FIRMWARE FUNCTIONS.
 * -----------------------------------------------------------------------------
 * @attention 
 
 * THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE 
 * CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE 
 * TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH 
 * HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN 
 * CONNECTION WITH PRODUCTS MADE BY CUSTOMERS. 
 * 
 * <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2> 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/
#ifndef __EVENT_QUEUE_H
#define __EVENT_QUEUE_H

/* Files includes  -----------------------------------------------------------*/
#include <pthread.h>
#include <stdbool.h>

/* Defines -------------------------------------------------------------------*/
#define MAX_QUEUE_SIZE 32

/* Variables -----------------------------------------------------------------*/
typedef struct {
    int id;
} Event_t;

typedef struct {
    Event_t events[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
    pthread_mutex_t mutex;
} EventQueue_t;

/* Functions ----------------------------------------------------------------*/
void initializeQueue(EventQueue_t* queue);
void destroyQueue(EventQueue_t* queue);
bool isQueueEmpty(EventQueue_t* queue);
bool isQueueFull(EventQueue_t* queue);
int  enqueueEvent(EventQueue_t* queue, const Event_t* event);
bool dequeueEvent(EventQueue_t* queue, Event_t* out);


#endif
