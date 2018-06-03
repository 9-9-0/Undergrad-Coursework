typedef struct
{
  //Metrics
  unsigned int last_burst_use_time[2];
  unsigned int last_wait_time[2];     //Used exclusively for calculating wait times.
  unsigned int last_finished_time[2];

  //Actual stuff being used.
  unsigned int times_dispatched;
  unsigned int turnaround_time[2];
  unsigned int total_wait_time[2];
  unsigned int total_cpu_usage[2];
  unsigned int gen_time[2];

  //Variable Metrics (i.e. will change every dispatch)
  unsigned int assigned_burst_time[2];
  unsigned int actual_burst_time[2];

  //Meta Info
  int bm_ind; //Also used to determine which semaphore it connects to.
  int status; //-1 for empty. 0 for scheduled. 1 for dispatched/running. 2 for blocked(i.e. waiting for the event), 3 for terminated.
  long pid;
  int queue;  //Holds the current queue this process is in.
} pc_block;
