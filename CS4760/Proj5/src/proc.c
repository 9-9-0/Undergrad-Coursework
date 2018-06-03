#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stddef.h>
#include "../lib/common_management.h"
#include "../lib/proc_management.h"
#include "../lib/linked_list.h"
#include "../lib/resource.h"

/**************************************************************************/

//Administrative Functions
void exitIfError(char prog_name[]);
void cleanup();
void SHMDT();
void exitSIGINT();
void exitSIGTERM();
void exitDEADLOCK();
void exitTIMEOUT();
void close_files();

//Files
FILE* rand_file;

//Message Queue & Related
int msg_id_q1;
int msg_id_q2;
res_req_msg request_msg;
res_resp_msg receive_msg;

//Shared Memory
int shm_id_clock;
void* shm_addr_clock;

//Clock & Time Checkpoints
uint32_t* sim_clock;
uint32_t next_action_time[2];
uint32_t next_term_check_time[2];

//List of Resource IDs Currently Held
List* held_resources;

//Misc
int child_ind = -1;
int action;
int res_id;
int res_release_id;

/**************************************************************************/

int main(int argc, char* argv[]) {
  //Signal Handling Setup
  signal(SIGINT, exitSIGINT);
  signal(SIGTERM, exitSIGTERM);
  signal(SIGUSR1, exitDEADLOCK);
  signal(SIGUSR2, exitTIMEOUT);

  //Random File Setup
  rand_file = fopen("/dev/urandom", "r");
  if (rand_file == NULL) {
    exit(1);
  }

  //IPC Setup
  key_t shm_key_clock = ftok("./keyfile", 1);
  key_t msg_key_q1 = ftok("./keyfile", 3);
  key_t msg_key_q2 = ftok("./keyfile", 4);

  shm_id_clock = shmget(shm_key_clock, sizeof(uint32_t) * 2, 0);
  shm_addr_clock = shmat(shm_id_clock, (void*)0, 0);
  exitIfError(argv[0]);

  msg_id_q1 = msgget(msg_key_q1, 0);
  msg_id_q2 = msgget(msg_key_q2, 0);
  exitIfError(argv[0]);

  //Resource List Setup
  held_resources = new_list();

  //Clock Setup
  sim_clock = shm_addr_clock;
  next_action_time[0] = 0, next_action_time[1] = 0;
  next_term_check_time[0] = 0, next_term_check_time[1] = 0;

  //Get child index
  child_ind = atoi(argv[1]);
  //fprintf(stderr, "CHILD IND %d\n", child_ind);
  //fprintf(stderr, "%d CHILD RECEIVED INDEX: %d\n", getpid(), child_ind);
  //fprintf(stderr, "PROC RECEIVED TIME: %d:%d\n", sim_clock[0], sim_clock[1]);

  while(1) {
    if ( time_is_greater(next_term_check_time, sim_clock) ) {

      set_next_term_check(rand_file, sim_clock, next_term_check_time);

      if ( test_termination(rand_file) ) {
        //Request Termination
        construct_msg(&request_msg, child_ind, getpid(), 0, 2);
        msgsnd(msg_id_q1, &request_msg, sizeof(request_msg.contents), 0);
        exitIfError(argv[0]);
        break;
      }
    }

    if ( time_is_greater(next_action_time, sim_clock) ) {
      set_next_action_check(rand_file, sim_clock, next_action_time);

      action = get_rand(rand_file, 0, 1);
      //If currently holding a resource, choose between release + request
      if (size(held_resources)) {
        switch (action) {
          case 0:
            goto REQUEST;
            break;
          case 1:
            //Get the first resource it's holding.
            res_release_id = del_first(held_resources);
            //fprintf(stderr, "RELEASING %d\n", res_release_id);

            //Send a message to release
            construct_msg(&request_msg, child_ind, getpid(), res_release_id, 1);
            msgsnd(msg_id_q1, &request_msg, sizeof(request_msg.contents), 0);
            //fprintf(stderr, "%d SENDING RELEASE MSG\n", getpid());
            exitIfError(argv[0]);

            //Wait for acknowledgement
            msgrcv(msg_id_q2, &receive_msg, sizeof(receive_msg.contents), child_ind, 1);
            break;
        }
      }
      //If not holding a resource, just request
      else {
        REQUEST:

        res_id = get_rand(rand_file, 0, max_resource_types - 1);

        //Send Request Message
        //fprintf(stderr, "%d SENDING REQUEST MSG\n", getpid());
        construct_msg(&request_msg, child_ind, getpid(), res_id, 0);
        msgsnd(msg_id_q1, &request_msg, sizeof(request_msg.contents), 0);
        exitIfError(argv[0]);

        //WAIT for request to be granted
        msgrcv(msg_id_q2, &receive_msg, sizeof(resp_contents), child_ind, 0);

        //Sanity Check
        if (receive_msg.contents.res_id != res_id) {
          fprintf(stderr, "PROC %d res_id MISMATCH\n", getpid());
        }

        add(res_id, held_resources);
      }
    }
  }

  //Wait for termination acknowledgement
  msgrcv(msg_id_q2, &receive_msg, sizeof(receive_msg.contents), child_ind, 0);

  cleanup();
  exit(0);
}

void exitIfError(char prog_name[]) {
  if (errCheck(prog_name, errno)) {
    fprintf(stderr, "****%d ERRNO %d\n", getpid(), errno);
    cleanup();
  }
}

void cleanup() {
  SHMDT();
  destroy(held_resources);
  close_files();
  exit(1);
}

void SHMDT() {
  shmdt(shm_addr_clock);
}

void close_files() {
  if (rand_file) {
    fclose(rand_file);
  }
}

void exitSIGINT() {
  fprintf(stderr, "PID: %ld received interrupt signal. Exiting...\n", (long)getpid());
  cleanup();
}

void exitSIGTERM() {
  fprintf(stderr, "PID: %ld recieved notification of master process time out. Exiting...\n", (long)getpid());
  cleanup();
}

void exitDEADLOCK() {
  fprintf(stderr, "PID: %ld exiting in attempt to resolve deadlock...\n", (long)getpid());
  cleanup();
}

void exitTIMEOUT() {
  fprintf(stderr, "PID: %ld exiting due to master timeout (expected)...\n", (long)getpid());
  cleanup();
}
