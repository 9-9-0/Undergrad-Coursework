#include <stdlib.h>
#include <string.h>
#include "common_management.h"


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

void display_msg(res_req_msg* message) {
    fprintf(stderr, "*************************\n");
    fprintf(stderr, "REQUEST MESSAGE:\n");
    fprintf(stderr, "mtype : %ld\n", message->mtype);
    fprintf(stderr, "PID : %d\n", message->contents.PID);
    fprintf(stderr, "Res ID : %d\n", message->contents.res_id);
    fprintf(stderr, "Action : %d\n", message->contents.action);
    fprintf(stderr, "*************************\n");
}

void construct_msg(res_req_msg* message, int child_ind, pid_t pid, uint32_t resource_id, int act) {
  message->mtype = child_ind;
  message->contents.PID = pid;
  message->contents.res_id = resource_id;
  message->contents.action = act;
}

void display_notify_msg(res_resp_msg* message) {
    fprintf(stderr, "*************************\n");
    fprintf(stderr, "mtype : %ld\n", message->mtype);
    fprintf(stderr, "PID : %d\n", message->contents.PID);
    fprintf(stderr, "Res ID : %d\n", message->contents.res_id);
    fprintf(stderr, "*************************\n");
}

void construct_notify_msg(res_resp_msg* message, int child_ind, pid_t pid, uint32_t resource_id) {
  message->mtype = child_ind;
  message->contents.PID = pid;
  message->contents.res_id = resource_id;
}
/**************************************************************************/

/* If t2 is greater or equal to t1, return 1, else return 0 */
// t2 should be current time, t1 should be a cutoff time
int time_is_greater(uint32_t* t1, uint32_t* t2) {
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
void set_next_time(uint32_t* current_time, uint32_t* next_time_holder, uint32_t nanoseconds) {
  next_time_holder[0] = current_time[0];
  next_time_holder[1] = current_time[1] + nanoseconds;

  //Carryover check
  if (next_time_holder[1] >= billion) {
    next_time_holder[1] = next_time_holder[1] % billion;
    next_time_holder[0] += 1;
  }
}
