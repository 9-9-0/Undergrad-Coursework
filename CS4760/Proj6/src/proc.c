#include "../lib/proc/proc_funct.h"
#include "../lib/common/circ_queue.h"

/**************************************************************************/
//Administrative Functions
void exitIfError(char prog_name[]);
void cleanup();
void close_files();
void SHMDT();
void exitSIGINT();
void exitSIGTERM();

//Files
FILE* rand_file;

//Shared Memory
int shm_id_clock;
void* shm_addr_clock;
int shm_id_req_queue;
void* shm_addr_req_queue;

//Semaphores
int sem_proc_id; //Used to coordinate entries into proc critical section

//Message Queue
int msg_id_q1; //Messages in here notify processes of request fulfillment
req_success_msg success_msg;

//Request Queue
CirQueue* req_queue;

//Clock Related
uint* sim_clock;

//Termination Counters
int num_requests = 0;   //Total memory requests made
int next_num_check = 0; //Next number of requests to check termination at

int child_ind;
int child_mtype;
int pid;

//Request Related
int32 req_contents[6];

/**************************************************************************/

int main(int argc, char* argv[]) {
  //Signal Handling
  signal(SIGINT, exitSIGINT);
  signal(SIGTERM, exitSIGTERM);

  //Random File Setup
  rand_file = fopen("/dev/urandom", "r");
  if (rand_file == NULL) {
    exit(1);
  }

  //IPC Setup
  key_t shm_key_clock = ftok("./keyfile", 1);
  key_t shm_key_req_queue = ftok("./keyfile", 2);

  shm_id_clock = shmget(shm_key_clock, sizeof(uint) * 2, 0);
  shm_addr_clock = shmat(shm_id_clock, (void*)0, 0);

  shm_id_req_queue = shmget(shm_key_req_queue, sizeof(CirQueue), 0);
  shm_addr_req_queue = shmat(shm_id_req_queue, (void*)0, 0);
  exitIfError(argv[0]);

  //Request Queue Setup
  req_queue = shm_addr_req_queue;

  //Clock Setup
  sim_clock = shm_addr_clock;

  //Semaphore Setup
  key_t sem_proc_key = ftok("./keyfile", 3);
  sem_proc_id = semget(sem_proc_key, 1, 0);
  exitIfError(argv[0]);

  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = -1;
  sb.sem_flg = 0;

  //Message Queue Setup
  key_t msg_key_q1 = ftok("./keyfile", 4);
  msg_id_q1 = msgget(msg_key_q1, 0);
  exitIfError(argv[0]);

  //Get ChildIndex
  pid = getpid();
  child_ind = (int)strtol(argv[1], NULL, 0); //Set pc block (shared mem) index
  child_mtype = child_ind + 1;

  //fprintf(stderr, "Child %d PID %d spawned. Exiting...\n", child_ind, getpid());

  //Set the first memory reference count checkpoint
  set_next_check_count(rand_file, &next_num_check);

  //Begin Request Loop
  while (1) {
    //Check for termination
    if (num_requests == next_num_check) {
      if (check_term(rand_file)) {
        break;
      }
    }

    //Enter Critical Section To Create a Request
    sb.sem_op = -1;
    semop(sem_proc_id, &sb, 1);
    exitIfError(argv[0]);

    set_request(rand_file, req_contents, sim_clock, 0, child_ind, pid);
    submit_request(req_queue, req_contents);
    fprintf(stderr, "Child generated req_contents: %d, %d, %d, %d %d:%d\n", req_contents[0], req_contents[1], req_contents[2], req_contents[3], req_contents[4], req_contents[5]);

    sb.sem_op = 1;
    semop(sem_proc_id, &sb, 1);
    exitIfError(argv[0]);

    //Wait for its request to be fulfilled
    msgrcv(msg_id_q1, &success_msg, sizeof(req_success_msg_contents), child_mtype, 0);

    //Sanity Checks
    if (success_msg.contents.PID != pid) {
      fprintf(stderr, "Forced termination or Child PID mismatch in received message. Exiting...\n");
      cleanup();
      exit(1);
    }
  }

  //Enter Critical Section To Create Termination Request
  sb.sem_op = -1;
  semop(sem_proc_id, &sb, 1);
  exitIfError(argv[0]);

  set_request(rand_file, req_contents, sim_clock, 1, child_ind, pid);
  submit_request(req_queue, req_contents);
  fprintf(stderr, "Child generated req_contents: %d, %d, %d, %d %d:%d\n", req_contents[0], req_contents[1], req_contents[2], req_contents[3], req_contents[4], req_contents[5]);

  sb.sem_op = 1;
  semop(sem_proc_id, &sb, 1);
  exitIfError(argv[0]);

  fprintf(stderr, "Child process waiting for request to fulfill.\n");
  //Wait for its termination request to be fulfilled
  msgrcv(msg_id_q1, &success_msg, sizeof(req_success_msg_contents), child_mtype, 0);

  fprintf(stderr, "Child process terminating normally.\n");

  cleanup();
  return 0;
}

/**************************************************************************/

void exitIfError(char prog_name[]) {
  if (errCheck(prog_name, errno)) {
    fprintf(stderr, "PID %d ERRNO %d\n", getpid(), errno);
    cleanup();
  }
}

void cleanup() {
  SHMDT();
  close_files();
  exit(1);
}

void SHMDT() {
  shmdt(shm_addr_clock);
  shmdt(shm_addr_req_queue);
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
