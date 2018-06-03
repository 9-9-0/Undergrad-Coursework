#ifndef PROC_MANAGEMENT_H
#define PROC_MANAGEMENT_H

#include "common_management.h"

#define termination_chance 10   //Percentage
#define term_check_interval 250 //In milleseconds
#define action_check_interval 150 //In milleseconds

void set_next_term_check(FILE* rand_file, uint32_t* current_time, uint32_t* next_check_time);

void set_next_action_check(FILE* rand_file, uint32_t* current_time, uint32_t* next_check_time);

int test_termination(FILE* rand_file);


#endif
