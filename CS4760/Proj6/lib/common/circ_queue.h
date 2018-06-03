#ifndef CIRC_QUEUE_H
#define CIRC_QUEUE_H

#include <stdint.h>

#define MAX_SIZE 18

typedef int32_t int32;
typedef uint32_t uint;

typedef struct {
    int front;
    int rear;
    int count;
    int32 request[MAX_SIZE][6];
    //Max of 18 requests
    /* 0 = PID
     * 1 = child index
     * 2 = byte reference
     * 3 = operation type (read/write/terminate)
     * 4 = second @ which request was generated
     * 5 = nanosecond @ which request was generated
     */
} CirQueue;

void initCirQueue(CirQueue * q);

int isFull(CirQueue * q);

int isEmpty(CirQueue * q);

void enQueue(CirQueue * q, int32 req_contents[6]);

int deQueue(CirQueue * q, int32 holder[6]);

void peakHead(CirQueue *q, int32 holder[6]);

int peak_head_time(CirQueue *q, uint holder[2]);

#endif
