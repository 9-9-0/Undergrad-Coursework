#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "../lib/palin_fun.h"

void exitSIGINT();
void exitSIGTERM();
void openFiles();
void writeToFile(int index, char input[]);
void closeFiles();
void SHMDT();
void errCheck(char progName[]);

//Global Variables
int shmidStr;
int shmidFlag;
int shmidTurn;
void* shmstraddr;
void* shmshmFlagContentsaddr;
void* shmturnaddr;
FILE* Pfp;
FILE* NONPfp;

//Flag values huehue
int idle    = 0;
int want_in = 1;
int in_cs   = 2;

int main(int argc, char* argv[]) {
  //Signal Handlers
  signal(SIGINT, exitSIGINT);
  signal(SIGTERM, exitSIGTERM);


  //Parse arguments. Setup getopt if time permits.
  if (argc != 6) {
    fprintf(stderr, "Invalid Number of arguments passed to palin. Exiting...\n");
    exit(1);
  }
  int startInd = atoi(argv[1]);
  int assignedCount = atoi(argv[2]);
  int shmStrSize = atoi(argv[3]);
  int maxStrLen = atoi(argv[4]);
  int childInd = atoi(argv[5]);

  //File setup
  openFiles();
  //fprintf(stderr, "Start Ind: %d, AssgnCount: %d, shmStrSize: %d, maxStrLen: %d, ChildIndex: %d\n", startInd, assignedCount, shmStrSize, maxStrLen, childInd);

  //Shared Memory Attachment
  key_t shmstrkey = ftok("./dummyfile", 1);
  key_t shmshmFlagContentskey = ftok("./dummyfile", 2);
  key_t shmturnkey = ftok("./dummyfile", 3);

  shmidStr = shmget(shmstrkey, shmStrSize, 0);
  shmidFlag = shmget(shmshmFlagContentskey, 19 * sizeof(int), 0); //Hard-coding these for now. This size shouldn't change unless maxChildCount changes.
  shmidTurn = shmget(shmturnkey, sizeof(int), 0); //Hard-coding these for now.
  errCheck(argv[0]);

  //fprintf(stderr, "shmidStr: %d\n", shmidStr);
  shmstraddr = shmat(shmidStr, (void*)0, 0);
  shmshmFlagContentsaddr = shmat(shmidFlag, (void*)0, 0);
  shmturnaddr = shmat(shmidTurn, (void*)0, 0);
  errCheck(argv[0]);

  char (*shmStrContents)[maxStrLen] = shmstraddr;
  int *shmFlagContents = shmshmFlagContentsaddr;
  int* shmTurnContents = shmturnaddr;

  /*
  fprintf(stderr, "Turn value: %d\n", *shmTurnContents);
  for (int localTurn = 0; localTurn < 19; localTurn++) {
    fprintf(stderr, "Flag %d value: %d\n", localTurn, shmFlagContents[localTurn]);
  }
  */

  //Misc Setup
  srand(time(NULL));
  time_t tyme = time(NULL);

  //Process palindrome loop.
  int localTurn;
  for (int i = startInd; i < startInd + assignedCount; i++) {
    //fprintf(stderr, "HIHI %s\n", shmStrContents[i]);

      //Peterson's multi process algorithm
      do {
        shmFlagContents[childInd] = want_in; // Raise my shmFlagContents
        localTurn = *shmTurnContents; // Set local variable
        // wait until its my turn
        while ( localTurn != childInd )
        localTurn = ( shmFlagContents[localTurn] != idle ) ? *shmTurnContents : ( localTurn + 1 ) % 19;
        // Declare intention to enter critical section
        shmFlagContents[childInd] = in_cs;
        // Check that no one else is in critical section
        for ( localTurn = 0; localTurn < 19; localTurn++ )
        if ( ( localTurn != childInd ) && ( shmFlagContents[localTurn] == in_cs ) )
        break;
      } while ( ( localTurn < 19 ) || ( *shmTurnContents != childInd && shmFlagContents[*shmTurnContents] != idle ) );
      // Assign turn to self and enter critical section
      *shmTurnContents = childInd;

      //Sleep for random amount of time between 0-2s
      sleep((int)(rand() % 3));

      //Notify stderr of entering
      fprintf(stderr, "%s - %ld is ENTERING the critical section...\n", ctime(&tyme), (long)getpid());
      //Execute critical section code
      writeToFile(childInd, shmStrContents[i]);

      //Sleep for random amount of time
      sleep((int)(rand() % 3));

      // Exit section
      fprintf(stderr, "%s - %ld is EXITING the critical section...\n", ctime(&tyme), (long)getpid());
      localTurn = (*shmTurnContents + 1) % 19;
      while (shmFlagContents[localTurn] == idle)
      localTurn = (localTurn + 1) % 19;
      // Assign turn to next waiting process; change own shmFlagContents to idle
      *shmTurnContents = localTurn;
      shmFlagContents[childInd] = idle;

  }

  closeFiles();
  exit(0);

}

//On a SIGINT signal sent by master (^C), detach and exit process
void exitSIGINT() {
  fprintf(stderr, "PID: %ld received interrupt signal. Exiting...\n", (long)getpid());
  closeFiles();
  SHMDT();
  exit(1);
}

//On a SIGTERM signal sent by master (timeout), detach and exit process
void exitSIGTERM() {
  fprintf(stderr, "PID: %ld recieved notification of master process time out. Exiting...\n", (long)getpid());
  closeFiles();
  SHMDT();
  exit(1);
}

void openFiles() {
  Pfp = fopen("../out/palin.out", "a");
  NONPfp = fopen("../out/nopalin.out", "a");
}

void writeToFile(int index, char input[]) {
  if (isPalin(input, 0)) {
    fprintf(Pfp, "PID: %ld Child: %d String: %s\n", (long)getpid(), index + 1, input); //[0,18] offset into [1,19]
  }
  else {
    fprintf(NONPfp, "PID: %ld Child: %d String: %s\n", (long)getpid(), index + 1, input); //[0,18] offset into [1,19]
  }
}

void closeFiles() {
  fclose(Pfp);
  fclose(NONPfp);
}

//Detaches child process from shared memory segments
void SHMDT() {
  shmdt(shmstraddr);
  shmdt(shmshmFlagContentsaddr);
  shmdt(shmturnaddr);
}
//Exits if ERRNO is set
void errCheck(char progName[]) {
  if (errno) {
    fprintf(stderr, "%s: Error: %s\n", progName, strerror(errno));
    exit(1);
  }
}
