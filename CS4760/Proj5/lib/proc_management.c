#include "proc_management.h"


/* Self-Explanatory */
void set_next_term_check(FILE* rand_file, uint32_t* current_time, uint32_t* next_check_time) {
  uint32_t rand_nano = get_rand(rand_file, 0, term_check_interval) * million;
  set_next_time(current_time, next_check_time, rand_nano);
}

void set_next_action_check(FILE* rand_file, uint32_t* current_time, uint32_t* next_check_time) {
  uint32_t rand_nano = get_rand(rand_file, 0, action_check_interval) * million;
  set_next_time(current_time, next_check_time, rand_nano);
}

/*  */
int test_termination(FILE* rand_file) {
  int rand_num = get_rand(rand_file, 1, 100);

  if (rand_num <= termination_chance)
    return 1;

  return 0;
}
