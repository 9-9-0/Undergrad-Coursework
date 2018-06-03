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
#include "../lib/parse_opt.h"

void exitIfError(char prog_name[]);
void cleanup();
void closeFiles();
void SHMDTRM();
void killSIGINT();
void killSIGTERM();
void notifyEnd();

//Shared Memory
int shmtime_id;
int shmmsg_id;
void* shmtime_addr;
void* shmmsg_addr;

//Semaphore
int sem_id;
union semun {
    int val;               /* used for SETVAL only */
    //struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    //ushort *array;         /* used for GETALL and SETALL */
};

//Input Variables
int max_run_time = -1;
int max_slave_count = -1;
int nano_increment = 1;
char* log_filename;
FILE* log_fp = NULL;

int main(int argc, char* argv[]) {
  //Signal Handlers
  signal(SIGINT, killSIGINT);
  signal(SIGALRM, killSIGTERM);

  //getopt() setup
  static char usage[] = "%s: Usage: %s [-n # of max slave processes] [-l log file location] [-t max run time]\n\
                        [-m optional nanoseconds to increment by] [-h help]\n";
  static char help[] = "-n    Number of max slave process that may run at one time.\n\
-l    Filepath for the log to be written to. Must be writable.\n\
-t    Maximum real time (in seconds) for the main process to run before timing out.\n\
-m    Optional number of simulated nanoseconds to increment by. Must be between 0 and 1,000,000,000.\n\
      Default is 1.\n";
  int c;
  extern char *optarg;
  extern int optind;

  while ((c = getopt(argc, argv, "hn:l:t:m:")) != -1) {
    exitIfError(argv[0]);
    switch (c) {
      case 'h':
        ;
        getHelp(argv[0], usage, help);
        break;
      case 'n':
        ;
        if (isNumber(optarg))
          max_slave_count = (int)strtol(optarg, NULL, 0);
          if (max_slave_count < 1)
            errno = EINVAL;
        break;
      case 'l':
        ;
        log_fp = fopen(optarg, "w");
        log_filename = optarg;
        break;
      case 't':
        ;
        if (isNumber(optarg))
          max_run_time = (int)strtol(optarg, NULL, 0);
          if (max_run_time < 1)
            errno = EINVAL;
        break;
      case 'm':
        ;
        if (isNumber(optarg))
          nano_increment = (int)strtol(optarg, NULL, 0);
          //set bounds on the nanosecond increment value.
          if (nano_increment > 1000000000 || nano_increment < 0)
            errno = EINVAL;
        break;
      case '?':
        badUse(argv[0], usage);
        break;
    }
  }
  exitIfError(argv[0]);

  if (max_run_time == -1 || max_slave_count == -1 || log_fp == NULL) {
    fprintf(stderr, "Missing required arguments.\n");
    badUse(argv[0], usage);
  }
  alarm(max_run_time); //set timeout

  //fprintf(stderr, "getopt done parsing\n");

  //Shared Memory Setup
  key_t shmtime_key = ftok("./dummyfile", 1);
  key_t shmmsg_key = ftok("./dummyfile", 2);
  key_t sem_key = ftok("./dummyfile", 3);

  shmtime_id = shmget(shmtime_key, sizeof(int) * 2, IPC_CREAT | 0644);
  shmmsg_id = shmget(shmmsg_key, sizeof(int) * 3, IPC_CREAT | 0644);
  exitIfError(argv[0]);
  shmtime_addr = shmat(shmtime_id, (void*)0, 0);
  shmmsg_addr = shmat(shmmsg_id, (void*)0, 0);
  exitIfError(argv[0]);

  //Semaphore Setup
  sem_id = semget(sem_key, 1, IPC_CREAT | 0666);
  exitIfError(argv[0]);
  union semun arg;
  arg.val = 1;
  semctl(sem_id, 0, SETVAL, arg);
  exitIfError(argv[0]);

  //Clock setup.
  int* sim_clock = shmtime_addr;
  int* msg   = shmmsg_addr;
  //Initialize starting values for clock and message.
  sim_clock[0] = 0, sim_clock[1] = 0;       //0 = seconds, 1 = nanoseconds
  msg[0] = -1, msg[1] = -1, msg[2] = 0;    //0 = seconds, 1 = nanoseconds, 2 = child pid.
                                           //Note the "empty" value of msg[2] corresponds to the system's scheduler pid

  int running_count = 0;
  pid_t pid;

  fprintf(stderr, "HI");
  //Initial Process Spawn
  for (int i = 0; i < max_slave_count; i++) {
    pid = fork();
    exitIfError(argv[0]);
    if (pid == 0) {
      execl("./isabella", "isabella", NULL);
    }
    if (pid) {
      running_count++;
    }
  }

  //fprintf(stderr, "running_count: %d\n", running_count);

  fprintf(stderr, "Clock running...\n");
  //Begin looping through time...
  for (; sim_clock[0] < 2; sim_clock[1] += nano_increment) {
    //Increment seconds if enough nanoseconds
    if (sim_clock[1] > 1000000000) {
      sim_clock[0]++;
      sim_clock[1] = sim_clock[1] % 1000000000;
      //fprintf(stderr, "INCREMENTED SECONDS\n");
    }
    else {
      //fprintf(stderr, "DID NOT INCREMENT SECONDS\n");
    }

    //Spawn processes up until max processes are in use
    while (running_count < max_slave_count) {
      pid = fork();
      exitIfError(argv[0]);
      if (pid == 0) {
        execl("./isabella", "isabella", log_filename, NULL);
      }
      if (pid) {
        running_count++;
      }
    }

    //Check if a slave has written to shared message
    if (msg[0] != -1 && msg[1] != -1 && msg[2] != 0) {
      //Write to file.
      fprintf(log_fp, "Master: Slave %ld is terminating at my time %d:%d because it reached %d:%d in slave.\n", (long)msg[2], sim_clock[0], sim_clock[1], msg[0], msg[1]);
      //"Clear" shared message and decrement count
      msg[0] = -1, msg[1] = -1, msg[2] = 0;

      //Call a wait on the child to prevent zombie status
      waitpid(msg[2], NULL, 0);
      exitIfError(argv[0]);
      running_count--;
    }
  }

  notifyEnd();
}

//"Wrapper" function for errCheck()
void exitIfError(char prog_name[]) {
  if (errCheck(prog_name, errno)) {
    cleanup();
  }
}

void cleanup() {
  closeFiles();
  fprintf(stderr, "Master exiting due to error. Sending SIGTERM to children...\n");
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
}

void SHMDTRM() {
  if (shmtime_addr)
    shmdt(shmtime_addr);
  if (shmtime_id)
    shmctl(shmtime_id, IPC_RMID, NULL);
  if (shmmsg_addr)
    shmdt(shmmsg_addr);
  if (shmmsg_id)
    shmctl(shmmsg_id, IPC_RMID, NULL);
  semctl(sem_id, 0, IPC_RMID);
}

void killSIGINT() {
  fprintf(stderr, "PID: %ld Master process exiting due to interrupt. Sending SIGINT to children...\n", (long)getpid());
  kill(0, SIGINT);
  SHMDTRM();
  exit(1);
}

void killSIGTERM() {
  fprintf(stderr, "PID: %ld Master process exiting due to timeout. Sending SIGTERM to children...\n", (long)getpid());
  SHMDTRM();
  kill(0, SIGTERM);
  exit(1);
}

//When master reaches 2 seconds in simulated time, send SIGTERM to children.
void notifyEnd() {
  fprintf(stderr, "Master process reached 2 seconds of simulated time. Sending SIGKILL to children...\n");
  closeFiles();
  SHMDTRM();
  kill(0, SIGTERM);
  exit(0);
}
