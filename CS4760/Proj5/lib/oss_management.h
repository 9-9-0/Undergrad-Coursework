#ifndef OSS_MANAGEMENT_H
#define OSS_MANAGEMENT_H

#include "resource.h"
#include "data_structs/uthash.h"
#include "data_structs/granted_stack.h"
#include "data_structs/stack.h"

#define hund_million 100000000
#define proc_spawn_interval 500000000
#define time_inc_interval 100000000 //100 million nanoseconds
#define deadlock_check_interval 10

#define max_resource_types 20
#define max_child_procs 18
#define max_instances 10
#define min_instances 1
#define max_shareable 5
#define min_shareable 3

/**************************************************************************/

//Hash Table Stuff
typedef struct {
  int id;             //Unique Key. Value of child PID
  int procInd;        //Assigned index associated with the child PID
  UT_hash_handle hh;
} entry;

void add_pid(entry** map, int pid, int childInd);

entry* find_pid(entry** map, int pid);

void delete_pid(entry** map, entry *pid);

void delete_all(entry** map);

void print_pids(entry** map);

typedef struct {
  int id;           //Unique Key. Child Process Index
  int pid;          //PID of the process holding the child process index.
  UT_hash_handle hh;
} child_id_entry;

void add_child_id(child_id_entry** map, int child_id, int pid);

child_id_entry* find_child_id(child_id_entry** map, int child_id);

void delete_child_id(child_id_entry** map, child_id_entry *pid);

void delete_all_child_ids(child_id_entry** map);

void print_child_ids(child_id_entry** map);

/**************************************************************************/

//Time Management
void set_next_gen_time(FILE* rand_file, uint32_t* current_time, uint32_t* next_gen_time_holder);

void increment_time(uint32_t* current_time);

/**************************************************************************/

//Resource Control Block Initalization
void display_all_blocks(FILE* log_file, int* logged_lines, resource_control* blocks);

void init_all_res(resource_control* blocks, FILE* rand_file);

void init_shareable_res(resource_control* blocks, FILE* rand_file);

void init_block_lists(resource_control* blocks, List** alloc_list, List** request_list);

void free_block_lists(resource_control* blocks);

/**************************************************************************/

//Request, Release, Termination Handling
int is_res_free(resource_control* blocks, uint32_t res_id);

void grant_res(resource_control* blocks, uint32_t res_id, pid_t pid);

void block_res(resource_control* blocks, uint32_t res_id, pid_t pid);

int free_res(resource_control* blocks, uint32_t res_id, pid_t pid);

void clear_all_pid(FILE* log_file, int* logged_lines, int verbosity, uint32_t* current_time, resource_control* blocks, pid_t pid);

void grant_freed_res(FILE* log_file, int* logged_lines, int verbosity, int deadlock, uint32_t* current_time, Grant_Stack* granted_holder, resource_control* blocks);

/**************************************************************************/

//Deadlock Detection Functionality

void clear_alloc_matrix(int32_t alloc_matrix_holder[max_child_procs][max_resource_types]);

void clear_avail_matrix(int32_t avail_matrix_holder[max_resource_types]);

void clear_request_matrix(int32_t request_matrix_holder[max_child_procs][max_resource_types]);

void gen_avail_matrix(int32_t avail_matrix_holder[max_resource_types], resource_control* blocks);

void gen_alloc_request_matrices(int32_t alloc_matrix_holder[max_child_procs][max_resource_types], int32_t request_matrix_holder[max_child_procs][max_resource_types], resource_control* blocks, entry** map);

int detect_deadlock(Stack* deadlocked_procs, int32_t alloc_matrix_holder[max_child_procs][max_resource_types], int32_t request_matrix_holder[max_child_procs][max_resource_types], int32_t avail_matrix_holder[max_resource_types]);

int req_lt_avail(int32_t proc_req[max_resource_types], int32_t current_avail[max_resource_types]);

/**************************************************************************/

//Logging
void log_msg(FILE* logfile, int* logged_lines, uint32_t* current_time, char msg[]);

void print_lists(resource_control* blocks);

void log_alloc_matrix(FILE* logfile, int* logged_lines, uint32_t* current_time, int32_t alloc_matrix_holder[max_child_procs][max_resource_types]);

int log_deadlocked_procs(FILE* logfile, int* logged_lines, Stack* deadlocked_child_ids);

void log_free_deadlock(FILE* logfile, int* logged_lines, int child_ind, int pid);

#endif
