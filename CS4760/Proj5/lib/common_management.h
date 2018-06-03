#ifndef COMMON_MANAGEMENT_H
#define COMMON_MANAGEMENT_H

#define billion 1000000000
#define million 1000000
#define max_resource_types 20

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

//Structures for resource requests

typedef struct {
  pid_t PID;
  uint32_t res_id;
  int action; //0 for request, 1 for release, 2 for termination
} req_contents;

typedef struct {
  long mtype;
  req_contents contents;
} res_req_msg;

//Structures for OSS -> proc response (yes, they're pretty much the same. Used for readability)

typedef struct {
  pid_t PID;
  uint32_t res_id;
} resp_contents;

typedef struct {
  long mtype;
  resp_contents contents;
} res_resp_msg;

/**************************************************************************/

//Error Checking
int errCheck(char progName[], int errnoval);

//Generalized RNG
int get_rand(FILE* rand_file, int min, int max);

//Message Queue
void display_msg(res_req_msg* message);
void construct_msg(res_req_msg* message, int child_ind, pid_t pid, uint32_t resource_id, int act);
void display_notify_msg(res_resp_msg* message);
void construct_notify_msg(res_resp_msg* message, int child_ind, pid_t pid, uint32_t resource_id);

//Time Functionality
int time_is_greater(uint32_t* t1, uint32_t* t2);
void set_next_time(uint32_t* current_time, uint32_t* next_time_holder, uint32_t milleseconds);


#endif
