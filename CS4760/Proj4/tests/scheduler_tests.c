#include <stdio.h>
#include "../lib/deque.h"
#include "../lib/scheduler.h"

int main() {
  FILE* rand_file = fopen("/dev/random", "r");
  for (int i = 0; i < 20; i++) {
    fprintf(stderr, "%d\n", get_rand_action(rand_file));
  }
}
