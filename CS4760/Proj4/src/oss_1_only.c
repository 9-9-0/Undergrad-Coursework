#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <limits.h>
#include "../lib/bitmap.h"
#include "../lib/parse_opt.h"
#include "../lib/deque.h"
#include "../lib/scheduler.h" //pc_block type is included here

void init_queues();
void free_queues();

void exitIfError(char prog_name[]);
void cleanup();
void closeFiles();
void SHMDTRM();
void killSIGINT();
void killSIGTERM();
void notifyEnd();

//Shared Memory
int shm_blocks_id;
int shm_clock_id;
int shm_rqueue_id;
void* shm_blocks_addr;
void* shm_clock_addr;
void* shm_rqueue_addr;

//Semaphore
int sem_proc_id; //Used for coordinating OSS with process return
int sem_stat_id; //Used for coordinating processes with pc_block flags
union semun {
    int val;                 /* used for SETVAL only */
    //struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    //ushort *array;         /* used for GETALL and SETALL */
};

//Input Variables
unsigned int alpha = 0;
unsigned int beta = 0;
int q = 1; //
unsigned int quantum[3][2] = {
  {1, 0},
  {0, halfbillion},
  {0, quarterbillion}
};
unsigned int max_run_time = 0;

//File Pointers
FILE* log_fp = NULL;
FILE* rand_fp = NULL; //Pointer to /dev/random

//PC + BitMap Variables
int bit_map = 0;
pc_block* all_blocks_ptr;
pc_block* one_block_ptr;

//Queues
//deque_type* ready_queue; //Turn this into a deque_type if you have time, otherwise a long[19] should suffice.
long* ready_queue;
deque_type* proc_queue[3];
unsigned int avg_wait_times[3][2] = {
  {0, 0},
  {0, 0},
  {0, 0}
};

//Scheduler Metrics Variables
unsigned int total_idle_time[2] = {0, 0};
unsigned int avg_proc_wait[2] = {0, 0}; //NOT the average wait times of a queue
unsigned int avg_turnaround[2] = {0, 0};
unsigned int avg_proc_cpu[2] = {0, 0};
unsigned int total_proc_term = 0; //Used to calculate the averages. Total procs scheduled.
unsigned int metric_totals[6] = {0, 0, 0, 0, 0, 0}; //0,1 for wait totals, 2,3 for turnaround, 4,5 for cpu usage
unsigned int total_proc_cpu_use[2];
unsigned int total_proc_wait[2];
unsigned int total_proc_turn[2];

int main(int argc, char* argv[]) {
  //Signal Handlers
  signal(SIGINT, killSIGINT);
  signal(SIGALRM, killSIGTERM);

  //getopt() setup
  static char usage[] = "%s: Usage: %s [-a queue 1 threshold multiplier] [-b queue 2 threshold multiplier]\n\
                        [-q base time slice quantum] [-l log file location] [-t max run time]\n\
                        [-h help]\n";
                        //One day, set up pretty formatting. Look into the justify options of printf.
  static char help[] = "Read the README.\n";
  int c;
  extern char *optarg;
  extern int optind;

  //Option Parsing
  while ((c = getopt(argc, argv, "ha:b:q:l:t:")) != -1) {
    exitIfError(argv[0]);
    switch (c) {
      case 'h':
        ;
        getHelp(argv[0], usage, help);
        break;
      case 'a':
        ;
        if (isNumber(optarg))
          alpha = (int)strtol(optarg, NULL, 0);
          if (alpha < 1)
            errno = EINVAL;
        break;
      case 'b':
        ;
        if (isNumber(optarg))
          beta = (int)strtol(optarg, NULL, 0);
          if (beta < 1)
            errno = EINVAL;
        break;
      case 'q':
        ;
        if (isNumber(optarg))
          q = (int)strtol(optarg, NULL, 0);
          if (q < 1)
            errno = EINVAL;
        break;
      case 'l':
        ;
        log_fp = fopen(optarg, "w");
        setbuf(log_fp, NULL);
        break;
      case 't':
        ;
        if (isNumber(optarg))
          max_run_time = (int)strtol(optarg, NULL, 0);
          if (max_run_time < 1)
            errno = EINVAL;
        break;
      case '?':
        badUse(argv[0], usage);
        break;
    }
  }
  rand_fp = fopen("/dev/urandom", "r");
  exitIfError(argv[0]);

  if (alpha == 0 || beta == 0 || q == 0 || max_run_time == 0 ||  log_fp == NULL) {
    fprintf(stderr, "Missing required arguments.\n");
    badUse(argv[0], usage);
  }
  alarm(max_run_time); //Set Timeout

  //Shared Memory Setup
  key_t shm_clock_key = ftok("./dummyfile", 1);
  key_t shm_blocks_key = ftok("./dummyfile", 2);
  key_t shm_rqueue_key = ftok("./dummyfile", 3);

  shm_clock_id = shmget(shm_clock_key, sizeof(unsigned int) * 2, IPC_CREAT | 0644);
  shm_blocks_id = shmget(shm_blocks_key, sizeof(pc_block) * 19, IPC_CREAT | 0644);
  shm_rqueue_id = shmget(shm_rqueue_key, sizeof(long) * 19, IPC_CREAT | 0644);
  exitIfError(argv[0]);
  shm_clock_addr = shmat(shm_clock_id, (void*)0, 0);
  shm_blocks_addr = shmat(shm_blocks_id, (void*)0, 0);
  shm_rqueue_addr = shmat(shm_rqueue_id, (void*)0, 0);
  exitIfError(argv[0]);

  //Semaphore Setup
  key_t sem_proc_key = ftok("./dummyfile", 4);
  sem_proc_id = semget(sem_proc_key, max_child_procs, IPC_CREAT | 0666);
  exitIfError(argv[0]);
  union semun arg;
  arg.val = 0;
  for (int i = 0; i < max_child_procs; i++) {
    semctl(sem_proc_id, i, SETVAL, arg);
  }
  struct sembuf sb;
  sb.sem_flg = 0;
  exitIfError(argv[0]);

  //Process Blocks Setup
  all_blocks_ptr = shm_blocks_addr;
  pc_init_empty_blocks(all_blocks_ptr);

  /*
  //Initialize these to zero just in case.
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      avg_wait_times[i][j] = 0;
    }
    fprintf(stderr, "Average Wait Time of Queue %d: %d:%d\n", i, avg_wait_times[i][0], avg_wait_times[i][1]);
  }
  */

  //Ready Queue Setup (Unused for now)
  ready_queue = shm_rqueue_addr;
  for (int i = 0; i < max_child_procs; i++) {
    ready_queue[i] = -1;
  }

  //Multilevel Feedback Queue Setup
  init_queues();
  exitIfError(argv[0]);

  //Clock and Clock Snapshot Setup (0 = seconds, 1 = nanoseconds)
  unsigned int* sim_clock = shm_clock_addr;
  sim_clock[0] = 0, sim_clock[1] = 0;
  unsigned int next_gen_time[2] = { 0, 0};

  //Main Scheduler Loop Variables
  pid_t pid;
  int pid_ind;
  char pid_ind_str[2];

  //Dispatch Dynamic Temp Variables
  long dispatched_pid;
  int dispatched_pid_ind;
  long err_check;
  int term_pid_ind = 0;
  unsigned int term_time_store[6] = {0, 0, 0, 0, 0, 0};
  unsigned int last_idle_time[2] = {0, 0};
  int is_blocked = 0; //Not exactly the best variable name for this, but this is for if a dispatched proc has to wait for an event
  unsigned int next_disp_time[2];


  int total_proc_gen = 0;
  int spawned_proc = 0;
  //Enter Main Scheduler Loop
  while (1) {
    spawned_proc = 0;
    fprintf(stderr,"*****************************************************\n");
    fprintf(stderr, "CURRENT TIME: %d:%d\n", sim_clock[0], sim_clock[1]);
    //fprintf(stderr, "CURRENT PROC GEN: %d\n", total_proc_gen);

    if (total_proc_gen >= 19) {
      break;
    }

    //Update queues: calculate wait times, redistribute processes
    pc_update_wait_times(all_blocks_ptr, sim_clock);
    q_update_avg_times(avg_wait_times, all_blocks_ptr);
    q_update_queues(log_fp, sim_clock, proc_queue, all_blocks_ptr, avg_wait_times, alpha, beta);

    //Process Generation Pathing (Simulates processes requesting CPU time)
    if (clock_is_time_up(sim_clock, next_gen_time)) {
      //If a process is not able to be generated, this will be checked with each loop iteration until one is.
      if (bm_check_free_proc(bit_map)) {
        clock_set_next_gen_time(sim_clock, next_gen_time, rand_fp);

        pid_ind = bm_get_free_ind(bit_map); //Get the next free index.

        //Set process arguments
        sprintf(pid_ind_str, "%d", pid_ind);

        pid = fork();
        exitIfError(argv[0]);

        spawned_proc = 1;

        if (pid) {
          //Set up control block
          one_block_ptr = pc_init_control_block( (long)pid, pid_ind, all_blocks_ptr, &bit_map, sim_clock);

          //Schedule process
          schedule_process( (long)pid, proc_queue[0], one_block_ptr);
          log_generation(log_fp, (long)pid, sim_clock[0], sim_clock[1]);

          //
          fprintf(stderr, "Generating process %ld\n", (long)pid);

          //Debugging counter.
          total_proc_gen++;
        }
        if (pid == 0) {
          execl("./proc", "proc", pid_ind_str, NULL);
        }

        //sleep(1);
      }
    }

    //Check for if an event is blocking the previously dispatched process.
    if (is_blocked) {
      //Add idle time if nothing was done up until here
      if (!spawned_proc) {
        sch_add_idle_time(last_idle_time, sim_clock, total_idle_time);
      }

      if (clock_is_time_up(sim_clock, next_disp_time)) {
        //Dispatch the proc for the entire time quantum.
        is_blocked = 0;
        pc_update_wait_time_single(&all_blocks_ptr[dispatched_pid_ind], sim_clock);

        log_redispatch(log_fp, sim_clock, dispatched_pid);

        all_blocks_ptr[dispatched_pid_ind].status = 0;
        goto DISPATCH;
      }

      //Increment the wait time of the currently dispatched process
      pc_update_wait_time_single(&all_blocks_ptr[dispatched_pid_ind], sim_clock);

      sch_set_last_idle_time(last_idle_time, sim_clock);
      clock_increment_time(sim_clock, get_rand_nano(rand_fp));
      continue;
    }

    //Dispatch Pathing
    dispatched_pid = d_get_head_pid(proc_queue); //Get the highest priority PID, pop it from queue

    //No processes to be dispatched?
    if (dispatched_pid == -1) {
      sch_add_idle_time(last_idle_time, sim_clock, total_idle_time);

      fprintf(stderr, "No processes waiting in queue\n");

      sch_set_last_idle_time(last_idle_time, sim_clock);
      clock_increment_time(sim_clock, get_rand_nano(rand_fp));
      continue;
    }

    //Get the pid index NOTE: Eventually refactor case 0 functions to use this also.
    dispatched_pid_ind = d_get_pid_ind(dispatched_pid, all_blocks_ptr);
    if (dispatched_pid_ind == -1) {
      fprintf(stderr, "Error retrieving pid_ind for process %ld\n", dispatched_pid);
      cleanup();
    }
    sb.sem_num = dispatched_pid_ind;
    fprintf(stderr, "SEM NUM CHOSEN TO DISPATCH %d, PID: %ld\n", sb.sem_num, dispatched_pid);

    log_select_proc(log_fp, dispatched_pid, sim_clock[0], sim_clock[1]);

    //Decide what will happen to the process:
    DISPATCH:
    switch (1) {
      case 0: //Process "Terminates"
        term_pid_ind = d_terminate_pid(dispatched_pid, all_blocks_ptr); //Mark the status as terminated
        fprintf(stderr, "TERM_PID_IND: %d\n", term_pid_ind);
        if (term_pid_ind == -1) {
          fprintf(stderr, "Error in d_terminate_pid with pid %d\n", pid);
          cleanup();
        }

        //Signal to proess.
        sb.sem_num = term_pid_ind;
        sb.sem_op = 1;
        semop(sem_proc_id, &sb, 1);
        exitIfError(argv[0]);

        fprintf(stderr, "Waiting for child to return...\n");
        waitpid(dispatched_pid, NULL, 0); //Wait for the child to return (with the turnaround_time updated).

        //Log it.
        log_term_proc(log_fp, dispatched_pid, sim_clock[0], sim_clock[1]);

        //Extract times to term_time_store
        pc_harvest_metrics(term_time_store, term_pid_ind, all_blocks_ptr);
        /*
        fprintf(stderr, "TEMP VALS: \n");
        for (int i = 0; i < 7; i++) {
          fprintf(stderr, "%d", term_time_store[i]);
          fprintf(stderr, "\n");
        }
        */
        //Update totals
        sch_update_proc_metrics(metric_totals, term_time_store);
        total_proc_term++;
        if (total_proc_term == UINT_MAX) {
          break;
        }

        //Clear pc_block and bitmap bit
        err_check = d_clear_pid_info(term_pid_ind, all_blocks_ptr, &bit_map);
        fprintf(stderr, "err_check value: %ld\n", err_check);
        fprintf(stderr, "dispatched_pid value: %ld\n", dispatched_pid);
        if (err_check != dispatched_pid) {
          fprintf(stderr, "Error clearing block...exiting\n");
          cleanup();
        }

        /*
        fprintf(stderr, "TOTALS: \n");
        for (int i = 0; i < 7; i++) {
          fprintf(stderr, "%d ", metric_totals[i]);
          fprintf(stderr, "\n");
        }
        */

        //sleep(1);

        break;
      case 1: //Process runs for its time quantum.
        err_check = d_proc_run_full_q(dispatched_pid, all_blocks_ptr, quantum);
        if (err_check == -1) {
            fprintf(stderr, "Unexpected return value (%ld) from d_proc_run_full_q()\n", err_check);
            cleanup();
        }
        log_run_proc(log_fp, dispatched_pid, err_check, quantum[err_check], sim_clock);

        //fprintf(log_fp, "sem num: %d\n", sb.sem_num);

        //Signal to process
        sb.sem_op = 1;
        semop(sem_proc_id, &sb, 1);
        exitIfError(argv[0]);

        //Busy wait for status to change
        fprintf(stderr, "Scheduler busywaiting...\n");
        while (all_blocks_ptr[dispatched_pid_ind].status != 0);

        //Resume scheduler pathing
        sb.sem_op = -1;
        fprintf(stderr, "Scheduler waiting on semaphore inc...\n");
        semop(sem_proc_id, &sb, 1);
        exitIfError(argv[0]);

        //Reschedule the process
        log_proc_return(log_fp, all_blocks_ptr[dispatched_pid_ind].pid, all_blocks_ptr[dispatched_pid_ind].assigned_burst_time, all_blocks_ptr[dispatched_pid_ind].actual_burst_time, sim_clock);

        reschedule_process(&all_blocks_ptr[dispatched_pid_ind], proc_queue);

        log_reschedule(log_fp, all_blocks_ptr[dispatched_pid_ind].pid, all_blocks_ptr[dispatched_pid_ind].queue, sim_clock);
        //sleep(1);

        break;
      case 2: //Wait until an event occurs before dispatching.
        //Set the unblock time.
        sch_set_next_disp(next_disp_time, sim_clock, rand_fp);

        //Log it
        log_next_disp(log_fp, next_disp_time, sim_clock, dispatched_pid);

        //Set control flow flag
        is_blocked = 1;

        //Modify the status of blocked process to blocked (so that it's exempt from queue movement)
        all_blocks_ptr[dispatched_pid_ind].status = 2;

        break;
      case 3: //Process gets pre-empted, i.e. only use a percent of its assigned quantum




        break;
      default:
        fprintf(stderr, "get_rand_action switch hit default...exiting\n");
        cleanup();
    }

    sch_set_last_idle_time(last_idle_time, sim_clock);
    clock_increment_time(sim_clock, get_rand_nano(rand_fp));
  }

  //None of this will run unless you break out during debugging
  pc_update_wait_times(all_blocks_ptr, sim_clock);
  q_update_avg_times(avg_wait_times, all_blocks_ptr);
  q_update_queues(log_fp, sim_clock, proc_queue, all_blocks_ptr, avg_wait_times, alpha, beta);

  //Set variables for totals
  total_proc_wait[0] = metric_totals[0], total_proc_wait[1] = metric_totals[1];
  total_proc_turn[0] = metric_totals[2], total_proc_turn[1] = metric_totals[3];
  total_proc_cpu_use[0] = metric_totals[4], total_proc_cpu_use[1] = metric_totals[5];

  //Log Scheduler Metrics
  log_scheduler_metrics(log_fp,
                        sch_calc_avg_wait(avg_proc_wait, total_proc_wait, total_proc_term),
                        sch_calc_turnaround(avg_turnaround, total_proc_turn, total_proc_term),
                        sch_calc_avg_cpu(avg_proc_cpu, total_proc_cpu_use, total_proc_term),
                        total_idle_time);


  fprintf(log_fp, "End Time: %d: %d\n", sim_clock[0], sim_clock[1]);
  fprintf(log_fp, "Bitmap Value: %d\n", bit_map);

  for (int i = 0; i < 3; i++) {
    fprintf(log_fp, "Average Wait Time of Queue %d: %d:%d\n", i, avg_wait_times[i][0], avg_wait_times[i][1]);
    fprintf(log_fp, "Queue Size %d: %d\n", i, deque_get_size(proc_queue[i]));
    while (deque_is_empty(proc_queue[i]) != true) {
		 fprintf(log_fp, "%ld\n", deque_pop_front(proc_queue[i]));
	  }
  }

  for (int i = 0; i < 19; i++) {
    fprintf(log_fp, "%d: CPU Usage:  %d:%d\n", i, all_blocks_ptr[i].total_cpu_usage[0], all_blocks_ptr[i].total_cpu_usage[1]);
    fprintf(log_fp, "%d: Exist Time: %d:%d\n", i, all_blocks_ptr[i].turnaround_time[0], all_blocks_ptr[i].turnaround_time[1]);
    fprintf(log_fp, "%d: Burst: %d:%d\n", i, all_blocks_ptr[i].last_burst_use_time[0], all_blocks_ptr[i].last_burst_use_time[1]);
    fprintf(log_fp, "%d: Wait: %d:%9d\n", i, all_blocks_ptr[i].total_wait_time[0], all_blocks_ptr[i].total_wait_time[1]);
    fprintf(log_fp, "%d: Last Finished: %d:%d\n", i, all_blocks_ptr[i].last_finished_time[0], all_blocks_ptr[i].last_finished_time[1]);
    fprintf(log_fp, "%d: Times Dispatched:%d\n", i, all_blocks_ptr[i].times_dispatched);

    fprintf(log_fp, "%d: Gen: %d:%9d\n", i, all_blocks_ptr[i].gen_time[0], all_blocks_ptr[i].gen_time[1]);
    fprintf(log_fp, "%d: Status: %d\n", i,all_blocks_ptr[i].status);
    fprintf(log_fp, "%d: Pid: %ld\n", i,all_blocks_ptr[i].pid);
    fprintf(log_fp, "%d: BM Index: %d\n", i,all_blocks_ptr[i].bm_ind);
    fprintf(log_fp, "%d: Q: %d\n", i,all_blocks_ptr[i].queue);
    fprintf(log_fp, "\n");
  }

  return 0;
}

void init_queues() {
  //ready_queue = deque_alloc();
  for (int i = 0; i < 3; i++) {
    proc_queue[i] = deque_alloc();
  }
  if (!proc_queue[0] || !proc_queue[1] || !proc_queue[2]) {
    errno = ENOMEM;
  }
}

void free_queues() {
  //deque_free(ready_queue);
  for (int i = 0; i < 3; i++) {
    deque_free(proc_queue[i]);
  }
}

//"Wrapper" function for errCheck()
void exitIfError(char prog_name[]) {
  if (errCheck(prog_name, errno)) {
    cleanup();
  }
}

void cleanup() {
  fprintf(stderr, "Master exiting due to error. Sending SIGTERM to children...\n");
  free_queues();
  closeFiles();
  SHMDTRM();
  kill(0, SIGTERM);

  if (errno) {
    strerror(errno);
    exit(1);
  }
  exit(0);
}

void closeFiles() {
  if (log_fp) {
    fclose(log_fp);
  }
  if (rand_fp) {
    fclose(rand_fp);
  }
}

void SHMDTRM() {
  if (shm_blocks_addr)
    shmdt(shm_blocks_addr);
  if (shm_blocks_id)
    shmctl(shm_blocks_id, IPC_RMID, NULL);
  if (shm_clock_addr)
    shmdt(shm_clock_addr);
  if (shm_clock_id)
    shmctl(shm_clock_id, IPC_RMID, NULL);
  if (shm_rqueue_addr)
    shmdt(shm_rqueue_addr);
  if (shm_rqueue_id)
    shmctl(shm_rqueue_id, IPC_RMID, NULL);

  semctl(sem_proc_id, 0, IPC_RMID);
}

void killSIGINT() {
  fprintf(stderr, "PID: %ld Master process exiting due to interrupt. Sending SIGINT to children...\n", (long)getpid());
  fprintf(stderr, "Logging Metrics...\n");

  total_proc_wait[0] = metric_totals[0], total_proc_wait[1] = metric_totals[1];
  total_proc_turn[0] = metric_totals[2], total_proc_turn[1] = metric_totals[3];
  total_proc_cpu_use[0] = metric_totals[4], total_proc_cpu_use[1] = metric_totals[5];

  log_scheduler_metrics(log_fp,
                        sch_calc_avg_wait(avg_proc_wait, total_proc_wait, total_proc_term),
                        sch_calc_turnaround(avg_turnaround, total_proc_turn, total_proc_term),
                        sch_calc_avg_cpu(avg_proc_cpu, total_proc_cpu_use, total_proc_term),
                        total_idle_time);

  free_queues();
  SHMDTRM();
  kill(0, SIGINT);
  exit(1);
}

void killSIGTERM() {
  fprintf(stderr, "PID: %ld Master process exiting due to timeout. Sending SIGTERM to children...\n", (long)getpid());
  fprintf(stderr, "Logging Metrics...\n");

  total_proc_wait[0] = metric_totals[0], total_proc_wait[1] = metric_totals[1];
  total_proc_turn[0] = metric_totals[2], total_proc_turn[1] = metric_totals[3];
  total_proc_cpu_use[0] = metric_totals[4], total_proc_cpu_use[1] = metric_totals[5];

  log_scheduler_metrics(log_fp,
                        sch_calc_avg_wait(avg_proc_wait, total_proc_wait, total_proc_term),
                        sch_calc_turnaround(avg_turnaround, total_proc_turn, total_proc_term),
                        sch_calc_avg_cpu(avg_proc_cpu, total_proc_cpu_use, total_proc_term),
                        total_idle_time);

  free_queues();
  SHMDTRM();
  kill(0, SIGTERM);
  exit(1);
}
