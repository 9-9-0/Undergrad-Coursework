#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "../lib/checks.h"
#include "../lib/uthash.h"
#include "../lib/stack.h"

void killSIGINT();
void killSIGTERM();
void SHMDTRM();
void badUse(char progName[], const char usage[]);
void errCheck(char progName[]); //Might want to still check for -1 return values from shm/sem calls to be safe...

//Global Variables
int shmidStr;
int shmidFlag;
int shmidTurn;
void* shmstraddr;
void* shmflagaddr; // Critical Section Address for flag
void* shmturnaddr; // Critical Section Address for turn flags

//HashTable Entry struct
struct entry {
  int id;             //Unique Key. Value of child PID
  int procInd;        //Assigned index associated with the child PID
  UT_hash_handle hh;
};
struct entry *map = NULL;
//HashTable Functions
void add_pid(int pid, int childInd);
struct entry* find_pid(int pid);
void delete_pid(struct entry *pid);
void delete_all();

int main(int argc, char* argv[]) {
  //Signal Handlers
  signal(SIGINT, killSIGINT);
  signal(SIGALRM, killSIGTERM);

  //Main Stuff
  int timeLimit;
  char* fileName;
  int maxChildCount = 19; //Configurable?

  //Option Parsing, look into modularizing this and putting it elsewhere.
  static char usage[] = "%s: Usage: %s [-f file to be processed] [-t max time to run (seconds)]\n";
  int c;

  if (argc != 5) {
    badUse(argv[0], usage);
  }
  while ((c = getopt(argc, argv, "f:t:")) != -1) {
    switch (c) {
      case 'f':
        ;
        fileName = optarg;
        //Need to do re-add file validation here.
        break;
      case 't':
        ;
        if (isNumber(optarg)) {
          timeLimit = (int)strtol(optarg, NULL, 0);
          alarm(timeLimit); //Set the alarm
        }
        else {
          errno = EINVAL;
        }
        break;
      case '?':
        badUse(argv[0], usage);
        break;
    }
  }
  errCheck(argv[0]);

  //File Reading
  int maxStrLen = 256;
  char strBuf[maxStrLen];
  int lineCount = 0;

  FILE* fp;
  fp = fopen(fileName, "r+");
  while (fgets(strBuf, 256, fp)) {
    lineCount++;
  }
  rewind(fp);
  //fprintf(stderr, "# of lines: %d\n", lineCount);

  char fContents[lineCount][maxStrLen];
  for (int i = 0; i < lineCount; i++) {
    fgets(fContents[i], 256, fp);
    for (int j = strlen(fContents[i]) - 1; j>=0 && (fContents[i][j]=='\n' || fContents[i][j]=='\r'); j--) {
      fContents[i][j] = '\0';
    }
  }

  //Shared Memory Creation
  key_t shmstrkey = ftok("./dummyfile", 1);
  key_t shmflagkey = ftok("./dummyfile", 2);
  key_t shmturnkey = ftok("./dummyfile", 3);

  int size = lineCount * maxStrLen;

  shmidStr = shmget(shmstrkey, size, IPC_CREAT | 0644);
  shmidFlag = shmget(shmflagkey, maxChildCount * sizeof(int), IPC_CREAT | 0644);
  shmidTurn =  shmget(shmturnkey, sizeof(int), IPC_CREAT | 0644);
  errCheck(argv[0]);

  /*
  fprintf(stderr, "shmidStr: %d\n", shmidStr);
  fprintf(stderr, "size: %d\n", size);
  fprintf(stderr, "line count: %d\n", lineCount);
  */

  shmstraddr = shmat(shmidStr, (void*)0, 0);
  shmflagaddr = shmat(shmidFlag, (void*)0, 0);
  shmturnaddr = shmat(shmidTurn, (void*)0, 0);

  char (*shmstrings)[maxStrLen] = shmstraddr;
  int* shmflags = shmflagaddr;
  int* shmturn = shmturnaddr;
  errCheck(argv[0]);

  //Copy file contents to its shared memory segment
  for (int i = 0; i < lineCount; i++) {
    memcpy(shmstrings[i], fContents[i], maxStrLen);
  }
  //Initialize turn to 0
  *shmturn = 0;

  //Initialize all flag variables to idle.
  for (int j = 0; j < maxChildCount; j++) {
    shmflags[j] = 0;
  }

  /*
  fprintf(stderr, "Shared mem string: %s\n", shmstrings[0]);
  fprintf(stderr, "Shared mem string: %s\n", shmstrings[1]);
  fprintf(stderr, "Shared mem string: %s\n", shmstrings[2]);
  fprintf(stderr, "Shared mem string: %s\n", shmstrings[3]);
  fprintf(stderr, "Shared mem string: %s\n", shmstrings[lineCount - 1]);
  */

  //Child Spawn Variables
  char startIndStr[100]; //Un-hardcode these buffer sizes at some point.
  char assgnCntStr[100];
  char sizeStr[100];
  char maxStrLenStr[100];
  char childIndStr[100];

  int maxWrites = 5;
  int startInd = 0;
  int assgnCnt = maxWrites;
  //Constant value string arguments
  sprintf(sizeStr, "%d", size);
  sprintf(maxStrLenStr, "%d", maxStrLen);

  pid_t pid;
  int spawnedCount = 0;
  int linesLeft = lineCount;

  //Load up the stack containing free indexes.
  Stack freeIndexes;
  Stack_Init(&freeIndexes);
  for (int k = maxChildCount - 1; k >= 0; k--) {
    Stack_Push(&freeIndexes, k);
  }


  int returnedPID;                //Holds the child PID that returns from wait() call
  struct entry* returnedE = NULL; //Temporary pointer that points to a returned key-value entry in hashtable
  int childInd;                   //Holds the next available child index to be passed to palin

  //MAIN PROCESS SPAWN LOOP
  while (linesLeft > 0) {
    //fprintf(stderr, "Lines Left Before: %d\n", linesLeft);
    //fprintf(stderr, "spawned count before if check: %d\n", spawnedCount);
    if (spawnedCount >= maxChildCount) {
      //fprintf(stderr, "max processes running\n");
      while ( (returnedPID = wait(NULL)) == 0);
      spawnedCount--;

      //Delete PID->Index entry
      returnedE = find_pid(returnedPID);
      if (returnedE == NULL) {
        fprintf(stderr, "dun goofed\n");
        exit(1);
      }
      //Return the Index to the Free Index stack
      Stack_Push(&freeIndexes, returnedE->procInd);
      delete_pid(returnedE);
      continue;
    }
    //fprintf(stderr, "spawned count after if check: %d\n", spawnedCount);

    if (linesLeft < maxWrites) {
      assgnCnt = linesLeft % maxWrites; //e.g. 84 lines, 4 left.
    }

    //Construct String arguments
    sprintf(startIndStr, "%d", startInd);
    sprintf(assgnCntStr, "%d", assgnCnt);

    //Get a free index
    childInd = Stack_Top(&freeIndexes);
    Stack_Pop(&freeIndexes);
    //Construct childIndex string argument
    sprintf(childIndStr, "%d", childInd);

    pid = fork();
    //fprintf(stderr, "Spawned child, starting index: %d\n", startInd);
    errCheck(argv[0]);
    spawnedCount++;
    linesLeft -= assgnCnt;

    if (pid) {
      //Associate the index with child PID (key)
      add_pid(pid, childInd);
    }
    if (pid == 0) {
      execl("./palin", "palin", startIndStr, assgnCntStr, sizeStr, maxStrLenStr, childIndStr, NULL);
    }

    errCheck(argv[0]);
    startInd += assgnCnt; //Increment starting index for the next process.


    //fprintf(stderr, "Lines Left After: %d\n", linesLeft);
  }

  //Edge Case handling: When less than maxChildCount * 5 strings are processed
  while (spawnedCount != 0) {
    while ( (returnedPID = wait(NULL)) == 0);
    spawnedCount--;

    //Not necessary to do these operations since no additional children spawning, but what the hell
    //Delete from Hash Table
    returnedE = find_pid(returnedPID);
    if (returnedE == NULL) {
      fprintf(stderr, "dun goofed\n");
      exit(1);
    }
    //Return the Index to the Free Index stack
    Stack_Push(&freeIndexes, returnedE->procInd);
    delete_pid(returnedE);
  }

  errCheck(argv[0]);

  fprintf(stderr, "DONE :)\n");

  //Cleanup
  SHMDTRM();
  delete_all();
  exit(0);
}

void killSIGINT() {
  fprintf(stderr, "PID: %ld Master process exiting due to interrupt. Sending SIGINT to children...\n", (long)getpid());
  kill(0, SIGINT);
  SHMDTRM();
  delete_all();
  exit(1);
}
void killSIGTERM() {
  fprintf(stderr, "PID: %ld Master process exiting due to timeout. Sending SIGKILL to children...\n", (long)getpid());
  kill(0, SIGTERM);
  SHMDTRM();
  delete_all();
  exit(1);
}

//Detach from all shared memory segments and remove them.
void SHMDTRM() {
  shmdt(shmstraddr);
  shmdt(shmflagaddr);
  shmdt(shmturnaddr);
  shmctl(shmidStr, IPC_RMID, NULL);
  shmctl(shmidFlag, IPC_RMID, NULL);
  shmctl(shmidTurn, IPC_RMID, NULL);
}

//Exits if ERRNO is set
void errCheck(char progName[]) {
  if (errno) {
    fprintf(stderr, "%s: Error: %s\n", progName, strerror(errno));
    exit(1);
  }
}

//Prints usage and exits.
void badUse(char progName[], const char usage[]) {
    fprintf(stderr, usage, progName, progName);
    exit(1);
}

//HashTable Functions
void add_pid(int pid, int childInd) {
  struct entry *e;

  HASH_FIND_INT(map, &pid, e);  /* pid already in the hash? */
  if (e==NULL) {
    e = (struct entry*)malloc(sizeof(struct entry));
    e->id = pid;
    e->procInd = childInd;
    HASH_ADD_INT( map, id, e );  /* id: name of key field */
  }
  else {
    fprintf(stderr, "HashTable error...exiting.\n");
    exit(1);
  }
}

//Get the entry with pid argument as key.
struct entry* find_pid(int pid) {
  struct entry *e;

  HASH_FIND_INT( map, &pid, e);
  return e;
}

//Delete a PID - Index relation.
void delete_pid(struct entry *e) {
  HASH_DEL( map, e);
  free(e);
}

//Delete all the hash table entries.
void delete_all() {
  struct entry *current_entry, *tmp;

  HASH_ITER(hh, map, current_entry, tmp) {
    //fprintf(stderr, "HASH_ITER running\n"); //This should print maxChildCount times upon exiting.
    HASH_DEL(map,current_entry);
    free(current_entry);
  }
}
