#include "../lib/oss/parse_opt.h"
#include "../lib/oss/oss_funct.h"
#include "../lib/common/circ_queue.h"

/**************************************************************************/

//Administrative Functions
void exitIfError(char prog_name[]);
void cleanup();
int expected_term = 1;
void close_files();
void SHMDTRM();
void kill_SIGINT();
void kill_SIGTERM();

//Logging + Rand File
FILE* file_log;
FILE* file_rand;
int flg_verb = 0;
char log_buf[256];

//Metrics
uint num_mem_access = 0;
uint num_page_faults = 0;
uint total_access_time[2] = {0, 0};
//Throughput???

//Message Queue
int msg_id_q1; //Messages in here notify processes of request fulfillment
req_success_msg success_msg;

//Shared Memory
int shm_id_clock;
void* shm_addr_clock;
int shm_id_req_queue;
void* shm_addr_req_queue;

//Semaphores
int sem_proc_id; //Used to coordinate entries into proc critical section
union semun {
    int val;                 /* used for SETVAL only */
    //struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    //ushort *array;         /* used for GETALL and SETALL */
};

//Clock Related
uint* sim_clock;
uint next_gen_time[2]; //To store the next process spawn time.

//Request Queue & Request Related
CirQueue* req_queue;
CirQueue dev_queue; //For secondary storage swap-ins and swap-outs

//Second Chance/Clock Policy Algorithm BVs
uint BV_lru[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint BV_dirty_clean[8] = {0, 0, 0, 0, 0, 0, 0, 0};

//Page Tables, Memory
int32 sim_main_mem[MAX_FRAMES][2];
uint sim_page_tables[MAX_RUNNING_PROCS][MAX_PROC_PAGES];

//Process Accounting
int num_procs_to_run = 18;
int num_procs_running = 0;
int total_procs_spawned = 0;
int total_procs_terminated = 0;
Stack free_child_ids; //Tracks available childIds
entry* map = NULL;    //Associating PIDS with a childId
entry* holder = NULL; //Pointer for searching + deleting

/**************************************************************************/

int main(int argc, char* argv[]) {
  //Signal Handlers
  signal(SIGINT, kill_SIGINT);
  signal(SIGALRM, kill_SIGTERM);

  //Option Parsing
  static char usage[] = "%s: Usage: %s [-l log file location] [-v]\n";
  static char help[] = "\n-l    Filepath for the log to be written to. Must be writable.\n\
-v    Verbose logging. See Project Specs for additional information logged.\n";
  int c;
  while ((c = getopt(argc, argv, "hl:vn:")) != -1) {
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
      case 'n':
        ;
        if (!isNumber(optarg)) {
          errno = EINVAL;
        }
        else {
          num_procs_to_run = (int)strtol(optarg, NULL, 0);
          fprintf(stderr, "num procs to run: %d\n", num_procs_to_run);
        }
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

  //IPC Setup
  key_t shm_key_clock = ftok("./keyfile", 1);
  key_t shm_key_req_queue = ftok("./keyfile", 2);

  shm_id_clock = shmget(shm_key_clock, sizeof(uint) * 2, IPC_CREAT | 0644);
  shm_addr_clock = shmat(shm_id_clock, (void*)0, 0);

  shm_id_req_queue = shmget(shm_key_req_queue, sizeof(CirQueue), IPC_CREAT | 0644);
  shm_addr_req_queue = shmat(shm_id_req_queue, (void*)0, 0);
  exitIfError(argv[0]);

  //Semaphore Setup
  key_t sem_proc_key = ftok("./keyfile", 3);
  sem_proc_id = semget(sem_proc_key, 1, IPC_CREAT | 0666);
  exitIfError(argv[0]);

  union semun arg;
  arg.val = 1;
  semctl(sem_proc_id, 0, SETVAL, arg);
  exitIfError(argv[0]);

  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = -1;
  sb.sem_flg = 0;

  //Message Queue Setup
  key_t msg_key_q1 = ftok("./keyfile", 4);
  msg_id_q1 = msgget(msg_key_q1, IPC_CREAT | 0666);
  exitIfError(argv[0]);

  //Clock Setup
  sim_clock = shm_addr_clock;
  sim_clock[0] = 0, sim_clock[1] = 0;
  next_gen_time[0] = 0, next_gen_time[1] = 0; //First iteration will generate a process.

  //Request Queue Setup
  req_queue = shm_addr_req_queue;
  initCirQueue(req_queue);
  initCirQueue(&dev_queue);

  //Populate Id Stack
  Stack_Init(&free_child_ids);
  for (int k = MAX_RUNNING_PROCS - 1; k >= 0; k--) {
    Stack_Push(&free_child_ids, k);
  }

  //Page Table + Main Memory Initialization
  init_frames(sim_main_mem);
  init_tables(sim_page_tables);

  //Loop Temporary Variables
  int child_ind;
  char child_ind_str[5];
  int child_mtype;
  int pid;
  int req_page;
  int page_frame_index;
  uint req_head_time[2];
  int32 req_holder[6];
  int32 dev_req_holder[6];

  //Begin main loop
  while (1) {
    if (total_procs_terminated == 1) {
      break;
    }
    //Check if a process can be spawned
    if ( is_time_greater_eq(next_gen_time, sim_clock) && num_procs_running == 0 ) {
      if (num_procs_running < MAX_RUNNING_PROCS) {
        //Give it a free ID
        child_ind = Stack_Top(&free_child_ids);
        Stack_Pop(&free_child_ids);
        fprintf(stderr, "Free child ind: %d\n", child_ind);
        sprintf(child_ind_str, "%d", child_ind);

        pid = fork();
        exitIfError(argv[0]);
        total_procs_spawned += 1;

        if (pid == 0) {
          execl("./proc", "proc", child_ind_str, NULL);
        }

        //Add hashmap association
        add_pid(&map, pid, child_ind);

        num_procs_running += 1;
        if (flg_verb) {
          snprintf(log_buf, 256, "Process P%d (PID %d) generated.", child_ind, pid);
          log_msg(file_log, sim_clock, log_buf);
        }
      }

      set_next_gen_time(file_rand, sim_clock, next_gen_time);
    }

    //Critical Section to Process Requests
    sleep(1); //DELETE THIS LATER
    sb.sem_op = -1;
    semop(sem_proc_id, &sb, 1);
    exitIfError(argv[0]);

    fprintf(stderr, "Current Time: %d:%d\n", sim_clock[0], sim_clock[1]);

    //Check for request deadlock
    if (isFull(req_queue)) {
      //Fast forward to the time of the head request
      peak_head_time(req_queue, req_head_time);
      if (flg_verb) {
        snprintf(log_buf, 256, "OSS detected deadlock on memory requests. Fast forwarding to %d:%d\n", req_head_time[0], req_head_time[1]);
        log_msg(file_log, sim_clock, log_buf);
      }
      sim_clock[0] = req_head_time[0];
      sim_clock[1] = req_head_time[1];
    }

    if (isEmpty(req_queue) && !isEmpty(&dev_queue)) {
      //Fast forward to the completion of the head request
      peak_head_time(&dev_queue, req_head_time);
      if (flg_verb) {
        snprintf(log_buf, 256, "OSS detected deadlock on device requests. Fast forwarding to %d:%d", req_head_time[0], req_head_time[1]);
        log_msg(file_log, sim_clock, log_buf);
      }
      sim_clock[0] = req_head_time[0];
      sim_clock[1] = req_head_time[1];

      if (deQueue(&dev_queue, req_holder)) {
        //Exit if queue underflows.
        expected_term = 0;
        cleanup();
      }

      if (flg_verb) {
        snprintf(log_buf, 256, "Device queue returned request: Child %d (PID %d) | Page Index: %d | Operation: %d | Finished Time: %d:%d", \
                  req_holder[0], req_holder[1], req_holder[2], req_holder[3], req_holder[4], req_holder[5]);
        log_msg(file_log, sim_clock, log_buf);
      }

      //Simulate swap queue having swapped in the frame
      if ( (page_frame_index = get_first_free_frame(sim_main_mem)) == -1) {
        fprintf(stderr, "Corrupted page tables. Exiting.\n");
        expected_term = 0;
        cleanup();
      }
      sim_page_tables[req_holder[0]][req_holder[2]] = 1;
      mark_frame(BV_lru, BV_dirty_clean, page_frame_index);

      //Perform the request
      if (req_holder[3] == 0) {
        increment_time_read_op(sim_clock, total_access_time);
        if (flg_verb) {
          snprintf(log_buf, 256, "OSS completing read request for Child %d (PID %d).", req_holder[1], req_holder[0]);
          log_msg(file_log, sim_clock, log_buf);
        }
      }
      else {
        increment_time_write_op(sim_clock, total_access_time);
        if (flg_verb) {
          snprintf(log_buf, 256, "OSS completing write request for Child %d (PID %d).", req_holder[1], req_holder[0]);
          log_msg(file_log, sim_clock, log_buf);
        }
      }

      //Signal to the process whose request was fulfilled.
      //Produce a message
      holder = find_pid(&map, req_holder[1]);
      if (holder == NULL) {
        fprintf(stderr, "Corrupted Hashmap. Exiting.\n");
        expected_term = 0;
        cleanup();
      }
      if (holder->procInd != req_holder[0]) {
        fprintf(stderr, "Proc Ind MISMATCH!\n");
        expected_term = 0;
        cleanup();
      }
      child_mtype = holder->procInd + 1;
      construct_msg(&success_msg, child_mtype, req_holder[1], req_page);
      holder = NULL;

      //Send Message
      msgsnd(msg_id_q1, &success_msg, sizeof(success_msg.contents), 0);
      exitIfError(argv[0]);
    }

    if (!isEmpty(req_queue)) {
      //Process the first request ONLY if it's time is met.
      peak_head_time(req_queue, req_head_time);

      if (is_time_greater_eq(req_head_time, sim_clock)) {
        if (deQueue(req_queue, req_holder)) {
          //Exit if queue underflows.
          expected_term = 0;
          cleanup();
        }

        //Log received request
        if (flg_verb) {
          snprintf(log_buf, 256, "OSS received a request: Child %d (PID %d) | Byte Offset: %d | Operation: %d | Time: %d:%d", \
                  req_holder[1], req_holder[0], req_holder[2], req_holder[3], req_holder[4], req_holder[5]);
          log_msg(file_log, sim_clock, log_buf);
        }

        //Handle request based on operation type
        switch(req_holder[3]) {
          //Read Operation
          case 0:
            //Determine page of the request
            req_page = req_holder[2] / FRAME_PAGE_SIZE;

            //Run daemon?
            while (can_daemon_run(sim_main_mem)) {
              //Log a sweep
              if (flg_verb) {
                snprintf(log_buf, 256, "OSS detected < 10%% of free frames...performing sweep.");
                log_msg(file_log, sim_clock, log_buf);
              }
              sweep_frames(sim_main_mem, BV_lru, BV_dirty_clean, file_log, flg_verb);
              swap_out_frames(sim_clock, sim_main_mem, BV_lru, BV_dirty_clean, file_log, flg_verb);
            }

            //Check if it's in Memory
            if (sim_page_tables[req_holder[1]][req_page] == 1) {
              //Increment time
              increment_time_read_op(sim_clock, total_access_time);
              if (flg_verb) {
                snprintf(log_buf, 256, "OSS completing read request for Child %d (PID %d).\n", req_holder[1], req_holder[0]);
                log_msg(file_log, sim_clock, log_buf);
              }

              //Get the frame index
              if ( (page_frame_index = get_frame_index(sim_main_mem, req_holder[1], req_page)) == -1) {
                fprintf(stderr, "Corrupted page tables. Exiting.\n");
                expected_term = 0;
                cleanup();
              }
              //Mark bits
              mark_frame(BV_lru, BV_dirty_clean, page_frame_index);

              //Produce a message
              holder = find_pid(&map, req_holder[0]);
              if (holder == NULL) {
                fprintf(stderr, "Corrupted Hashmap. Exiting.\n");
                expected_term = 0;
                cleanup();
              }
              if (holder->procInd != req_holder[1]) {
                fprintf(stderr, "Proc Ind MISMATCH!\n");
                expected_term = 0;
                cleanup();
              }
              child_mtype = holder->procInd + 1;
              construct_msg(&success_msg, child_mtype, req_holder[0], req_page);
              holder = NULL;

              //Send Message
              msgsnd(msg_id_q1, &success_msg, sizeof(success_msg.contents), 0);
              exitIfError(argv[0]);

              num_mem_access += 1;
            }
            else {
              //Place the request into the secondary storage queue
              num_page_faults += 1;

              if (flg_verb) {
                snprintf(log_buf, 256, "Page fault detected. Sending the request to the swap queue.");
                log_msg(file_log, sim_clock, log_buf);
              }

              set_dev_req(dev_req_holder, req_holder[1], req_holder[0], req_page, req_holder[3], sim_clock);
              enQueue(&dev_queue, dev_req_holder);
            }
            break;
          //Write Operation
          case 1:
            //Determine page of the request
            req_page = req_holder[2] / FRAME_PAGE_SIZE;

            //Run daemon?
            while (can_daemon_run(sim_main_mem)) {
              //Log a sweep
              if (flg_verb) {
                snprintf(log_buf, 256, "OSS detected < 10%% of free frames...performing sweep.");
                log_msg(file_log, sim_clock, log_buf);
              }
              sweep_frames(sim_main_mem, BV_lru, BV_dirty_clean, file_log, flg_verb);
              swap_out_frames(sim_clock, sim_main_mem, BV_lru, BV_dirty_clean, file_log, flg_verb);
            }

            //Check if it's in Memory
            if (sim_page_tables[req_holder[1]][req_page] == 1) {
              //Increment time
              increment_time_write_op(sim_clock, total_access_time);
              if (flg_verb) {
                snprintf(log_buf, 256, "OSS completing write request for Child %d (PID %d).\n", req_holder[1], req_holder[0]);
                log_msg(file_log, sim_clock, log_buf);
              }

              //Get the frame index
              if ( (page_frame_index = get_frame_index(sim_main_mem, req_holder[1], req_page)) == -1) {
                fprintf(stderr, "Corrupted page tables. Exiting.\n");
                expected_term = 0;
                cleanup();
              }
              //Mark bits
              mark_frame(BV_lru, BV_dirty_clean, page_frame_index);

              //Produce a message
              holder = find_pid(&map, req_holder[0]);
              if (holder == NULL) {
                fprintf(stderr, "Corrupted Hashmap. Exiting.\n");
                expected_term = 0;
                cleanup();
              }
              if (holder->procInd != req_holder[1]) {
                fprintf(stderr, "Proc Ind MISMATCH!\n");
                expected_term = 0;
                cleanup();
              }
              child_mtype = holder->procInd + 1;
              construct_msg(&success_msg, child_mtype, req_holder[0], req_page);
              holder = NULL;

              //Send Message
              msgsnd(msg_id_q1, &success_msg, sizeof(success_msg.contents), 0);
              exitIfError(argv[0]);

              num_mem_access += 1;
            }
            else {
              //Place the request into the secondary storage queue
              num_page_faults += 1;

              if (flg_verb) {
                snprintf(log_buf, 256, "Page fault detected. Sending the request to the swap queue.");
                log_msg(file_log, sim_clock, log_buf);
              }

              set_dev_req(dev_req_holder, req_holder[1], req_holder[0], req_page, req_holder[3], sim_clock);
              enQueue(&dev_queue, dev_req_holder);
            }
            break;
          //Termination
          case 2:
            //Free up page tables + Frames
            free_proc_table(sim_page_tables[req_holder[1]]);
            free_frames(sim_main_mem, req_holder[1]);

            //Remove entry in hash table
            holder = find_pid(&map, req_holder[0]);
            if (holder == NULL) {
              fprintf(stderr, "Corrupted Hashmap. Exiting.\n");
              expected_term = 0;
              cleanup();
            }
            if (holder->procInd != req_holder[1]) {
              fprintf(stderr, "Proc Ind MISMATCH!\n");
              expected_term = 0;
              cleanup();
            }
            child_mtype = holder->procInd + 1;
            construct_msg(&success_msg, child_mtype, req_holder[0], -1);
            delete_pid(&map, holder);
            holder = NULL;

            //Send Message
            msgsnd(msg_id_q1, &success_msg, sizeof(success_msg.contents), 0);
            exitIfError(argv[0]);

            if (flg_verb) {
              snprintf(log_buf, 256, "OSS has terminated Child %d (PID %d) and freed its page table and frames.", \
                      req_holder[1], req_holder[0]);
              log_msg(file_log, sim_clock, log_buf);
            }
            //Wait for it to exit
            waitpid(req_holder[0], NULL, 0);
            fprintf(stderr, "successful child termination.\n");
            total_procs_terminated += 1;
            break;
        }
      }
    }

    //Check the device queue and fast forward to the head request's completion time.



    sb.sem_op = 1;
    semop(sem_proc_id, &sb, 1);
    exitIfError(argv[0]);
  }

  //Wait for all children to return (Delete this later)
  while ( (pid = waitpid(-1, NULL, 0)) ) {
   if (errno == ECHILD) {
      break;
    }
  }

  fprintf(stderr, "Current Time: %d:%d\n", sim_clock[0], sim_clock[1]);
  fprintf(stderr, "OSS Exiting\n");

  cleanup();
}

/**************************************************************************/

void exitIfError(char prog_name[]) {
  if (errCheck(prog_name, errno)) {
    fprintf(stderr, "Master exiting due to error. Sending SIGTERM to children (if spawned)...\n");
    cleanup();
  }
}

void cleanup() {
  close_files();
  SHMDTRM();
  delete_all(&map);

  if (!expected_term) {
    fprintf(stderr, "Unexpected Termination.\n");
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

  if (shm_addr_req_queue)
    shmdt(shm_addr_req_queue);
  if (shm_id_req_queue)
    shmctl(shm_id_req_queue, IPC_RMID, NULL);

  semctl(sem_proc_id, 0, IPC_RMID);
  msgctl(msg_id_q1, IPC_RMID, NULL);
}

//Modify this so that OSS waits until all children exit.
void kill_SIGINT() {
  fprintf(stderr, "PID: %ld Master process exiting due to interrupt. Sending SIGINT to children...\n", (long)getpid());
  kill(0, SIGINT);
  SHMDTRM();
  exit(1);
}

void kill_SIGTERM() {
  fprintf(stderr, "PID: %ld Master process exiting due to error. Sending SIGTERM to children...\n", (long)getpid());

  signal(SIGTERM, SIG_IGN); //OSS ignores termination signal
  kill(-getpid(), SIGTERM); //Send downstream
  //Wait for all PIDS to return
  pid_t pid;
  while ( (pid = waitpid(-1, NULL, 0)) ) {
   if (errno == ECHILD) {
      break;
    }
  }

  fprintf(stderr, "Done. Master Exiting...\n");
  expected_term = 1;
  cleanup();
}
