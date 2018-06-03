#include "lib/stack.h"

int main(int argc, char argv[]) {

  Stack myStack;
  Stack_Init(&myStack);

  Stack_Push(&myStack, 1);
  Stack_Push(&myStack, 2);
  Stack_Push(&myStack, 3);
  Stack_Push(&myStack, 4);

  for (int i = 0; i < 5; i++) {
    fprintf(stderr, "Value Returned: %d\n", Stack_Top(&myStack));
    Stack_Pop(&myStack);
  }

  return 0;
}
