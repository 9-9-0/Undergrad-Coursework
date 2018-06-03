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

void calcEndTime(int startTime[], int endTime[], int duration);
void exitIfError(char prog_name[]);
void cleanup();
void SHMDT();
void exitSIGINT();
void exitSIGTERM();
void closeFiles();

//Shared Memory
int shmtime_id;
int shmmsg_id;
void* shmtime_addr;
void* shmmsg_addr;
int sem_id;

FILE* rand_fp; //Pointer to /dev/random

int main(int argc, char* argv[]) {
  //Setup signal handling.
  signal(SIGINT, exitSIGINT);
  signal(SIGTERM, exitSIGTERM);

  //Setup /dev/urandom file pointer
  rand_fp = fopen("/dev/urandom", "r");
  exitIfError(argv[0]);

  //Generate keys
  key_t shmtime_key = ftok("./dummyfile", 1);
  key_t shmmsg_key = ftok("./dummyfile", 2);
  key_t sem_key = ftok("./dummyfile", 3);
  exitIfError(argv[0]);

  //Attach to shared memory
  shmtime_id = shmget(shmtime_key, sizeof(int) * 2, 0);
  shmmsg_id = shmget(shmmsg_key, sizeof(int) * 3, 0);
  shmtime_addr = shmat(shmtime_id, (void*)0, 0);
  shmmsg_addr = shmat(shmmsg_id, (void*)0, 0);
  exitIfError(argv[0]);

  //Semaphore Setup
  sem_id = semget(sem_key, 1, 0);
  struct sembuf sb;
  exitIfError(argv[0]);

  //Get current "system time"
  int current_time_snapshot[2];
  int* current_time = shmtime_addr;
  int end_time[2];
  memcpy(current_time_snapshot, shmtime_addr, 8);

  //Determine termination time
  unsigned int seed;
  fread(&seed, sizeof(unsigned int), 1, rand_fp); //Maybe one day I'll use a "real" rng.
  calcEndTime(current_time_snapshot, end_time, (rand_r(&seed) % 1000000) + 1);

  //fprintf(stderr, "Child PID: %ld, start time: %d:%d, end time: %d:%d\n", (long)getpid(), current_time_snapshot[0], current_time_snapshot[1], end_time[0], end_time[1]);

  //Check if "termination" time is up. See README
  long end_time_nanoseconds = (long)(end_time[0] * 1000000000 + end_time[1]);
  long current_time_nanoseconds;

  int time_up = 0;
  while (!time_up) {
    current_time_nanoseconds = (long)(current_time[0] * 1000000000 + current_time[1]);
    if (end_time_nanoseconds <= current_time_nanoseconds) {
      time_up = 1;
    }
  };

  int has_written = 0;
  int* msg = shmmsg_addr;

  while (!has_written) {
    //Attempt to enter critical section
    sb.sem_num = 0, sb.sem_op = -1, sb.sem_flg = 0;
    semop(sem_id, &sb, 1);
    exitIfError(argv[0]);

    //Critical Section
    //fprintf(stderr, "%ld:I made it!\n", (long)getpid());

    if (msg[0] == -1 && msg[1] == -1 && msg[2] == 0) {
      msg[0] = end_time[0], msg[1] = end_time[1], msg[2] = getpid();
      has_written = 1;
    }

    //fprintf(stderr, "%ld: I'm leaving the critical section!\n", (long)getpid());

    //Exit Critical Section
    sb.sem_op = 1; //, sb.sem_flg = SEM_UNDO;
    semop(sem_id, &sb, 1);
    exitIfError(argv[0]);
  }

  closeFiles();
  SHMDT();
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

//needs to be tested.
void calcEndTime(int startTime[], int endTime[], int duration) {
  int seconds_end = startTime[0];
  int nanoseconds_end = startTime[1] + duration;

  //1 billion nanoseconds = 1 second
  if (nanoseconds_end > 1000000000) {
    nanoseconds_end = nanoseconds_end % 1000000000;
    seconds_end += 1;
  }

  endTime[0] = seconds_end;
  endTime[1] = nanoseconds_end;
}

void SHMDT() {
  shmdt(shmtime_addr);
  shmdt(shmmsg_addr);
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

void closeFiles() {
  //fclose(log_fp);
  if (rand_fp) {
    fclose(rand_fp);
  }
}
