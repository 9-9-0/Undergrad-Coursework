#include "proc_funct.h"

/**************************************************************************/

//1 = terminate, 0 = continue
int check_term(FILE* rand_file) {
  if (get_rand(rand_file, 1, 100) <= 50) {
    return 1;
  }
  else {
    return 0;
  }
}

/* Pass in the holder to the # of memory references for the next termination check.
 * Add [900,1100] to it. */
void set_next_check_count(FILE* rand_file, int* next_check_holder) {
  *next_check_holder += get_rand(rand_file, 900, 1100);
}

/**************************************************************************/

void submit_request(CirQueue* queue, int32* req) {
  enQueue(queue, req);
}

void set_request(FILE* rand_file, int32* req_holder, uint* time, int term, int child_ind, int pid) {
  req_holder[0] = pid;
  req_holder[1] = child_ind;

  //Set time of request
  req_holder[4] = time[0];
  req_holder[5] = time[1];

  //Setting a termination request?
  if (term) {
    req_holder[2] = -1;
    req_holder[3] = 2;
  }
  else {
    req_holder[2] = get_req_offset(rand_file);
    req_holder[3] = get_req_action(rand_file);
  }
}

//Gets the byte of the memory reference
int get_req_offset(FILE* rand_file) {
  return get_rand(rand_file, 0, MAX_PROC_PAGES * FRAME_PAGE_SIZE - 1);
}

//1 = Write Operation, 0 = Read Operation
int get_req_action(FILE* rand_file) {
  if (get_rand(rand_file, 1, 100) <= 50) {
    return 1;
  }
  else {
    return 0;
  }
}
