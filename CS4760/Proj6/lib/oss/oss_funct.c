#include "oss_funct.h"

/**************************************************************************/

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

/**************************************************************************/

void init_frames(int32 frames[MAX_FRAMES][2]) {
  for (int i = 0; i < MAX_FRAMES; i++) {
    frames[i][0] = -1;
    frames[i][1] = -1;
  }
}

void init_tables(uint page_tables[MAX_RUNNING_PROCS][MAX_PROC_PAGES]) {
    for (int i = 0; i < MAX_RUNNING_PROCS; i++) {
      for (int j = 0; j < MAX_PROC_PAGES; j++) {
        page_tables[i][j] = 0;
      }
    }
}

//Iterates through frames and checks if the page of the given child index is loaded in
int get_frame_index(int32 frames[MAX_FRAMES][2], int32 child_ind, int32 page) {
  for (int i = 0; i < MAX_FRAMES; i++) {
    if (frames[i][0] == child_ind && frames[i][1] == page) {
      return i;
    }
  }

  return -1;
}

int get_first_free_frame(int32 frames[MAX_FRAMES][2]) {
  for (int i = 0; i < MAX_FRAMES; i++) {
    if (frames[i][0] == -1 && frames[i][1] == -1) {
      return i;
    }
  }

  return -1;
}

int can_daemon_run(int32_t frames[MAX_FRAMES][2]) {
  int num_free = 0;

  for (int i = 0; i < MAX_FRAMES; i++) {
    if (frames[i][0] == -1) {
        num_free += 1;
    }
  }

  if (num_free < (MAX_FRAMES / 10))
    return 1;
  else
    return 0;
}

void sweep_frames(int32 frames[MAX_FRAMES][2], uint* LRU_bitmap, uint* dirty_bitmap, FILE* logfile, int verbose) {
  //Log Pre-Sweep
  if (verbose) {
    log_frames(logfile, frames, LRU_bitmap, dirty_bitmap, 0);
  }

  for (int i = 0; i < MAX_FRAMES; i++) {
    //Bit is set
    if (TestBit(LRU_bitmap, i) != 0){
      ClearBit(LRU_bitmap, i);
    }
    //Not set
    else {
      //Set the dirty bit for swapping out.
      SetBit(dirty_bitmap, i);
    }
  }

  //Log Post-Sweep
  if (verbose) {
    log_frames(logfile, frames, LRU_bitmap, dirty_bitmap, 1);
  }
}

void swap_out_frames(uint* current_time, int32 frames[MAX_FRAMES][2], uint* LRU_bitmap, uint* dirty_bitmap, FILE* logfile, int verbose) {
  for (int i = 0; i < MAX_FRAMES; i++) {
    //Swap-out is needed
    if (TestBit(dirty_bitmap, i)) {
      //Sanity Check
      if (frames[i][0] == -1) {
        fprintf(stderr, "Dirty bit set, but frame is already empty...\n");
      }

      //Mark frames as free
      frames[i][0] = -1;
      frames[i][1] = -1;

      //Clear dirty bit
      ClearBit(dirty_bitmap, i);
    }
  }

  //Increment time (see README deviations)
  increment_time(current_time, SWAP_TIME);

  //Log Post-Swap
  if (verbose) {
    log_frames(logfile, frames, LRU_bitmap, dirty_bitmap, 2);
  }
}

void mark_frame(uint* LRU_bitmap, uint* dirty_bitmap, int frame_index) {
  SetBit(LRU_bitmap, frame_index);
  ClearBit(dirty_bitmap, frame_index);
}

void free_proc_table(uint* page_table) {
    for (int i = 0; i < MAX_PROC_PAGES; i++) {
      page_table[i] = 0;
    }
}

void free_frames(int32 frames[MAX_FRAMES][2], int32 proc_ind) {
  for (int i = 0; i < MAX_FRAMES; i++) {
    if (frames[i][0] == proc_ind) {
      frames[i][0] = -1;
      frames[i][1] = -1;
    }
  }
}
/**************************************************************************/

void set_dev_req(int32 req[6], int32 pid, int32 child_ind, int32 page_ind, int32 op, uint* current_time) {
  uint end_time[2] = {0, 0};
  set_next_time(current_time, end_time, SWAP_TIME);

  req[0] = pid;
  req[1] = child_ind;
  req[2] = page_ind;
  req[3] = op;
  req[4] = end_time[0];
  req[5] = end_time[1];
}

/**************************************************************************/
//Time Management
void set_next_gen_time(FILE* rand_file, uint* current_time, uint* next_gen_time_holder) {
  uint rand_nano = get_rand(rand_file, 1, HALF_SEC);
  set_next_time(current_time, next_gen_time_holder, rand_nano);
}

void increment_time_read_op(uint* current_time, uint* access_time_timer) {
  increment_time(current_time, READ_TIME);
  increment_time(access_time_timer, READ_TIME);
}

void increment_time_write_op(uint* current_time, uint* access_time_timer) {
  increment_time(current_time, WRITE_TIME);
  increment_time(access_time_timer, WRITE_TIME);
}

/**************************************************************************/

void log_msg(FILE* logfile, uint* current_time, char msg[]) {
  fprintf(logfile, "(T-%9d:%9d) - %s\n", current_time[0], current_time[1], msg);
}

void log_frames(FILE* logfile, int32 frames[MAX_FRAMES][2], uint* LRU_bitmap, uint* dirty_bitmap, int stage) {
  fprintf(logfile, "\n");

  switch(stage) {
    case 0:
      fprintf(logfile, "Pre-sweep Vector Values: \n");
      break;
    case 1:
      fprintf(logfile, "Post-sweep Vector Values: \n");
      break;
    case 2:
      fprintf(logfile, "Post-swap Vector Values: \n");
      break;
    case 3:
      fprintf(logfile, "Vector Values (minute snapshot): \n");
  }

  //Log the dirty bitmap
  for (int i = 0; i < MAX_FRAMES; i++) {
    if (frames[i][0] == -1) {
      fprintf(logfile, ".");
    }
    else {
      if (TestBit(dirty_bitmap, i) == 0) {
        fprintf(logfile, "U");
      }
      else {
        fprintf(logfile, "D");
      }
    }
  }

  fprintf(logfile, "\n");

  //Log the LRU bitmap
  for (int i = 0; i < MAX_FRAMES; i++) {
    if (frames[i][0] == -1) {
      fprintf(logfile, ".");
    }
    else {
      if (TestBit(dirty_bitmap, i) == 0) {
        fprintf(logfile, "0");
      }
      else {
        fprintf(logfile, "1");
      }
    }
  }

  fprintf(logfile, "\n\n");
}
