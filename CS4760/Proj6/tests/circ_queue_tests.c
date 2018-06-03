#include "../lib/common/circ_queue.h"
#include <stdio.h>

//gcc circ_queue_tests.c ../lib/common/circ_queue.c

int main() {
  CirQueue test_queue;
  initCirQueue(&test_queue);

  int temp[6] = {111, 222, 333, 1, 0, 0};
  int holder[6];

  /*
  fprintf(stderr, "FULL : %d\n", isFull(&test_queue));
  fprintf(stderr, "EMPTY : %d\n", isEmpty(&test_queue));

  enQueue(&test_queue, temp);

  fprintf(stderr, "deQueue operation : %d\n", deQueue(&test_queue, holder));
  fprintf(stderr, "Dequeued contents: %d, %d, %d, %d\n", holder[0], holder[1], holder[2], holder[3]);

  fprintf(stderr, "FULL : %d\n", isFull(&test_queue));
  fprintf(stderr, "EMPTY : %d\n", isEmpty(&test_queue));
  */

  for (int i = 0; i < 25; i++) {
    temp[2] = i;
    enQueue(&test_queue, temp);
  }

  fprintf(stderr, "FULL : %d\n", isFull(&test_queue));
  fprintf(stderr, "EMPTY : %d\n", isEmpty(&test_queue));

  while (!isEmpty(&test_queue)) {
    if (deQueue(&test_queue,holder)) {
      fprintf(stderr, "Failed to dequeue. \n");
      return 0;
    }
    fprintf(stderr, "Dequeued contents: %d, %d, %d, %d %d:%d\n", holder[0], holder[1], holder[2], holder[3], holder[4], holder[5]);
  }
  fprintf(stderr, "FULL : %d\n", isFull(&test_queue));
  fprintf(stderr, "EMPTY : %d\n", isEmpty(&test_queue));

  return 0;
}
