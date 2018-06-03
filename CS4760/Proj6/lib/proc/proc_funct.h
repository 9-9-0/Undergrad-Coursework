#include "../common/common_funct.h"
#include "../common/circ_queue.h"

/**************************************************************************/

//50/50 chance of returning 1, i.e. terminate
int check_term(FILE* rand_file);

void set_next_check_count(FILE* rand_file, int* next_check_holder);

/**************************************************************************/

void submit_request(CirQueue* queue, int32* req);

void set_request(FILE* rand_file, int32* req_holder, uint* time, int term, int child_ind, int pid);

int get_req_offset(FILE* rand_file);

int get_req_action(FILE* rand_file);
