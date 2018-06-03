#ifndef COMMON_FUNCT_H
#define COMMON_FUNCT_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "common_types.h"
#include "common_macros.h"

/**************************************************************************/

//Error Checking
int errCheck(char progName[], int errnoval);

//Rand # Generation
int get_rand(FILE* rand_file, int min, int max);

//Time Functionality
int is_time_greater_eq(uint* t1, uint* t2);
void set_next_time(uint* current_time, uint* next_time_holder, uint nanoseconds);
void increment_time(uint* current_time, uint nanoseconds);

//Message Queue Related
void construct_msg(req_success_msg* message, int child_mtype, pid_t pid, int page_ind); 

#endif
