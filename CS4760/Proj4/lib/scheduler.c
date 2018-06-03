#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "scheduler.h"
#include "bitmap.h"

/**************************************************************************************/
unsigned int get_rand_nano(FILE* rand_file) {
  unsigned int seed;
  fread(&seed, sizeof(unsigned int), 1, rand_file);

  return ((rand_r(&seed) % nano_range) + 1);
}

/* Returns [0,3]. See specs on pathing decided by this number. */
unsigned int get_rand_action(FILE* rand_file) {
  unsigned int seed;
  fread(&seed, sizeof(unsigned int), 1, rand_file);

  //return ((rand_r(&seed) % 4));
  return ((rand_r(&seed) % 3));
}

unsigned int get_event_rand_sec(FILE* rand_file) {
  unsigned int seed;
  fread(&seed, sizeof(unsigned int), 1, rand_file);

  return ((rand_r(&seed) % 6));
}

unsigned int get_event_rand_nano(FILE* rand_file) {
  unsigned int seed;
  fread(&seed, sizeof(unsigned int), 1, rand_file);

  return ((rand_r(&seed) % 1001));
}
/**************************************************************************************/

/* Increments the current clock time by the nanoseconds passed in as an argument */
void clock_increment_time(unsigned int* input_time, unsigned int nanoseconds) {
  input_time[1] += nanoseconds;
  if (input_time[1] >= billion) {
    input_time[1] = input_time[1] % billion;
    input_time[0] += 1;
  }
}

/* Returns 1 if current_time has passed the time stored in next. 0 Otherwise */
int clock_is_time_up(unsigned int* current, unsigned int* next) {
  if (current[0] > next[0]) {
    return 1;
  }
  else if (current[0] == next[0]) {
    if (current[1] >= next[1]) {
      return 1;
    }
    return 0;
  }
  else { //current0] < next[0]
    return 0;
  }
}

void clock_set_next_gen_time(unsigned int* current_time, unsigned int* next_gen_time, FILE* rand_file) {
  unsigned int seed;
  fread(&seed, sizeof(unsigned int), 1, rand_file);

  unsigned int rand_secs = (rand_r(&seed) % proc_gen_range);
  //fprintf(stderr, "RAND SECS: %d\n", rand_secs);

  next_gen_time[0] = current_time[0] + rand_secs;
  next_gen_time[1] = current_time[1];

  //fprintf(stderr, "NEXT GEN TIME: %d:%d\n", next_gen_time[0], next_gen_time[1]);
}

/**************************************************************************************/

/* Returns 1 if bitmap is not filled, 0 otherwise. */
int bm_check_free_proc(int bitmap) {
  if (bitmap != 524287) { //If bits 0-18 are all filled.
    return 1;
  }
  return 0;
}

/* Determines the first free bit in the bitmap passed in as an argument. */
int bm_get_free_ind(int bitmap) {
  //Can swap this out for a mod operation.
  for (int i = 0; i < 19; i++) {
    if (!TestBit(bitmap, i)) {
      return i;
    }
  }
  return -1; //This should never return as at least 1 should exist.
}

/**************************************************************************************/

/* Cycles through the process control blocks. If a process is waiting, its wait time is adjusted based on the current time */
void pc_update_wait_times(pc_block* pc_blocks, unsigned int* current_time) {
  unsigned int current_time_snap[2];

  for (int i = 0; i < max_child_procs; i++) {
    current_time_snap[0] = current_time[0];
    current_time_snap[1] = current_time[1];

    if (pc_blocks[i].pid == -1) continue;

    //If the process is not running, update its wait time.
    if (pc_blocks[i].status == 0) {
      if (current_time_snap[1] < pc_blocks[i].total_cpu_usage[1]) {
        current_time_snap[0] -= 1;
        current_time_snap[1] += billion;
      }
      pc_blocks[i].total_wait_time[0] = current_time_snap[0] - pc_blocks[i].total_cpu_usage[0];
      pc_blocks[i].total_wait_time[1] = current_time_snap[1] - pc_blocks[i].total_cpu_usage[1];

      if (pc_blocks[i].total_wait_time[1] < pc_blocks[i].gen_time[1]) {
        pc_blocks[i].total_wait_time[0] -= 1;
        pc_blocks[i].total_wait_time[1] += billion;
      }
      pc_blocks[i].total_wait_time[0] = pc_blocks[i].total_wait_time[0] - pc_blocks[i].gen_time[0];
      pc_blocks[i].total_wait_time[1] = pc_blocks[i].total_wait_time[1] - pc_blocks[i].gen_time[1];
      //If a process has not been dispatched, use its generated time to calculate wait times
      /*
      if (pc_blocks[i].times_dispatched == 0) {
        if (pc_blocks[i].gen_time[1] > current_time_snap[1]) {
          current_time_snap[0] -= 1;
          current_time_snap[1] += billion;
        }
        pc_blocks[i].total_wait_time[0] = current_time_snap[0] - pc_blocks[i].gen_time[0];
        pc_blocks[i].total_wait_time[1] = current_time_snap[1] - pc_blocks[i].gen_time[1];
      }
      */
      //If a process has been dispatched, calculate wait time using total cpu usage and gen time
      /*
      else {
      }
      */
    }
  }
}

/* Used exclusively for the blocked processes section....*/
void pc_update_wait_time_single(pc_block* pc_block, unsigned int* current_time) {
  unsigned int current_time_snap[2];
  current_time_snap[0] = current_time[0], current_time_snap[1] = current_time[1];

  if (pc_block->total_cpu_usage[1] > current_time_snap[1]) {
    current_time_snap[0] -= 1;
    current_time_snap[1] += billion;
  }
  pc_block->total_wait_time[0] = current_time_snap[0] - pc_block->total_cpu_usage[0];
  pc_block->total_wait_time[1] = current_time_snap[1] - pc_block->total_cpu_usage[1];
}

/* All blocks are marked as empty (a pid of -1 and queue value of -1) */
void pc_init_empty_blocks(pc_block* pc_blocks) {
  for (int i = 0; i < max_child_procs; i++) {
    pc_blocks[i].bm_ind = -1;
    pc_blocks[i].status = -1;
    pc_blocks[i].pid = -1;
    pc_blocks[i].queue = -1;
  }
}

/* Initializes a pc block for a single process. Corresponding bit in bitmap is marked as filled. */
pc_block* pc_init_control_block(long pid, int index, pc_block* block_arr, int* bitmap, unsigned int* sys_clock) {
  pc_block* single_block_ptr;
  SetBit(*bitmap, index);

  single_block_ptr = &block_arr[index];

  //Allocate block values
  single_block_ptr->total_cpu_usage[0] = 0, single_block_ptr->total_cpu_usage[1] = 0;
  single_block_ptr->turnaround_time[0] = 0, single_block_ptr->turnaround_time[1] = 0;
  single_block_ptr->last_burst_use_time[0] = 0, single_block_ptr->last_burst_use_time[1] = 0;
  single_block_ptr->total_wait_time[0] = 0, single_block_ptr->total_wait_time[1] = 0;
  single_block_ptr->last_finished_time[0] = 0, single_block_ptr->last_finished_time[1] = 0;
  single_block_ptr->times_dispatched = 0;

  single_block_ptr->gen_time[0] = sys_clock[0], single_block_ptr->gen_time[1] = sys_clock[1];
  single_block_ptr->status = 0;
  single_block_ptr->bm_ind = index;
  single_block_ptr->pid = pid;
  single_block_ptr->queue = -1; //Remains -1 until inserted into queue 0

  return single_block_ptr;
}

/* Resets the values for a pc_block and clears its bit from the bitmap */
long pc_empty_single_block(pc_block* pc_block) {
  long return_pid = pc_block->pid;

  //Clear meta info
  pc_block->bm_ind = -1;
  pc_block->pid = -1;
  pc_block->status = -1;
  pc_block->queue = -1;

  //Clear Metrics
  pc_block->gen_time[0] = 0, pc_block->gen_time[1] = 0;
  pc_block->total_wait_time[0] = 0, pc_block->total_wait_time[1] = 0;

  //Unused Stuff for now NOTE: clear out later.
  pc_block->total_cpu_usage[0] = 0, pc_block->total_cpu_usage[1] = 0;
  pc_block->last_burst_use_time[0] = 0, pc_block->last_burst_use_time[1] = 0;
  pc_block->last_wait_time[0] = 0, pc_block->last_wait_time[1] = 0;
  pc_block->last_finished_time[0] = 0, pc_block->last_finished_time[1] = 0;

  return return_pid;
}

/* Stores the wait time and turnaround time of the terminated process into the array passed in */
void pc_harvest_metrics(unsigned int* metric_holders, int index, pc_block* pc_blocks) {
  metric_holders[0] = pc_blocks[index].total_wait_time[0];
  metric_holders[1] = pc_blocks[index].total_wait_time[1];
  metric_holders[2] = pc_blocks[index].turnaround_time[0];
  metric_holders[3] = pc_blocks[index].turnaround_time[1];
  metric_holders[4] = pc_blocks[index].total_cpu_usage[0];
  metric_holders[5] = pc_blocks[index].total_cpu_usage[1];
}

/**************************************************************************************/

void sch_set_next_disp(unsigned int* next_disp_time, unsigned int* current_time, FILE* rand_file) {
  next_disp_time[0] = current_time[0] + get_event_rand_sec(rand_file);
  next_disp_time[1] = current_time[1] + get_event_rand_nano(rand_file);
}

/* Moves a process into queue 0 by modifying its block value and by pushing its pid into queue 0. */
void sch_set_last_idle_time(unsigned int* time_holder, unsigned int* current_time) {
  time_holder[0] = current_time[0];
  time_holder[1] = current_time[1];
}

void sch_add_idle_time(unsigned int* last_idle, unsigned int* current_time, unsigned int* total_idle_holder) {
    unsigned int current_time_snap[2];
    current_time_snap[0] = current_time[0];
    current_time_snap[1] = current_time[1];

    if (current_time_snap[1] < last_idle[1]) {
      current_time_snap[0] -= 1;
      current_time_snap[1] += billion;
    }

    total_idle_holder[0] += (current_time_snap[0] - last_idle[0]);
    total_idle_holder[1] += (current_time_snap[1] - last_idle[1]);

    if (total_idle_holder[1] >= billion) {
      total_idle_holder[1] = total_idle_holder[1] % billion;
      total_idle_holder[0] += 1;
    }
}

void schedule_process(long pid, deque_type* top_queue, pc_block* block) {
  deque_push_back(top_queue, pid);
  block->queue = 0;
  block->status = 0;
}

void reschedule_process(pc_block* block, deque_type* queue[3]) {
  if (block->queue == 0) {
    deque_push_back(queue[0], block->pid);
    return;
  }
  if (block->queue == 1) {
    deque_push_back(queue[1], block->pid);
    return;
  }
  if (block->queue == 2) {
    deque_push_back(queue[2], block->pid);
    return;
  }
}

void sch_update_proc_metrics(unsigned int* totals, unsigned int* temp) {
  for (int i = 0; i < 6; i++) {
    totals[i] += temp[i];
  }
  //Check for carry-overs (see pc_harvest_metrics for which elements are nanosecond holders)
  if (totals[1] > billion) {
    totals[1] = totals[1] % billion;
    totals[0] += 1;
  }
  if (totals[3] > billion) {
    totals[3] = totals[3] % billion;
    totals[2] += 1;
  }
  if (totals[5] > billion) {
    totals[5] = totals[5] % billion;
    totals[4] += 1;
  }
}

unsigned int* sch_calc_avg_wait(unsigned int* avg_wait_holder, unsigned int* total_wait, unsigned int procs) {
  if (procs == 0) {
    fprintf(stderr, "Unable to calculate average wait time of terminated process. %d # of processes received.\n", procs);
    avg_wait_holder[0] = 0;
    avg_wait_holder[1] = 0;
    return avg_wait_holder;
  }

  avg_wait_holder[0] = total_wait[0] / procs;
  avg_wait_holder[1] = total_wait[1] / procs;
  return avg_wait_holder;
}

unsigned int* sch_calc_turnaround(unsigned int* avg_turnaround_holder, unsigned int* total_turnaround, unsigned int procs) {
  if (procs == 0) {
    fprintf(stderr, "Unable to calculate average turnaround time of terminated process. %d # of processes received.\n", procs);
    avg_turnaround_holder[0] = 0;
    avg_turnaround_holder[1] = 0;
    return avg_turnaround_holder;
  }

  avg_turnaround_holder[0] = total_turnaround[0] / procs;
  avg_turnaround_holder[1] = total_turnaround[1] / procs;
  return avg_turnaround_holder;
}

unsigned int* sch_calc_avg_cpu(unsigned int* avg_cpu_holder, unsigned int* total_cpu_use, unsigned int procs) {
  if (procs == 0) {
    fprintf(stderr, "Unable to calculate average cpu usage of terminated process. %d # of processes received.\n", procs);
    avg_cpu_holder[0] = 0;
    avg_cpu_holder[1] = 0;
    return avg_cpu_holder;
  }
  avg_cpu_holder[0] = total_cpu_use[0] / procs;
  avg_cpu_holder[1] = total_cpu_use[1] / procs;
  return avg_cpu_holder;
}
/**************************************************************************************/

/* Updates the average wait times of each queue using values stored within the pc_blocks */
//NOTE: Needs testing...
void q_update_avg_times(unsigned int avg_wait_times[3][2], pc_block* pc_blocks) {
  unsigned int num_procs[3] = {0, 0, 0};
  unsigned int total_wait_times[3][2];
  //They don't all get set to zero...
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 2; y++) {
      total_wait_times[x][y] = 0;
    }
  }

  for (int i = 0; i < max_child_procs; i++) {
    if (pc_blocks[i].pid != -1 && pc_blocks[i].status == 0) {
      if (pc_blocks[i].queue == 0) {
        num_procs[0] += 1;
        total_wait_times[0][0] += pc_blocks[i].total_wait_time[0];
        total_wait_times[0][1] += pc_blocks[i].total_wait_time[1];

        //Check for carry-overs
        if (total_wait_times[0][1] >= billion) {
          total_wait_times[0][1] = total_wait_times[0][1] % billion;
          total_wait_times[0][0] += 1;
        }
        //fprintf(stderr, "TOTALS: %d, %d, %d\n", total_wait_times[0][0], total_wait_times[0][1], num_procs[0]);
        continue;
      }
      if (pc_blocks[i].queue == 1) {
        num_procs[1] += 1;
        total_wait_times[1][0] += pc_blocks[i].total_wait_time[0];
        total_wait_times[1][1] += pc_blocks[i].total_wait_time[1];

        //Check for carry-overs
        if (total_wait_times[1][1] >= billion) {
          total_wait_times[1][1] = total_wait_times[1][1] % billion;
          total_wait_times[1][0] += 1;
        }
        //fprintf(stderr, "TOTALS: %d, %d, %d\n", total_wait_times[0][0], total_wait_times[0][1], num_procs[0]);
        continue;
      }
      if (pc_blocks[i].queue == 2) {
        num_procs[2] += 1;
        total_wait_times[2][0] += pc_blocks[i].total_wait_time[0];
        total_wait_times[2][1] += pc_blocks[i].total_wait_time[1];

        //Check for carry-overs
        if (total_wait_times[2][1] >= billion) {
          total_wait_times[2][1] = total_wait_times[1][1] % billion;
          total_wait_times[2][0] += 1;
        }
        //fprintf(stderr, "TOTALS: %d, %d, %d\n", total_wait_times[0][0], total_wait_times[0][1], num_procs[0]);
        continue;
      }
    }
  }

  //Calculate Averages
  for (int j = 0; j < 3; j++) {
    if (num_procs[j] > 0) {
      avg_wait_times[j][0] = total_wait_times[j][0] / num_procs[j];
      avg_wait_times[j][1] = total_wait_times[j][1] / num_procs[j];
    }
  }
}

/* Based on the thresholds and average wait times of each queue, processes are reassigned queues through their pc_block and
pids are moved between queues. */
//NOTE: Seems to be working.
void q_update_queues(FILE* log, unsigned int* current_time, deque_type* queues[3], pc_block* pc_blocks, unsigned int avg_wait_times[3][2], unsigned int alpha, unsigned int beta) {
  unsigned int temp_secs;
  unsigned int temp_nanos;
  unsigned int sec_snap = current_time[0];
  unsigned int nano_snap = current_time[1];

  int returned_pid = 0;

  //Iterate through the pc_blocks and reassign queues
  for (int i = 0; i < max_child_procs; i++) {
    if (pc_blocks[i].status != 0) {
      //fprintf(stderr, "SKIPPING BLOCK REASSIGNMENT %d\n", i);
      continue;
    }

    //Reset temps
    temp_secs = 0;
    temp_nanos = 0;

    if (pc_blocks[i].queue == 0) {
      //Check if can be moved from 0 to 1
      //First, check if greater than threshold.
      if (pc_blocks[i].total_wait_time[0] >= q1_thresh_secs) {
        temp_secs = alpha * avg_wait_times[1][0];
        //Manually multiply...
        for (int x = 0; x < alpha; x++) {
          temp_nanos += avg_wait_times[1][1];
          if (temp_nanos >= billion) {
            temp_nanos = temp_nanos % billion;
            temp_secs += 1;
          }
        }
        //Compare to alpha * average wait times of q1 processes
        if (pc_blocks[i].total_wait_time[0] > temp_secs) {
          pc_blocks[i].queue = 1;
          returned_pid = deque_pop_pid(queues[0], pc_blocks[i].pid);
          if (returned_pid == -1) {
            fprintf(stderr, "Your deque code comes back to haunt you.\n");
            exit(1);
          }
          deque_push_back(queues[1], pc_blocks[i].pid);
          log_queue_move(log, pc_blocks[i].pid, 0, 1, sec_snap, nano_snap);
          continue;
        }
        if (pc_blocks[i].total_wait_time[0] == temp_secs) {
          if (pc_blocks[i].total_wait_time[1] >= temp_nanos) {
            pc_blocks[i].queue = 1;
            returned_pid = deque_pop_pid(queues[0], pc_blocks[i].pid);
            if (returned_pid == -1) {
              fprintf(stderr, "Your deque code comes back to haunt you.\n");
              exit(1);
            }
            deque_push_back(queues[1], pc_blocks[i].pid);
            log_queue_move(log, pc_blocks[i].pid, 0, 1, sec_snap, nano_snap);
            continue;
          }
        }
      }
      continue;
    }

    if (pc_blocks[i].queue == 1) {
      //Check if can be moved from 1 to 2
      //First, check if greater than threshold.
      if (pc_blocks[i].total_wait_time[0] >= q2_thresh_secs) {
        //Check if greater than beta * average wait time of q2 processes
        temp_secs = alpha * avg_wait_times[2][0];
        //Manually multiply...
        for (int x = 0; x < beta; x++) {
          temp_nanos += avg_wait_times[2][1];
          if (temp_nanos >= billion) {
            temp_nanos = temp_nanos % billion;
            temp_secs += 1;
          }
        }
        //Compare to beta * average wait times of q1 processes
        if (pc_blocks[i].total_wait_time[0] > temp_secs) {
          pc_blocks[i].queue = 2;
          returned_pid = deque_pop_pid(queues[1], pc_blocks[i].pid);
          if (returned_pid == -1) {
            fprintf(stderr, "Your deque code comes back to haunt you.\n");
            exit(1);
          }
          deque_push_back(queues[2], pc_blocks[i].pid);
          log_queue_move(log, pc_blocks[i].pid, 1, 2, sec_snap, nano_snap);
          continue;
        }
        if (pc_blocks[i].total_wait_time[0] == temp_secs) {
          if (pc_blocks[i].total_wait_time[1] >= temp_nanos) {
            pc_blocks[i].queue = 2;
            returned_pid = deque_pop_pid(queues[1], pc_blocks[i].pid);
            if (returned_pid == -1) {
              fprintf(stderr, "Your deque code comes back to haunt you.\n");
              exit(1);
            }
            deque_push_back(queues[2], pc_blocks[i].pid);
            log_queue_move(log, pc_blocks[i].pid, 1, 2, sec_snap, nano_snap);
            continue;
          }
        }
      }
      continue;
    }
  }
}

/**************************************************************************************/

/* Returns the "highest priority" pid, or -1 if all queues are empty. */
//NOTE: NEEDS TESTING
long d_get_head_pid(deque_type* queues[3]) {
  //Start from bottom queue on up.
  for (int i = 2; i >= 0; i--) {
    //Check if queue is empty. If it is, move up.
    if (deque_is_empty(queues[i]) == true) {
      continue;
    }

    //Pop the first.
    return deque_pop_front(queues[i]);
  }
  return -1;
}

int d_get_pid_ind(long pid, pc_block* pc_blocks) {
  for (int i = 0; i < max_child_procs; i++) {
    if (pc_blocks[i].pid == pid) {
      return i;
    }
  }
  return -1;
}
/* Sets the status flag of a process' pc_block to running */
long d_dispatch_pid(long pid, pc_block* pc_blocks) {
  for (int i = 0; i < max_child_procs; i++) {
    //Modify the pc_block's status flag to running (1).
    if (pc_blocks[i].pid == pid) {
      pc_blocks[i].status = 1;
      //Remove from queue

      return pid;
    }
  }
  return -1;
}

/* Marks the child's status as terminated. Returns the pid's pc_block index if successful, -1 if not */
int d_terminate_pid(long pid, pc_block* pc_blocks) {
  for (int i = 0; i < max_child_procs; i++) {
    if (pc_blocks[i].pid == pid) {
      pc_blocks[i].status = 3;
      return i;
    }
  }
  return -1;
}

long d_assign_quantum(pc_block* pc_block, unsigned int quantum[3][2]) {
  if (pc_block->queue == 0) {
    pc_block->assigned_burst_time[0] = quantum[0][0];
    pc_block->assigned_burst_time[1] = quantum[0][1];
    return 0;
  }
  else if (pc_block->queue == 1) {
    pc_block->assigned_burst_time[0] = quantum[1][0];
    pc_block->assigned_burst_time[1] = quantum[1][1];
    return 1;
  }
  else if (pc_block->queue == 2) {
    pc_block->assigned_burst_time[0] = quantum[2][0];
    pc_block->assigned_burst_time[1] = quantum[2][1];
    return 2;
  }
  else {
    return -1;
  }
}

long d_proc_run_full_q(long pid, pc_block* pc_blocks, unsigned int quantum[3][2]) {
  for (int i = 0; i < max_child_procs; i++) {
    if (pc_blocks[i].pid == pid) {
      pc_blocks[i].status = 1;
      pc_blocks[i].times_dispatched += 1;
      return d_assign_quantum(&pc_blocks[i], quantum);
    }
  }
  return -1;
}

/* Resets the values for a terminated process' control block and clears the corresponding bit in bitmap. Returns the  */
long d_clear_pid_info(int pid_ind, pc_block* pc_blocks, int* bitmap) {
  ClearBit(*bitmap, pid_ind); //Clear the bit
  return pc_empty_single_block(&pc_blocks[pid_ind]); //Return the PID
}

/**************************************************************************************/

void log_generation(FILE* logfile, long pid, unsigned int seconds, unsigned int nanos) {
  fprintf(logfile, "OSS: (T-%9d:%-9d) Generating Process (PID %ld) and inserting into queue 0.\n", seconds, nanos, pid);
}

void log_queue_move(FILE* logfile, long pid, int from_q, int to_q, unsigned int seconds, unsigned int nanos) {
  fprintf(logfile, "OSS: (T-%9d:%-9d) Moving Process (PID %ld) from queue %d to queue %d.\n", seconds, nanos, pid, from_q, to_q);
}

void log_select_proc(FILE* logfile, long pid, unsigned int seconds, unsigned int nanos) {
  fprintf(logfile, "OSS: (T-%9d:%-9d) Scheduler has selected (PID %ld) for dispatch.\n", seconds, nanos, pid);
}

void log_term_proc(FILE* logfile, long pid, unsigned int seconds, unsigned int nanos) {
  fprintf(logfile, "OSS: (T-%9d:%-9d) Process %ld is simulating termination.\n", seconds, nanos, pid);
}

void log_run_proc(FILE* logfile, long pid, int queue, unsigned int quantum[2], unsigned int* current_time) {
  fprintf(logfile, "OSS: (T-%9d:%-9d) Process %ld from queue %d has been scheduled to run for %d:%d.\n", current_time[0], current_time[1], pid, queue, quantum[0], quantum[1]);
}

void log_proc_return(FILE* logfile, long pid, unsigned int assgn_quantum[2], unsigned int actual_quantum[2], unsigned int* current_time) {
  if (assgn_quantum[0] == actual_quantum[0] && assgn_quantum[1] == actual_quantum[1]) {
    fprintf(logfile, "OSS: (T-%9d:%-9d) Process %ld ran for its entire quantum %d:%d.\n", current_time[0], current_time[1], pid, actual_quantum[0], actual_quantum[1]);
  }
  else {
    fprintf(logfile, "OSS: (T-%9d:%-9d) Process %ld ran for a partial quantum %d:%d.\n", current_time[0], current_time[1], pid, actual_quantum[0], actual_quantum[1]);
  }
}

void log_reschedule(FILE* logfile, long pid, unsigned int queue, unsigned int* current_time) {
  fprintf(logfile, "OSS: (T-%9d:%-9d) Process %ld reinserted into queue %d.\n", current_time[0], current_time[1], pid, queue);
}

void log_scheduler_metrics(FILE* logfile, unsigned int* avgwait, unsigned int* turnaround, unsigned int* avgcpu, unsigned int* idle) {
  fprintf(logfile, "\n------------------------------\n");
  fprintf(logfile, "------SCHEDULER METRICS-------\n");
  fprintf(logfile, "------------------------------\n\n");
  fprintf(logfile, "Average Wait Time: %d:%d\n", avgwait[0], avgwait[1]);
  fprintf(logfile, "Average Turnaround Time: %d:%d\n", turnaround[0], turnaround[1]);
  //fprintf(logfile, "Average CPU Use Time: %d:%d\n", avgcpu[0], avgcpu[1]); //Fix truncation issues before printing this...
  fprintf(logfile, "Scheduler Idle Time: %d:%d\n\n", idle[0], idle[1]);

  //Print to stderr
  fprintf(stderr, "\n------------------------------\n");
  fprintf(stderr, "------SCHEDULER METRICS-------\n");
  fprintf(stderr, "------------------------------\n\n");
  fprintf(stderr, "Average Wait Time: %d:%d\n", avgwait[0], avgwait[1]);
  fprintf(stderr, "Average Turnaround Time: %d:%d\n", turnaround[0], turnaround[1]);
  //fprintf(stderr, "Average CPU Use Time: %d:%d\n", avgcpu[0], avgcpu[1]); //Fix truncation issues before printing this...
  fprintf(stderr, "Scheduler Idle Time: %d:%d\n\n", idle[0], idle[1]);
}

void log_next_disp(FILE* logfile, unsigned int* next_disp, unsigned int* current_time, long pid) {
  fprintf(logfile, "OSS: (T-%9d:%-9d) Waiting until %d:%d to dispatch process %ld.\n", current_time[0], current_time[1], next_disp[0], next_disp[1], pid);
}

void log_redispatch(FILE* logfile, unsigned int* current_time, long pid) {
  fprintf(logfile, "OSS: (T-%9d:%-9d) Redispatching process %ld.\n", current_time[0], current_time[1], pid);
}
