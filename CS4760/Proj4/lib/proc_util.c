#include "proc_util.h"

/**************************************************************************************/

int rand_run_full_slice(FILE* rand_file) {
  unsigned int seed;
  fread(&seed, sizeof(unsigned int), 1, rand_file);

  return (rand_r(&seed) % 2);
}

int rand_percent_slice(FILE* rand_file) {
  unsigned int seed;
  fread(&seed, sizeof(unsigned int), 1, rand_file);

  return ((rand_r(&seed) % 4) + 1);
}

/**************************************************************************************/

void clock_fast_forward(unsigned int* shared_clock, unsigned int* burst_time) {
  //fprintf(stderr, "Received %d:%d\n", burst_time[0], burst_time[1]);
  //fprintf(stderr, "Current %d:%d\n", shared_clock[0], shared_clock[1]);
  shared_clock[0] += burst_time[0];
  shared_clock[1] += burst_time[1];

  if (shared_clock[1] >= billion) {
    shared_clock[1] = shared_clock[1] % billion;
    shared_clock[0] += 1;
  }
  fprintf(stderr, "Fast forwarded time: %d:%d\n", shared_clock[0], shared_clock[1]);
}

/**************************************************************************************/

void calc_turnaround(pc_block* pc_block, unsigned int* current_time) {
  pc_block->turnaround_time[0] = pc_block->total_wait_time[0] + pc_block->total_cpu_usage[0];
  pc_block->turnaround_time[1] = pc_block->total_wait_time[1] + pc_block->total_cpu_usage[1];

  if (pc_block->turnaround_time[1] > billion) {
    pc_block->turnaround_time[0] += 1;
    pc_block->turnaround_time[1] = pc_block->turnaround_time[1] % billion;
  }
  /*
  unsigned int current_time_snap[2];
  current_time_snap[0] = current_time[0], current_time_snap[1] = current_time[1];

  //Put some validation checks in here at some point.
  pc_block->turnaround_time[0] = current_time_snap[0] - pc_block->gen_time[0];

  //Carry over if necessary
  if (current_time_snap[1] < pc_block->gen_time[1]) {
      pc_block->turnaround_time[0] -= 1;
      current_time_snap[1] += billion;
  }

  pc_block->turnaround_time[1] = current_time_snap[1] - pc_block->gen_time[1];
  */
}

void set_actual_burst(pc_block* pc_block, unsigned int* time) {
  pc_block->actual_burst_time[0] = time[0];
  pc_block->actual_burst_time[1] = time[1];

  //Do some validation checking here.
}

void set_actual_burst_part(pc_block* pc_block, unsigned int* time, int rand_percent) {
  //rand_percent will be 1: 25%, 2: 50%, so on
  //This definitely needs refactoring when variable sized quantums are set...
  unsigned int assn_time_snap[2];
  assn_time_snap[0] = time[0];
  assn_time_snap[1] = time[1];

  if (assn_time_snap[0] == 1) {
    assn_time_snap[0] -= 1;
    assn_time_snap[1] += billion;
  }

  pc_block->actual_burst_time[0] = ((assn_time_snap[0] / 4) * rand_percent);
  pc_block->actual_burst_time[1] = ((assn_time_snap[1] / 4) * rand_percent);
}

void set_finished_time(pc_block* pc_block, unsigned int* time) {
  pc_block->last_finished_time[0] = time[0];
  pc_block->last_finished_time[1] = time[1];
}

void run_slice(pc_block* pc_block, unsigned int* shared_clock) {
  pc_block->total_cpu_usage[0] += pc_block->actual_burst_time[0];
  pc_block->total_cpu_usage[1] += pc_block->actual_burst_time[1];

  //Carry-over.
  if (pc_block->total_cpu_usage[1] >= billion) {
    pc_block->total_cpu_usage[1] = pc_block->total_cpu_usage[1] % billion;
    pc_block->total_cpu_usage[0] += 1;
  }

  //Fast forward clock
  clock_fast_forward(shared_clock, pc_block->actual_burst_time);
}
