#ifndef GRANTED_STACK_H
#define GRANTED_STACK_H

#include <stdio.h>

#define GRANTED_STACK_MAX 200

struct Grant_Stack {
    int     data[GRANTED_STACK_MAX][2];
    int     size;
};

typedef struct Grant_Stack Grant_Stack;

void Grant_Stack_Init(Grant_Stack *S);

int* Grant_Stack_Top(Grant_Stack *S);

void Grant_Stack_Push(Grant_Stack *S, int pid, int res_id);

void Grant_Stack_Pop(Grant_Stack *S);

#endif
