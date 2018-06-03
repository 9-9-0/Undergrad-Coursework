#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include "../lib/parse_opt.h"
#include "../lib/oss_management.h"
#include "../lib/common_management.h"
#include "../lib/data_structs/stack.h"

/**************************************************************************/

//Administrative Functions
void exitIfError(char prog_name[]);
void cleanup();
int expected_term = 0;
void close_files();
void SHMDTRM();
void kill_SIGINT();
void kill_SIGTERM();

//Logging
FILE* file_log;
FILE* file_rand;
int flg_verb = 0;
char log_buf[256];

//Shared Memory
int shm_id_clock;
int shm_id_res_blocks;
void* shm_addr_clock;
void* shm_addr_res_blocks;

//Message Queue & Related
int msg_id_q1;
int msg_id_q2;
res_req_msg received_msg; //Receive a request message through q1 from proc
res_resp_msg notify_msg;  //Send a message to proc through q2

//Clock & Related
uint32_t* sim_clock;
uint32_t next_gen_time[2]; //To store the next process spawn time.

//Resources & Matrices
resource resources[20];       //NOTE: delete this later.
resource_control* res_blocks;
List* pid_allocations[20];
List* pid_requests[20];

int32_t available_matrix[max_resource_types];
int32_t alloc_matrix[max_child_procs][max_resource_types];
int32_t request_matrix[max_child_procs][max_resource_types];

Grant_Stack granted_child_res;
int* grant_temp;

Stack deadlocked_child_ids; //0 indexed. Increment values by 1 before use.
int next_read_thresh = deadlock_check_interval;
int is_deadlocked = 0;
int deadlocked_child_id;
int deadlocked_child_pid;
int contiguous_deadlocks = 0;

//Process Accounting
int num_procs_running = 0;
Stack free_child_ids; //Tracks available childIds
entry* map = NULL;    //Associating PIDS with a childId
entry* holder = NULL; //Pointer for searching + deleting
child_id_entry* child_id_map = NULL;    //Associating Child Indexes with a PID (i.e. reverse of map)
child_id_entry* child_id_holder = NULL; //Pointer for searching + deleting

//Metrics
int lines_logged = 0;
int read_messages = 0;
int requests_granted = 0;    //# of resource requests granted
int procs_normal_term = 0;   //# of processes terminated via case 2 message
int procs_force_term = 0;    //# of processes terminated to free deadlock
int total_procs_spawned = 0;
int deadlock_algo_run_count = 0;
int deadlocks_detected = 0;


/**************************************************************************/

int main(int argc, char* argv[]) {
  //Signal Handling
  signal(SIGINT, kill_SIGINT);
  signal(SIGALRM, kill_SIGTERM);

  //Option Parsing
  static char usage[] = "%s: Usage: %s [-l log file location] [-v]\n";
  static char help[] = "\n-l    Filepath for the log to be written to. Must be writable.\n\
-v    Verbose logging. See Project Specs for additional information logged.\n";
  int c;
  while ((c = getopt(argc, argv, "hl:v")) != -1) {
    exitIfError(argv[0]);
    switch (c) {
      case 'h':
        ;
        getHelp(argv[0], usage, help);
        break;
      case 'l':
        ;
        file_log = fopen(optarg, "w+");
        setbuf(file_log, NULL);
        break;
      case 'v':
        ;
        flg_verb = 1;
        break;
      case '?':
        badUse(argv[0], usage);
        break;
    }
  }
  exitIfError(argv[0]);
  if (file_log == NULL) {
    fprintf(stderr, "Missing required arguments.\n");
    badUse(argv[0], usage);
  }

  //Setup rand file
  file_rand = fopen("/dev/urandom", "r");
  if (file_rand == NULL) {
    fprintf(stderr, "Could not read from /dev/random.\n");
    cleanup();
  }

  //IPC setup
  key_t shm_key_clock = ftok("./keyfile", 1);
  key_t shm_key_res_blocks = ftok("./keyfile", 2);
  key_t msg_key_q1 = ftok("./keyfile", 3);
  key_t msg_key_q2 = ftok("./keyfile", 4);

  shm_id_clock = shmget(shm_key_clock, sizeof(uint32_t) * 2, IPC_CREAT | 0644);
  shm_addr_clock = shmat(shm_id_clock, (void*)0, 0);

  shm_id_res_blocks = shmget(shm_key_res_blocks, sizeof(resource_control) * 20, IPC_CREAT | 0644);
  shm_addr_res_blocks = shmat(shm_id_res_blocks, (void*)0, 0);

  msg_id_q1 = msgget(msg_key_q1, IPC_CREAT | 0666);
  msg_id_q2 = msgget(msg_key_q2, IPC_CREAT | 0666);
  exitIfError(argv[0]);

  //Clock Setup
  sim_clock = shm_addr_clock;
  sim_clock[0] = 0, sim_clock[1] = 0;
  next_gen_time[0] = 0, next_gen_time[1] = 0; //First iteration will generate a process.

  //Resource Creation
  res_blocks = shm_addr_res_blocks;
  init_all_res(res_blocks, file_rand);
  init_shareable_res(res_blocks, file_rand);
  init_block_lists(res_blocks, pid_allocations, pid_requests);
  display_all_blocks(file_log, &lines_logged, res_blocks);

  //Initial Matrix Setup
  clear_alloc_matrix(alloc_matrix);
  clear_avail_matrix(available_matrix);

  //Populate Id Stack
  Stack_Init(&free_child_ids);
  for (int k = max_child_procs; k >= 1; k--) {
    Stack_Push(&free_child_ids, k);
  }

  alarm(2); //Setup 2 second maximum execution time

  //Child Indexes, see README
  char child_ind_str[5];
  int child_ind;
  pid_t pid;

  //Main loop
  while(1 && lines_logged < 100000) {
    //Check if a process can be spawned
    if ( time_is_greater(next_gen_time, sim_clock) ) {
      if (num_procs_running < max_child_procs) {
        //Give it a free ID
        child_ind = Stack_Top(&free_child_ids);
        Stack_Pop(&free_child_ids);
        sprintf(child_ind_str, "%d", child_ind);

        pid = fork();
        exitIfError(argv[0]);
        total_procs_spawned += 1;

        if (pid == 0) {
          execl("./proc", "proc", child_ind_str, NULL);
        }

        //Add association to hashmaps
        add_pid(&map, pid, child_ind);
        add_child_id(&child_id_map, child_ind, pid);

        num_procs_running += 1;
        if (flg_verb) {
          snprintf(log_buf, 256, "Process P%d (PID %d) generated.", child_ind, pid);
          log_msg(file_log, &lines_logged, sim_clock, log_buf);
        }
      }

      set_next_gen_time(file_rand, sim_clock, next_gen_time);
    }

    //Check for any requests
    while (msgrcv(msg_id_q1, &received_msg, sizeof(req_contents), 0, IPC_NOWAIT) != -1) {
      //Process Request
      read_messages += 1;
      switch (received_msg.contents.action) {
        case 0:
          //Resource Request
          if (flg_verb) {
            snprintf(log_buf, 256, "OSS receiving request for R%d from P%ld (PID %d) .", received_msg.contents.res_id, received_msg.mtype, received_msg.contents.PID);
            log_msg(file_log, &lines_logged, sim_clock, log_buf);
          }

          //Grant if resource is shared/instances are free
          if (is_res_free(res_blocks, received_msg.contents.res_id) > 0) {
            if (flg_verb) {
              snprintf(log_buf, 256, "OSS granting request for R%d from P%ld (PID %d).", received_msg.contents.res_id, received_msg.mtype, received_msg.contents.PID);
              log_msg(file_log, &lines_logged, sim_clock, log_buf);
            }

            add(received_msg.contents.PID, res_blocks[received_msg.contents.res_id].pid_allocations);

            //Signal back through the 2nd message queue to proc.
            construct_notify_msg(&notify_msg, received_msg.mtype, received_msg.contents.PID, received_msg.contents.res_id);
            msgsnd(msg_id_q2, &notify_msg, sizeof(notify_msg.contents), 0);
            exitIfError(argv[0]);

            //Log Allocation Table if necessary
            requests_granted += 1;
            if ( (requests_granted % 20 == 0) && flg_verb) {
              gen_alloc_request_matrices(alloc_matrix, request_matrix, res_blocks, &map);
              log_alloc_matrix(file_log, &lines_logged, sim_clock, alloc_matrix);
            }
          }
          //Block if not.
          else {
            if (flg_verb) {
              snprintf(log_buf, 256, "OSS unable to grant request for R%d from P%ld (PID %d) .", received_msg.contents.res_id, received_msg.mtype, received_msg.contents.PID);
              log_msg(file_log, &lines_logged, sim_clock, log_buf);
            }

            add(received_msg.contents.PID, res_blocks[received_msg.contents.res_id].pid_requests);
          }
          break;
        case 1:
          //Resource Release
          if (flg_verb) {
            snprintf(log_buf, 256, "OSS receiving release of R%d from P%ld (PID %d).", received_msg.contents.res_id, received_msg.mtype, received_msg.contents.PID);
            log_msg(file_log, &lines_logged, sim_clock, log_buf);
          }

          if ( free_res(res_blocks, received_msg.contents.res_id, received_msg.contents.PID) != 1) {
            fprintf(stderr, "free_res() did not free any requests...\n");
          };

          if (flg_verb) {
            snprintf(log_buf, 256, "OSS releasing R%d from P%ld (PID %d).", received_msg.contents.res_id, received_msg.mtype, received_msg.contents.PID);
            log_msg(file_log, &lines_logged, sim_clock, log_buf);
          }

          //Grant any blocked requests TESTING
          grant_freed_res(file_log, &lines_logged, flg_verb, 0, sim_clock, &granted_child_res, res_blocks);
          while ( (grant_temp = Grant_Stack_Top(&granted_child_res)) != NULL ) {
            //Find the child id of the PID
            holder = find_pid(&map, grant_temp[0]);
            if (holder == NULL) {
              fprintf(stderr, "Corrupted Hashmap from GRANTING case 1. Exiting.\n");
              cleanup();
            }

            construct_notify_msg(&notify_msg, holder->procInd, grant_temp[0], grant_temp[1]);
            msgsnd(msg_id_q2, &notify_msg, sizeof(notify_msg.contents), 0);
            exitIfError(argv[0]);

            Grant_Stack_Pop(&granted_child_res);
          }

          //Signal back through the 2nd message queue to proc.
          construct_notify_msg(&notify_msg, received_msg.mtype, received_msg.contents.PID, received_msg.contents.res_id);
          msgsnd(msg_id_q2, &notify_msg, sizeof(notify_msg.contents), 0);
          exitIfError(argv[0]);
          break;
        case 2:
          //Process Termination
          if (flg_verb) {
            snprintf(log_buf, 256, "OSS receiving notice of termination from P%ld (PID %d).", received_msg.mtype, received_msg.contents.PID);
            log_msg(file_log, &lines_logged, sim_clock, log_buf);
          }

          //Clear the map entries + push child index
          holder = find_pid(&map, received_msg.contents.PID);
          if (holder == NULL) {
            fprintf(stderr, "Corrupted Hashmap. Exiting.\n");
            cleanup();
          }
          if (holder->procInd != received_msg.mtype) {
            fprintf(stderr, "Proc Ind MISMATCH!\n");
          }
          delete_pid(&map, holder);
          holder = NULL;
          //Clear the other map
          child_id_holder = find_child_id(&child_id_map, received_msg.mtype);
          if (child_id_holder == NULL) {
            fprintf(stderr, "Corrupted child_id -> pid hashmap. Exiting\n");
            cleanup();
          }
          if (child_id_holder->pid != received_msg.contents.PID) {
            fprintf(stderr, "PID MISMATCH!\n");
          }
          delete_child_id(&child_id_map, child_id_holder);
          child_id_holder = NULL;
          //Push it back
          Stack_Push(&free_child_ids, received_msg.mtype);

          //Clear the PID from all lists. Brute force iteration for now.
          clear_all_pid(file_log, &lines_logged, flg_verb, sim_clock, res_blocks, received_msg.contents.PID);

          //Grant any blocked requests for the freed resources.
          Grant_Stack_Init(&granted_child_res);
          grant_freed_res(file_log, &lines_logged, flg_verb, 0, sim_clock, &granted_child_res, res_blocks);

          //Send Messages to Processes that were able to be allocated
          while ( (grant_temp = Grant_Stack_Top(&granted_child_res)) != NULL ) {
            //Find the child id of the PID
            holder = find_pid(&map, grant_temp[0]);
            if (holder == NULL) {
              fprintf(stderr, "Corrupted Hashmap from GRANTING case 2. Exiting.\n");
              cleanup();
            }

            construct_notify_msg(&notify_msg, holder->procInd, grant_temp[0], grant_temp[1]);
            msgsnd(msg_id_q2, &notify_msg, sizeof(notify_msg.contents), 0);
            exitIfError(argv[0]);

            Grant_Stack_Pop(&granted_child_res);
          }

          //Send message to proc that's terminating
          construct_notify_msg(&notify_msg, received_msg.mtype, received_msg.contents.PID, received_msg.contents.res_id);
          msgsnd(msg_id_q2, &notify_msg, sizeof(notify_msg.contents), 0);
          exitIfError(argv[0]);
          //fprintf(stderr, "Waiting for termination \n");
          waitpid(received_msg.contents.PID, NULL, 0);

          num_procs_running -= 1; //Decrement child running child counter
          procs_normal_term += 1; //Increment # of normal terminations
          break;
      }
    }

    if (errno == 42) {
      errno = 0;
    }
    exitIfError(argv[0]);

    //Check for deadlocks every 5 requests handled
    if (read_messages > next_read_thresh) {
      next_read_thresh += deadlock_check_interval;
      if (flg_verb) {
        snprintf(log_buf, 256, "OSS checking for DEADLOCKS...");
        log_msg(file_log, &lines_logged, sim_clock, log_buf);
      }

      is_deadlocked = 0;
      contiguous_deadlocks = 0;

      do {
        Stack_Init(&deadlocked_child_ids);
        gen_avail_matrix(available_matrix, res_blocks);
        gen_alloc_request_matrices(alloc_matrix, request_matrix, res_blocks, &map);
        is_deadlocked = detect_deadlock(&deadlocked_child_ids, alloc_matrix, request_matrix, available_matrix);
        deadlock_algo_run_count += 1;

        if (is_deadlocked == 1) {
          deadlocks_detected += 1;
          contiguous_deadlocks += 1;

          if (contiguous_deadlocks < 2) {
            snprintf(log_buf, 256, "DEADLOCK detected.");
            log_msg(file_log, &lines_logged, sim_clock, log_buf);
          }
          else {
            fprintf(file_log, "%26s%s", "", "System still deadlocked...\n");
            lines_logged += 1;
          }
          //Log deadlocked processes, get the bottom one to kill
          deadlocked_child_id = log_deadlocked_procs(file_log, &lines_logged, &deadlocked_child_ids);

          //Get the PID of the child id.
          child_id_holder = find_child_id(&child_id_map, deadlocked_child_id);
          if (child_id_holder == NULL) {
            fprintf(stderr, "CORRUPTED CHILD_ID -> PID HASHMAP \n");
            cleanup();
          }
          deadlocked_child_pid = child_id_holder->pid;
          //Get the pid->child entry and also delete it
          holder = find_pid(&map, deadlocked_child_pid);
          if (holder == NULL) {
            fprintf(stderr, "CORRUPTED PID -> CHILD_ID HASHMAP \n");
            cleanup();
          }
          delete_pid(&map, holder);
          delete_child_id(&child_id_map, child_id_holder);

          //Log attempt to resolve
          log_free_deadlock(file_log, &lines_logged, deadlocked_child_id, deadlocked_child_pid);
          procs_force_term += 1;

          //Clear the message queue of all messages of the process to be killed.
          while (msgrcv(msg_id_q1, &received_msg, sizeof(req_contents), deadlocked_child_id, IPC_NOWAIT) != -1);
          if (errno == 42) {
            errno = 0;
          }
          exitIfError(argv[0]);

          //Clear its allocations and requests
          clear_all_pid(file_log, &lines_logged, 1, sim_clock, res_blocks, deadlocked_child_pid);

          //Grant freed
          Grant_Stack_Init(&granted_child_res);
          grant_freed_res(file_log, &lines_logged, 1, 1, sim_clock, &granted_child_res, res_blocks);

          //Send Messages to Processes that were able to be allocated
          while ( (grant_temp = Grant_Stack_Top(&granted_child_res)) != NULL ) {
            //Find the child id of the PID
            holder = find_pid(&map, grant_temp[0]);
            if (holder == NULL) {
              fprintf(stderr, "Corrupted Hashmap from GRANTING case 2. Exiting.\n");
              cleanup();
            }

            construct_notify_msg(&notify_msg, holder->procInd, grant_temp[0], grant_temp[1]);
            msgsnd(msg_id_q2, &notify_msg, sizeof(notify_msg.contents), 0);
            exitIfError(argv[0]);

            Grant_Stack_Pop(&granted_child_res);
          }

          //Kill the process.
          kill(deadlocked_child_pid, SIGUSR1);
          exitIfError(argv[0]);
          waitpid(deadlocked_child_pid, NULL, 0);
          exitIfError(argv[0]);
        }
        else {
          if (contiguous_deadlocks > 0) {
            fprintf(file_log, "%26s%s", "", "DEADLOCK Resolved!\n");
            lines_logged += 1;
          }
          else {
            if (flg_verb) {
              fprintf(file_log, "%26s%s", "", "No deadlock detected.\n");
              lines_logged += 1;
            }
          }
        }
      } while (is_deadlocked);
    }

    increment_time(sim_clock);
  }

  cleanup();
  return 0;
}

void exitIfError(char prog_name[]) {
  if (errCheck(prog_name, errno)) {
    fprintf(stderr, "Master exiting due to error. Sending SIGTERM to children...\n");
    cleanup();
  }
}

void cleanup() {
  delete_all(&map); //Cleanup Hash Table
  delete_all_child_ids(&child_id_map);
  free_block_lists(res_blocks); //Clear Resource Desc. Linked Lists
  close_files();
  SHMDTRM();

  if (!expected_term) {
    kill(0, SIGTERM);
  }

  if (errno) {
    strerror(errno);
    exit(1);
  }
  exit(0);
}

void close_files() {
  if (file_log) {
    fclose(file_log);
  }
  if (file_rand) {
    fclose(file_rand);
  }
}

void SHMDTRM() {
  if (shm_addr_clock)
    shmdt(shm_addr_clock);
  if (shm_id_clock)
    shmctl(shm_id_clock, IPC_RMID, NULL);

  if (shm_addr_res_blocks)
    shmdt(shm_addr_res_blocks);
  if (shm_id_res_blocks)
    shmctl(shm_id_res_blocks, IPC_RMID, NULL);

  msgctl(msg_id_q1, IPC_RMID, NULL);
  msgctl(msg_id_q2, IPC_RMID, NULL);
}

void kill_SIGINT() {
  fprintf(stderr, "PID: %ld Master process exiting due to interrupt. Sending SIGINT to children...\n", (long)getpid());
  kill(0, SIGINT);
  SHMDTRM();
  exit(1);
}

void kill_SIGTERM() {
  //print_lists(res_blocks);

  fprintf(stderr, "# of requests Granted: %d\n", requests_granted);
  fprintf(stderr, "# of normally terminated processes: %d\n", procs_normal_term);
  fprintf(stderr, "# of forcefully terminated processes: %d\n", procs_force_term);
  fprintf(stderr, "# of total processes spawned: %d\n", total_procs_spawned);
  fprintf(stderr, "# of times deadlock detection was run: %d\n", deadlock_algo_run_count);
  fprintf(stderr, "# of deadlocks detected: %d\n", deadlocks_detected);
  fprintf(stderr, "Messages read: %d\n", read_messages);
  fprintf(stderr, "Lines logged: %d\n", lines_logged);

  fprintf(stderr, "PID: %ld Master process exiting due to timeout. Sending SIGUSR2 to children...\n", (long)getpid());

  signal(SIGUSR2, SIG_IGN);
  kill(-getpid(), SIGUSR2);
  //Wait for all PIDS to return
  pid_t pid;
  while ( (pid = waitpid(-1, NULL, 0)) ) {
   if (errno == ECHILD) {
      break;
    }
  }

  fprintf(stderr, "DONE.\n");
  expected_term = 1;
  cleanup();
  exit(1);
}
