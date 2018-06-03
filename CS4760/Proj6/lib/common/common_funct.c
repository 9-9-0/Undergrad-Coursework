#include "common_funct.h"

/**************************************************************************/

int errCheck(char progName[], int errnoval) {
  if (errnoval) {
    fprintf(stderr, "%s: Error: %s\n", progName, strerror(errnoval));
    return 1;
  }
  return 0;
}

/**************************************************************************/

/* Generalized Random Number Generation */
int get_rand(FILE* rand_file, int min, int max) {
  if (max < min) {
    return -1;
  }

  unsigned int seed;
  fread(&seed, sizeof(unsigned int), 1, rand_file);

  int range = max - min + 1;
  return ((rand_r(&seed) % range) + min);
}

/**************************************************************************/

/* If t2 is greater or equal to t1, return 1, else return 0 */
// t2 should be current time, t1 should be a cutoff time
int is_time_greater_eq(uint* t1, uint* t2) {
  if (t2[0] > t1[0]) {
    return 1;
  }

  if (t2[0] == t1[0]) {
    if (t2[1] >= t1[1]) {
      return 1;
    }
    return 0;
  }

  return 0;
}

/* Hi */
void set_next_time(uint* current_time, uint* next_time_holder, uint nanoseconds) {
  next_time_holder[0] = current_time[0];
  next_time_holder[1] = current_time[1] + nanoseconds;

  //Carryover check
  if (next_time_holder[1] >= BILLION) {
    next_time_holder[1] = next_time_holder[1] % BILLION;
    next_time_holder[0] += 1;
  }
}

void increment_time(uint* current_time, uint nanoseconds) {
  current_time[1] += nanoseconds;

  if (current_time[1] >= BILLION) {
    current_time[1] = current_time[1] % BILLION;
    current_time[0] += 1;
  }
}

/**************************************************************************/

void construct_msg(req_success_msg* message, int child_mtype, pid_t pid, int page_ind) {
  message->mtype = child_mtype;
  message->contents.PID = pid;
  message->contents.page = page_ind;
  message->contents.status = 1;
}
