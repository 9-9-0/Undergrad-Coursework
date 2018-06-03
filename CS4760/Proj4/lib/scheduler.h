#ifndef scheduler_h__
#define scheduler_h__
#include "pc_block.h"
#include "deque.h"

#define max_child_procs 19      //which also corresponds to the max size of each of the deques
#define nano_range 500000000    //1000 is the real range. Any other values strictly for testing
#define proc_gen_range 3
#define billion 1000000000
#define halfbillion 500000000
#define quarterbillion 250000000
#define q1_thresh_secs 5
#define q2_thresh_secs 10

//Misc
extern unsigned int get_rand_nano(FILE* rand_file);
extern unsigned int get_rand_action(FILE* rand_file);
extern unsigned int get_event_rand_sec(FILE* rand_file);
extern unsigned int get_event_rand_nano(FILE* rand_file);

//Clock Utility
extern void clock_increment_time(unsigned int* input_time, unsigned int nanoseconds);
extern int clock_is_time_up(unsigned int* current, unsigned int* next);
extern void clock_set_next_gen_time(unsigned int* current_time, unsigned int* next_gen_time, FILE* rand_file);

//Bitmap Functions
extern int bm_check_free_proc(int bitmap);  //Returns 1 if a process can be generated, 0 if not.
extern int bm_get_free_ind(int bitmap);   //Returns the first free process index (strictly increasing 0 to 18)

//Process Control Block
extern void pc_update_wait_times(pc_block* pc_blocks, unsigned int* current_time);

extern void pc_update_wait_time_single(pc_block* pc_block, unsigned int* current_time);

extern void pc_init_empty_blocks(pc_block* all_blocks);

extern long pc_empty_single_block(pc_block* pc_block);

extern void pc_harvest_metrics(unsigned int* metric_holders, int index, pc_block* pc_blocks);

extern pc_block* pc_init_control_block(long pid, int index, pc_block* block_arr, int* bitmap, unsigned int* sys_clock);

//Scheduler Functions
extern void sch_set_next_disp(unsigned int* next_disp_time, unsigned int* current_time, FILE* rand_file);

extern void sch_set_last_idle_time(unsigned int* time_holder, unsigned int* current_time);

extern void sch_add_idle_time(unsigned int* last_idle, unsigned int* current_time, unsigned int* total_idle_holder);

extern void schedule_process(long pid, deque_type* top_queue, pc_block* block);

extern void reschedule_process(pc_block* block, deque_type* queue[3]);

extern void sch_update_proc_metrics(unsigned int* totals, unsigned int* temp);

extern unsigned int* sch_calc_avg_wait(unsigned int* avg_wait_holder, unsigned int* total_wait, unsigned int procs);

extern unsigned int* sch_calc_turnaround(unsigned int* avg_turnaround_holder, unsigned int* total_turnaround, unsigned int procs);

extern unsigned int* sch_calc_avg_cpu(unsigned int* avg_cpu_holder, unsigned int* total_cpu_use, unsigned int procs);

//Queue Update Functions
extern void q_update_avg_times(unsigned int avg_wait_times[3][2], pc_block* pc_blocks);

extern void q_update_queues(FILE* log, unsigned int* current_time, deque_type* queues[3], pc_block* pc_blocks, unsigned int avg_wait_times[3][2], unsigned int alpha, unsigned int beta);

//Dispatching Functions
extern long d_get_head_pid(deque_type* queues[3]);

extern int d_get_pid_ind(long pid, pc_block* pc_blocks);

extern long d_dispatch_pid(long pid, pc_block* pc_blocks);

extern int d_terminate_pid(long pid, pc_block* pc_blocks);

extern long d_assign_quantum(pc_block* pc_block, unsigned int quantum[3][2]);

extern long d_proc_run_full_q(long pid, pc_block* pc_blocks, unsigned int quantum[3][2]);

extern long d_clear_pid_info(int pid_ind, pc_block* pc_blocks, int* bitmap);

//Logging Functions
extern void log_generation(FILE* logfile, long pid, unsigned int seconds, unsigned int nanos);

extern void log_queue_move(FILE* logfile, long pid, int from_q, int to_q, unsigned int seconds, unsigned int nanos);

extern void log_select_proc(FILE* logfile, long pid, unsigned int seconds, unsigned int nanos);

extern void log_term_proc(FILE* logfile, long pid, unsigned int seconds, unsigned int nanos);

extern void log_run_proc(FILE* logfile, long pid, int queue, unsigned int quantum[2], unsigned int* current_time);

extern void log_proc_return(FILE* logfile, long pid, unsigned int assgn_quantum[2], unsigned int actual_quantum[2], unsigned int* current_time);

extern void log_reschedule(FILE* logfile, long pid, unsigned int queue, unsigned int* current_time);

extern void log_scheduler_metrics(FILE* logfile, unsigned int* avgwait, unsigned int* turnaround, unsigned int* avgcpu, unsigned int* idle);

extern void log_next_disp(FILE* logfile, unsigned int* next_disp, unsigned int* current_time, long pid);

extern void log_redispatch(FILE* logfile, unsigned int* current_time, long pid);
#endif
