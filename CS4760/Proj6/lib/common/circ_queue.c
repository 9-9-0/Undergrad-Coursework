#include <stdio.h>
#include "circ_queue.h"

/*Initailization of circular queue.*/
void initCirQueue(CirQueue * q)
{
    q->front =  0;
    q->rear  = -1;
    q->count =  0;
}

/*Check Queue is full or not*/
int isFull(CirQueue * q)
{
    if(q->count == MAX_SIZE)
        return 1;

    return 0;
}

/*Check Queue is empty or not*/
int isEmpty(CirQueue * q)
{
    if(q->count == 0)
      return 1;

    return 0;
}

/*To insert item into circular queue.*/
void enQueue(CirQueue * q, int32 req_contents[6])
{
    if( isFull(q) )
    {
        printf("\nQueue Overflow\n");
        return;
    }

    q->rear = (q->rear+1) % MAX_SIZE;

    //PID
    q->request[q->rear][0] = req_contents[0];
    //Child Index
    q->request[q->rear][1] = req_contents[1];
    //Byte Offset
    q->request[q->rear][2] = req_contents[2];
    //Read/Write
    q->request[q->rear][3] = req_contents[3];
    //Request Time (Second)
    q->request[q->rear][4] = req_contents[4];
    //Request Time (Nanosecond)
    q->request[q->rear][5] = req_contents[5];

    q->count++;

    //fprintf(stderr, "\nInserted item : %d %d %d %d, %d:%d\n", req_contents[0], req_contents[1], req_contents[2], req_contents[3], req_contents[4], req_contents[5]);
    return;
}

/* To delete item from queue.
 * Returns 1 on queue underflow */
int deQueue(CirQueue * q, int32 holder[6])
{
    if( isEmpty(q) )
    {
        fprintf(stderr, "\nQueue Underflow");
        return 1;
    }


    holder[0] = q->request[q->front][0];
    holder[1] = q->request[q->front][1];
    holder[2] = q->request[q->front][2];
    holder[3] = q->request[q->front][3];
    holder[4] = q->request[q->front][4];
    holder[5] = q->request[q->front][5];

    q->front = (q->front+1)%MAX_SIZE;

    q->count--;

    return 0;
}

int peak_head_time(CirQueue *q, uint holder[2]) {
  if (isEmpty(q)) {
    return -1;
  }

  holder[0] = q->request[q->front][4];
  holder[1] = q->request[q->front][5];

  return 0;
}
