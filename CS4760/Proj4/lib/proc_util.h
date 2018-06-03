#ifndef proc_util_h__
#define proc_util_h__

#include <stdio.h>
#include <stdlib.h>
#include "pc_block.h"

#define max_child_procs 19 //Make sure this is equal to the one set in scheduler.h (figure out how to do this without hardcoding)
#define billion 1000000000

extern int rand_run_full_slice(FILE* rand_file);

extern int rand_percent_slice(FILE* rand_file);

extern void clock_fast_forward(unsigned int* shared_clock, unsigned int* burst_time);

extern void calc_turnaround(pc_block* pc_block, unsigned int* current_time);

extern void set_actual_burst(pc_block* pc_block, unsigned int* time);

extern void set_actual_burst_part(pc_block* pc_block, unsigned int* time, int rand_percent);

extern void set_finished_time(pc_block* pc_block, unsigned int* time);

extern void run_slice(pc_block* pc_block, unsigned int* shared_clock);

#endif
