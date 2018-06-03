Table of Contents
=================

* [To-Do's](#to-dos)


* [Usage](#usage)
*  [Implementation Notes:](#implementation-notes)
  * [Spec Deviations](#spec-deviations)


**Project Specifications:** [Here](Proj5_Specs.md)

## To-Do's
- Cleanup Dependencies & Makefile (Outstanding)

## Usage
1. Run ```Make``` at the root of the project directory
2. Navigate to /bin of the project directory
3. Run ```./oss -l ../out/log``` or ```./oss -l ../out/log -v```
4. Run ```Make clear``` to clean out the /out directory

## Implementation Notes:

- Time is incremented throughout the OSS' main proccess loop, after any major action taken. While at the conclusion of an iteration, a full hundred million nanoseconds is added on to the clock.

**IPC**
- Two message queues are used. Q1 is used to receive messages from processes. Q2 is used for processes to receive messages from OSS. Messages to separate processes are differentiated by the mtype stored within the message sent. This mtype value ranges from [1, max_child_procs]. This is 1-indexed due to mtype constraints.

**Resources**
- No actual resources are created, only resource blocks are. The resource block for a "resource" contains information regarding whether or not it is shareable, the max number of instances assignable, and pointers to two lists: one containing the PIDs requesting for an instance and one containing PIDs currently holding a resource (1 to 1).

**OSS**
- OSS' main loop first determines if a process can be spawned (if it can, it will associate the spawned PID with a child index). If a child can be spawned, it associates the spawned PID with the next free child index assignable (from the stack) and the child id with the PID using the two hash uthashmaps.
- After the spawning segment, OSS will consume all messages in Q1, requesting, releasing, and terminating processes as needed.
- Every 10 messages (#deadlock_check_interval in oss_management.h), the deadlock algorithm will run. If one is detected, it will kill processes off one by one, reassigning resources after each kill, and rerun the detection until the deadlock ceases.

**Processes**
- After process is spawned, every 150ms (action_check_interval macro set in proc_management.h) it determines if it needs to request, release (only if currently holding) a resource. Every 250ms (term_check_interval), it determines whether or not to terminate.
- Each process has a linked_list of resources it is currently holding.

### Spec Deviations
- Regardless of whether or not the verbosity flag is set, the initial resource block information will be logged.
