/* This program will create child processes at an average rate of 1 process per seconds
   Generated processes will be sent to the ready queue, at which point the scheduler will
   schedule them to queue 0 and dispatch accordingly.
*/

#include <sys/shm.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "../lib/parse_opt.h"

#define billion 1000000000

void exitIfError(char prog_name[]);
void cleanup();
void SHMDT();
void closeFiles();
void exitSIGINT();
void exitSIGTERM();

//Shared Memory
int shm_clock_id;
int shm_rqueue_id;
void* shm_clock_addr;
void* shm_rqueue_addr;

FILE* rand_fp; //Pointer to /dev/random

int main(int argc, char* argv[]) {
  //Set up signal handling.
  signal(SIGINT, exitSIGINT);
  signal(SIGTERM, exitSIGTERM);

  //Attach to shared memory
  key_t shm_clock_key = ftok("./dummyfile", 1);
  key_t shm_rqueue_key = ftok("./dummyfile", 3);
  exitIfError(argv[0]);

  shm_clock_id = shmget(shm_clock_key, sizeof(unsigned int) * 2, 0);
  shm_rqueue_id = shmget(shm_rqueue_key, sizeof(long) * 19, 0);
  shm_clock_addr = shmat(shm_clock_id, (void*)0, 0);
  shm_rqueue_addr = shmat(shm_rqueue_id, (void*)0, 0);
  exitIfError(argv[0]);

  //Begin Process Generation Loop



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
  shmdt(shm_rqueue_addr);
}

void closeFiles() {
  if (rand_fp) {
    fclose(rand_fp);
  }
}

void exitSIGINT() {
  fprintf(stderr, "Process Generator (PID: %ld) received interrupt signal. Exiting...\n", (long)getpid());
  closeFiles();
  SHMDT();
  exit(1);
}

void exitSIGTERM() {
  fprintf(stderr, "Process Generator (PID: %ld) received notification of master timeout. Exiting...\n", (long)getpid());
  closeFiles();
  SHMDT();
  exit(1);
}
