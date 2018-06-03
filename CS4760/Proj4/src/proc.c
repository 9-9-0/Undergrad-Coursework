#include <stdio.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/sem.h>
#include <errno.h>
#include <semaphore.h>
#include "../lib/parse_opt.h"
#include "../lib/proc_util.h"

void exitIfError(char prog_name[]);
void cleanup();
void SHMDT();
void closeFiles();
void exitSIGINT();
void exitSIGTERM();

//Shared Memory
int shm_clock_id;
int shm_blocks_id;
void* shm_clock_addr;
void* shm_blocks_addr;
int sem_proc_id;

FILE* rand_fp; //Pointer to /dev/random

//Process Control Block Variables
pc_block* muh_block;
int block_ind;
int sem_ind;

unsigned int* current_time;
unsigned int current_time_snap[2];

int main(int argc, char* argv[]) {
  //Retrieve arguments
  if (!isNumber(argv[1])) {
    fprintf(stderr, "Proc: Invalid index argument.\n");
    exit(1);
  }

  //Setup
  block_ind = (int)strtol(argv[1], NULL, 0); //Set pc block (shared mem) index
  sem_ind = block_ind;
  if (block_ind < 0 || block_ind > 18) {
    fprintf(stderr, "Proc: Control block index out of bounds.\n");
    exit(1);
  }

  //Setup signal handling.
  signal(SIGINT, exitSIGINT);
  signal(SIGTERM, exitSIGTERM);

  //Setup /dev/urandom file pointer
  rand_fp = fopen("/dev/urandom", "r");
  exitIfError(argv[0]);

  //Generate keys
  key_t shm_clock_key = ftok("./dummyfile", 1);
  key_t shm_blocks_key = ftok("./dummyfile", 2);
  key_t sem_proc_key = ftok("./dummyfile", 4);
  exitIfError(argv[0]);

  //Attach to shared memory
  shm_clock_id = shmget(shm_clock_key, sizeof(unsigned int) * 2, 0);
  shm_blocks_id = shmget(shm_blocks_key, sizeof(pc_block) * 19, 0);
  shm_clock_addr = shmat(shm_clock_id, (void*)0, 0);
  shm_blocks_addr = shmat(shm_blocks_id, (void*)0, 0);
  exitIfError(argv[0]);

  //Attach to clock
  current_time = shm_clock_addr;

  //Semaphore Setup
  sem_proc_id = semget(sem_proc_key, max_child_procs, 0);
  struct sembuf sb;
  sb.sem_num = sem_ind, sb.sem_op = -1, sb.sem_flg = 0; //sem_op operation on semaphore @ sem_ind
  fprintf(stderr, "CHILD %ld RECEIVED SEM NUM %d.\n", (long)getpid(), sb.sem_num);
  exitIfError(argv[0]);

  //Attach to its control block
  muh_block = &((pc_block*)shm_blocks_addr)[block_ind];
  if (muh_block == NULL) {
    fprintf(stderr, "Error attaching to block index.\n");
    exit(1);
  }

  fprintf(stderr, "CHILD %ld ASSIGNED QUANTUM %d:%d, STATUS:%d\n", (long)getpid(), muh_block->assigned_burst_time[0], muh_block->assigned_burst_time[1], muh_block->status);
  int rand_partial;

  while (1) {
    sb.sem_op = -1;
    semop(sem_proc_id, &sb, 1); //Wait for scheduler to have dispatched it.
    fprintf(stderr, "CHILD %ld running.\n", (long)getpid());

    //Terminated?
    if (muh_block->status == 3) {
      break;
    }
    else {
      //Determine if the process will run for the entire slice
      if (rand_run_full_slice(rand_fp)) {
        //Set actual burst time
        set_actual_burst(muh_block, muh_block->assigned_burst_time);

        fprintf(stderr, "CHILD ACTUAL BURST: %d:%d\n", muh_block->actual_burst_time[0], muh_block->actual_burst_time[1]);
        //"Run" the slice and fast forward clock
        run_slice(muh_block, current_time);
      }
      else {
        //Run for a partial slice.
        rand_partial = rand_percent_slice(rand_fp);
        fprintf(stderr, "RAND PARTIAL: %d\n", rand_partial);

        //Set actual burst time to that partial slice.
        set_actual_burst_part(muh_block, muh_block->assigned_burst_time, rand_percent_slice(rand_fp));
        fprintf(stderr, "CHILD ACTUAL BURST (PARTIAL): %d:%d\n", muh_block->actual_burst_time[0], muh_block->actual_burst_time[1]);
        //"Run" the slice and fast forward clock
        run_slice(muh_block, current_time);
      }
    }

    set_finished_time(muh_block, current_time);
    //Mark process as not running
    fprintf(stderr, "Child marking status to 0.\n");
    muh_block->status = 0;

    //Signal completion to scheduler
    sb.sem_op = 1;
    semop(sem_proc_id, &sb, 1);

    fprintf(stderr, "Child %ld finished running its quantum at %d:%d...\n", (long)getpid(), current_time[0], current_time[1]);

    sb.sem_op = 0;
    semop(sem_proc_id, &sb, 1);
    //Possible race condition with scheduler right here...not if requests to semaphores are queued.
  }

  //Calculate turnaround time
  calc_turnaround(muh_block, current_time);

  fprintf(stderr, "Child Metrics Before Return: \n");
  fprintf(stderr, "Times Dispatched: %d  \n", muh_block->times_dispatched);
  fprintf(stderr, "Turnaround: %d:%d  \n", muh_block->turnaround_time[0], muh_block->turnaround_time[1]);
  fprintf(stderr, "Total-Wait: %d:%d  \n", muh_block->total_wait_time[0], muh_block->total_wait_time[1]);
  fprintf(stderr, "Total CPU Usage: %d:%d  \n", muh_block->total_cpu_usage[0], muh_block->total_cpu_usage[1]);

  fprintf(stderr, "CHILD RETURNING...\n");
  //Exit
  exit(0);
}

void exitIfError(char prog_name[]) {
  if (errCheck(prog_name, errno)) {
    cleanup();
  }
}

void cleanup() {
  SHMDT();
  closeFiles();
  exit(1);
}

void SHMDT() {
  shmdt(shm_clock_addr);
  shmdt(shm_blocks_addr);
}

void closeFiles() {
  if (rand_fp) {
    fclose(rand_fp);
  }
}

void exitSIGINT() {
  fprintf(stderr, "PID: %ld received interrupt signal. Exiting...\n", (long)getpid());
  closeFiles();
  SHMDT();
  exit(1);
}

void exitSIGTERM() {
  fprintf(stderr, "PID: %ld recieved notification of master process time out. Exiting...\n", (long)getpid());
  closeFiles();
  SHMDT();
  exit(1);
}
