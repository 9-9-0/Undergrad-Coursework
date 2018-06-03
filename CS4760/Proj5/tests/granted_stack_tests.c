#include "../lib/oss_management.h"

//gcc granted_stack_tests.c ../lib/oss_management.c ../lib/data_structs/granted_stack.c ../lib/common_management.c ../lib/linked_list.c

int main() {
  Grant_Stack stack;
  int* holder = NULL;

  Grant_Stack_Init(&stack);

  for (int i = 100; i < 118; i++) {
    Grant_Stack_Push(&stack, i, i+1);
  }

  for (int i = 100; i < 118; i++) {
    holder = Grant_Stack_Top(&stack);
    if (holder) {
      fprintf(stderr, "Stack Top Value: %d:%d\n", holder[0], holder[1]);
    }
    Grant_Stack_Pop(&stack);
  }

}
