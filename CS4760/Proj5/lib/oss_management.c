#include "oss_management.h"
#include "common_management.h"

/**************************************************************************/

// HashTable Functions

/* Pid -> Child Index Map */

void add_pid(entry** map, int pid, int childInd) {
  entry *e;

  HASH_FIND_INT( *map, &pid, e);  /* pid already in the hash? */
  if (e==NULL) {
    e = (entry*)malloc(sizeof(entry));
    e->id = pid;
    e->procInd = childInd;
    HASH_ADD_INT( *map, id, e );  /* id: name of key field */
  }
  else {
    fprintf(stderr, "HashTable error...exiting.\n");
    exit(1);
  }
}

//Get the entry with pid argument as key.
entry* find_pid(entry** map, int pid) {
  entry *e;

  HASH_FIND_INT( *map, &pid, e);
  return e;
}

//Delete a PID - Index relation.
void delete_pid( entry** map, entry *e) {
  HASH_DEL( *map, e);
  free(e);
}

//Delete all the hash table entries.
void delete_all(entry** map) {
  entry *current_entry, *tmp;

  HASH_ITER(hh, *map, current_entry, tmp) {
    //fprintf(stderr, "HASH_ITER running\n"); //This should print maxChildCount times upon exiting.
    HASH_DEL(*map,current_entry);
    free(current_entry);
  }
}

void print_pids(entry** map) {
  entry *e;

  for(e=*map; e != NULL; e=e->hh.next) {
    printf("pid %d: child id %d\n", e->id, e->procInd);
  }
}

/* Child Index -> Pid Map */

void add_child_id(child_id_entry** map, int child_id, int pid) {
  child_id_entry *e;

  HASH_FIND_INT( *map, &child_id, e);  /* child id already in the hash? */
  if (e==NULL) {
    e = (child_id_entry*)malloc(sizeof(child_id_entry));
    e->id = child_id;
    e->pid = pid;
    HASH_ADD_INT( *map, id, e );  /* id: name of key field */
  }
  else {
    fprintf(stderr, "HashTable error...exiting.\n");
    exit(1);
  }
}

child_id_entry* find_child_id(child_id_entry** map, int child_id) {
  child_id_entry *e;

  HASH_FIND_INT( *map, &child_id, e);
  return e;
}

void delete_child_id(child_id_entry** map, child_id_entry *e) {
  HASH_DEL( *map, e);
  free(e);
}

void delete_all_child_ids(child_id_entry** map) {
  child_id_entry *current_entry, *tmp;

  HASH_ITER(hh, *map, current_entry, tmp) {
    //fprintf(stderr, "HASH_ITER running\n"); //This should print maxChildCount times upon exiting.
    HASH_DEL(*map,current_entry);
    free(current_entry);
  }
}

void print_child_ids(child_id_entry** map) {
  child_id_entry *e;

  for(e=*map; e != NULL; e=e->hh.next) {
    printf("child id %d: pid %d\n", e->id, e->pid);
  }
}

/**************************************************************************/

//Time Management
void set_next_gen_time(FILE* rand_file, uint32_t* current_time, uint32_t* next_gen_time_holder) {
  uint32_t rand_nano = get_rand(rand_file, 1, proc_spawn_interval);
  set_next_time(current_time, next_gen_time_holder, rand_nano);
}

void increment_time(uint32_t* current_time) {
  current_time[1] += time_inc_interval;

  if (current_time[1] >= billion) {
    current_time[0] += 1;
    current_time[1] = current_time[1] % billion;
  }
}

/**************************************************************************/

//Resource Control Block Initalization

void display_all_blocks(FILE* log_file, int* logged_lines, resource_control* blocks) {
  for (int i = 0; i < max_resource_types; i++) {
    fprintf(log_file, "************************************\n");
    fprintf(log_file, "Res_Id: %d ", blocks[i].resource_class_ind);
    fprintf(log_file, "Num Instances: %d ", blocks[i].num_instances);
    fprintf(log_file, "Shareable: %d \n", blocks[i].is_shareable);
    //fprintf(log_file, "Alloc List Address: %p\n", blocks[i].pid_allocations);
    //fprintf(log_file, "Request List Address: %p\n", blocks[i].pid_requests);
    *logged_lines += 2;
  }
  fprintf(log_file, "************************************\n");
  *logged_lines += 1;
}

void init_all_res(resource_control* blocks, FILE* rand_file) {
  for (int i = 0; i < max_resource_types; i++) {
    blocks[i].resource_class_ind = i;
    blocks[i].num_instances = get_rand(rand_file, min_instances, max_instances);
    //blocks[i].num_instances = 2;
    blocks[i].is_shareable = 0;
  }
}

void init_shareable_res(resource_control* blocks, FILE* rand_file) {
  int num_shareable = get_rand(rand_file, min_shareable, max_shareable);
  for (int i = 0; i < num_shareable; i++) {
    blocks[i].is_shareable = 1;
    blocks[i].num_instances = 1;
  }
}

void init_block_lists(resource_control* blocks, List** alloc_list, List** request_list) {
  for (int i = 0; i < max_resource_types; i++) {
    alloc_list[i] = new_list();
    request_list[i] = new_list();
    blocks[i].pid_allocations = alloc_list[i];
    blocks[i].pid_requests = request_list[i];
  }
}

void free_block_lists(resource_control* blocks) {
  for (int i = 0; i < max_resource_types; i++) {
    if (blocks[i].pid_allocations) {
      destroy(blocks[i].pid_allocations);
      blocks[i].pid_allocations = NULL;
    }
    if (blocks[i].pid_requests) {
      destroy(blocks[i].pid_requests);
      blocks[i].pid_requests = NULL;
    }
  }
}

/**************************************************************************/

//Request Processing

/* Returns if 1 if request can be allocated, 0 if not. Shareable resources always return 1 */
int is_res_free(resource_control* blocks, uint32_t res_id) {
  if (blocks[res_id].is_shareable == 1)
    return 1;

  uint32_t current_size = size(blocks[res_id].pid_allocations);

  if ( current_size >= blocks[res_id].num_instances ) {
    if ( current_size > blocks[res_id].num_instances )
      fprintf(stderr, "CHECK IS_RES_FREE...ERROR!!!\n");

    return 0;
  }

  return 1;
}

void grant_res(resource_control* blocks, uint32_t res_id, pid_t pid) {
  add(pid, blocks[res_id].pid_allocations);
}

void block_res(resource_control* blocks, uint32_t res_id, pid_t pid) {
  add(pid, blocks[res_id].pid_requests);
}

int free_res(resource_control* blocks, uint32_t res_id, pid_t pid) {
  return delete(pid, blocks[res_id].pid_allocations);
}

//Needs optimization + Testing
void clear_all_pid(FILE* log_file, int* logged_lines, int verbosity, uint32_t* current_time, resource_control* blocks, pid_t pid) {
  int freed_allocs = 0;
  int freed_reqs = 0;
  char log_buf[256];

  if (verbosity) {
    fprintf(log_file, "%34s", "FREED: \n");
    *logged_lines += 1;
  }

  for (int i = 0; i < max_resource_types; i++) {
    freed_allocs = del_all_of_PID(pid, blocks[i].pid_allocations);
    freed_reqs = del_all_of_PID(pid, blocks[i].pid_requests);

    if (freed_allocs || freed_reqs) {
      if (verbosity) {
        snprintf(log_buf, 256, "%33s%d] - ", "* [R", i);
        fprintf(log_file, "%s", log_buf);
      }

      if (freed_allocs) {
        if (verbosity) {
          snprintf(log_buf, 256, "%d instances ", freed_allocs);
          fprintf(log_file, "%s", log_buf);
        }

        freed_allocs = 0;
      }
      if (freed_reqs) {
        if (verbosity) {
          snprintf(log_buf, 256, "%d requests ", freed_reqs);
          fprintf(log_file, "%s", log_buf);
        }

        freed_reqs = 0;
      }
      if (verbosity) {
        fprintf(log_file, "\n");
        *logged_lines += 1;
      }
    }
  }
}

/* Loop through all resources, if not shareable, has free instances, and has requests, grant them */
//Needs Testing
void grant_freed_res(FILE* log_file, int* logged_lines, int verbosity, int deadlock, uint32_t* current_time, Grant_Stack* granted_holder, resource_control* blocks) {
  int free_instances = 0;
  pid_t temp;
  char log_buf[256];

  for (int i = 0; i < max_resource_types; i++) {
    if (blocks[i].is_shareable)
      continue;

    //Check if instances are now free
    free_instances = blocks[i].num_instances - size(blocks[i].pid_allocations);
    //fprintf(stderr, "FREE INSTANCES %d: %d\n", i, free_instances);
    if (free_instances < 0) {
      fprintf(stderr, "grant_freed_res() detected negative free instances for R%d Value: %d...\n", i, free_instances);
    }

    while ( (free_instances > 0) && (size(blocks[i].pid_requests) > 0) ) {
      //Get the first PID from the request matrix
      temp = del_first(blocks[i].pid_requests);
      add(temp, blocks[i].pid_allocations);
      Grant_Stack_Push(granted_holder, temp, i);

      free_instances -= 1;
      //Log Allocations

      if (verbosity == 1) {
        if (deadlock == 1) {
          fprintf(log_file, "%26s%s%d%s%d\n", "", "OSS granting blocked request for R", i, " from PID ", temp);
          *logged_lines += 1;
        }
        else {
          snprintf(log_buf, 256, "OSS granting blocked request for R%d from PID %d", i, temp);
          log_msg(log_file, logged_lines, current_time, log_buf);
        }
      }
    }
  }
}

/**************************************************************************/

//Deadlock Detection Functionality

void clear_alloc_matrix(int32_t alloc_matrix_holder[max_child_procs][max_resource_types]) {
  for (int i = 0; i < max_child_procs; i++) {
    for (int j = 0; j < max_resource_types; j++) {
      alloc_matrix_holder[i][j] = 0;
    }
  }
}

void clear_avail_matrix(int32_t avail_matrix_holder[max_resource_types]) {
  for (int i = 0; i < max_resource_types; i++) {
    avail_matrix_holder[i] = 0;
  }
}

void clear_request_matrix(int32_t request_matrix_holder[max_child_procs][max_resource_types]) {
  for (int i = 0; i < max_child_procs; i++) {
    for (int j = 0; j < max_resource_types; j++) {
      request_matrix_holder[i][j] = 0;
    }
  }
}

void gen_avail_matrix(int32_t avail_matrix_holder[max_resource_types], resource_control* blocks) {
  //Clear the available matrix
  clear_avail_matrix(avail_matrix_holder);

  for (int i = 0; i < max_resource_types; i++) {
    if (blocks[i].is_shareable) {
      avail_matrix_holder[i] = 1000; //See readme for notes.
      continue;
    }

    avail_matrix_holder[i] = blocks[i].num_instances - size(blocks[i].pid_allocations);

    if (avail_matrix_holder[i] < 0) {
      fprintf(stderr, "gen_avail_matrix() resulted in a negative value.\n");
    }
  }
}

void gen_alloc_request_matrices(int32_t alloc_matrix_holder[max_child_procs][max_resource_types], int32_t request_matrix_holder[max_child_procs][max_resource_types], resource_control* blocks, entry** map) {
  //Clear both matrices
  clear_alloc_matrix(alloc_matrix_holder);
  clear_request_matrix(request_matrix_holder);

  pid_t temp_pid;
  entry* pid_entry;

  //Iterate through each resource block
  for (int i = 0; i < max_resource_types; i++) {

    //Do accounting for the allocated instances
    for (int j = 0; j < size(blocks[i].pid_allocations); j++) {
      //Get the PID the instance is allocated to
      temp_pid = get_pid_at(blocks[i].pid_allocations, j);

      //Find the child index of that pid
      pid_entry = find_pid(map, temp_pid);
      if (pid_entry == NULL) {
        fprintf(stderr, "gen_alloc_request_matrices() pid_allocations, pid -> childindex association NOT FOUND.\n");
        continue;
      }

      //Increment its allocation in the table (Remember the process indexes are 1 indexed)
      alloc_matrix_holder[pid_entry->procInd - 1][i] += 1;
    }

    //Do account for the requests
    for (int k = 0; k < size(blocks[i].pid_requests); k++) {
      //Get the PID of the request
      temp_pid = get_pid_at(blocks[i].pid_requests, k);

      //Find the child index of that PID
      pid_entry = find_pid(map, temp_pid);
      if (pid_entry == NULL) {
        fprintf(stderr, "gen_alloc_request_matrices() pid_requests, pid -> childindex association NOT FOUND.\n");
        continue;
      }

      //Increment its request in the table
      request_matrix_holder[pid_entry->procInd - 1][i] += 1;
    }


  }
}

int detect_deadlock(Stack* deadlocked_procs, int32_t alloc_matrix_holder[max_child_procs][max_resource_types], int32_t request_matrix_holder[max_child_procs][max_resource_types], int32_t avail_matrix_holder[max_resource_types]) {
  int work[max_resource_types];   // m resources
  int finish[max_child_procs];   // n processes
  int deadlocked = 0;

  //Set Initial Work matrix
  for (int i = 0; i < max_resource_types; i++) {
    work[i] = avail_matrix_holder[i];
  }
  //Set Initial Markers
  for (int i = 0; i < max_child_procs; i++) {
    finish[i] = 0;
  }

  for (int p = 0; p < max_child_procs; p++) {
      if ( finish[p] ) continue;

      if (req_lt_avail (request_matrix_holder[p], work)) {
         finish[p] = 1;

         for (int i = 0; i < max_resource_types; i++) {
            work[i] += alloc_matrix_holder[p][i];
         }

         p = -1;
      }
   }

   for (int p = 0; p < max_child_procs; p++) {
     if (finish[p] != 1) {
       deadlocked = 1;
       Stack_Push(deadlocked_procs, p);
     }
   }

   return deadlocked;
}

int req_lt_avail(int32_t proc_req[max_resource_types], int32_t current_avail[max_resource_types]) {
   for (int i = 0; i < max_resource_types; i++) {
      if (proc_req[i] > current_avail[i])
         return 0;
   }

   return 1;
}

/**************************************************************************/

//Logging

void log_msg(FILE* logfile, int* logged_lines, uint32_t* current_time, char msg[]) {
  fprintf(logfile, "(T-%9d:%9d) - %s\n", current_time[0], current_time[1], msg);
  *logged_lines += 1;
}

void print_lists(resource_control* blocks) {
  for (int i = 0; i < max_resource_types; i++) {
    fprintf(stderr, "R%d: Instances Allocated: %d Blocked: %d\n", i, size(blocks[i].pid_allocations), size(blocks[i].pid_requests));
  }
}

void log_alloc_matrix(FILE* logfile, int* logged_lines, uint32_t* current_time, int32_t alloc_matrix_holder[max_child_procs][max_resource_types]) {
  fprintf(logfile, "(T-%9d:%9d) - 20 requests granted. Logging current allocation table: \n", current_time[0], current_time[1]);
  *logged_lines += 1;

  char temp_buf[4];

  for (int i = -1; i < max_child_procs; i++) {
    //Print the resource row
    if (i == -1) {
      for (int j = 0; j < max_resource_types; j++) {
        snprintf(temp_buf, 4, "R%d", j);
        fprintf(logfile, "\t%s", temp_buf);
      }
      fprintf(logfile, "\n");
      *logged_lines += 1;
      continue;
    }

    //Real rows
    for (int j = -1; j < max_resource_types; j++) {
      if (j == -1) {
        snprintf(temp_buf, 4, "P%d", i + 1);
        fprintf(logfile, "%s", temp_buf);
        continue;
      }
      fprintf(logfile, "\t%d", alloc_matrix_holder[i][j]);
    }

    fprintf(logfile, "\n");
    *logged_lines += 1;
  }
}

int log_deadlocked_procs(FILE* logfile, int* logged_lines, Stack* deadlocked_child_ids) {
  int return_ind; //Holds the bottom deadlocked child_id for OSS to kill

  fprintf(logfile, "%25s %s", "", "Deadlocked Processes:");
  int deadlocked_ind;

  while ( (deadlocked_ind = Stack_Top(deadlocked_child_ids)) != -1 ) {
    fprintf(logfile, " P%d", deadlocked_ind + 1); //Add 1 for the offests
    return_ind = deadlocked_ind + 1;
    Stack_Pop(deadlocked_child_ids);
  }
  fprintf(logfile, "\n");
  *logged_lines += 1;

  return return_ind;
}

void log_free_deadlock(FILE* logfile, int* logged_lines, int child_ind, int pid) {
  fprintf(logfile, "%26s", "");
  fprintf(logfile, "%s%d%s%d%s", "Attempting to resolve deadlock by killing P", child_ind, " (PID ", pid, ")\n");
  *logged_lines += 1;
}
