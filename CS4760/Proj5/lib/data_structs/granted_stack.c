#include "granted_stack.h"

void Grant_Stack_Init(Grant_Stack *S)
{
    S->size = 0;
}

int* Grant_Stack_Top(Grant_Stack *S)
{
    if (S->size == 0) {
        //fprintf(stderr, "Error: Grant_Stack empty\n");
        return NULL;
    }

    return S->data[S->size-1];
}

void Grant_Stack_Push(Grant_Stack *S, int pid, int res_id)
{
    if (S->size < GRANTED_STACK_MAX) {
        S->data[S->size][0] = pid;
        S->data[S->size][1] = res_id;
        S->size++;
    }
    else
        fprintf(stderr, "Error: Grant_Stack full\n");
}

void Grant_Stack_Pop(Grant_Stack *S)
{
    if (S->size == 0)
        fprintf(stderr, "Error: Grant_Stack empty\n");
    else
        S->size--;
}
