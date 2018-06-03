#ifndef OSS_FUNCT_H
#define OSS_FUNCT_H

#include <sys/wait.h>
#include "../common/common_funct.h"
#include "../common/stack.h"
#include "../common/uthash.h"

/**************************************************************************/

//Bit vector Stuff
#define SetBit(A,k)     ( A[(k/32)] |= (1 << (k%32)) )
#define ClearBit(A,k)   ( A[(k/32)] &= ~(1 << (k%32)) )
#define TestBit(A,k)    ( A[(k/32)] & (1 << (k%32)) )

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

/**************************************************************************/

//Clock Policy Algorithm + Frame/Page Table Stuff
void init_frames(int32 frames[MAX_FRAMES][2]);

void init_tables(uint page_tables[MAX_RUNNING_PROCS][MAX_PROC_PAGES]);

int get_frame_index(int32 frames[MAX_FRAMES][2], int32 child_ind, int32 page);

int get_first_free_frame(int32 frames[MAX_FRAMES][2]);

int can_daemon_run(int32 frames[MAX_FRAMES][2]);

void sweep_frames(int32 frames[MAX_FRAMES][2], uint* LRU_bitmap, uint* dirty_bitmap, FILE* logfile, int verbose);

void swap_out_frames(uint* current_time, int32 frames[MAX_FRAMES][2], uint* LRU_bitmap, uint* dirty_bitmap, FILE* logfile, int verbose);

void mark_frame(uint* LRU_bitmap, uint* dirty_bitmap, int frame_index);

void free_proc_table(uint* page_table);

void free_frames(int32 frames[MAX_FRAMES][2], int32 proc_ind);

/**************************************************************************/

//OSS Time Management
void set_next_gen_time(FILE* rand_file, uint* current_time, uint* next_gen_time_holder);

void increment_time_read_op(uint* current_time, uint* access_time_timer);

void increment_time_write_op(uint* current_time, uint* access_time_timer);

/**************************************************************************/
//Request Queue + IPC Related

void set_dev_req(int32 req[6], int32 pid, int32 child_ind, int32 page_ind, int32 op, uint* current_time);

/**************************************************************************/

//Logging
void log_msg(FILE* logfile, uint* current_time, char msg[]);

void log_frames(FILE* logfile, int32 frames[MAX_FRAMES][2], uint* LRU_bitmap, uint* dirty_bitmap, int stage);

#endif
