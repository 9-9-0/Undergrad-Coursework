Table of Contents
=================

   * [Makefile Usage](#makefile-usage)

   * [Usage](#usage)

   * [A few notes on this implementation:](#a-few-notes-on-this-implementation)
         * [General Issues](#general-issues)

   * [To - Do's](#to---dos)


**Project Specifications:** [Here](Proj4_Specs.md)

# Makefile Usage

Run the following to see the scheduler actions in isolation, limited to only 19 processes.

`make` - Runs the program with all 3 scheduler actions enabled until timeout is reached.

`make oss0` - Processes will enter, be scheduled, and terminate on dispatch.

`make oss1` - Processes will enter, be scheduled, and run for an entire time quantum.

`make oss2` - Processes will enter, be scheduled, the first will be blocked the entire time.

# Usage
`./oss -a 1 -b 1 -l ../out/foo -t 2`
* -a is the threshold multiplier (a * average wait time of all processes in q1)
* -b is the threshold multiplier (b * average wait time of all processes in q2)
* -l is the location of the log output
* -t is the number of real seconds to run this program (recommended 2 or less)

# A few notes on this implementation:

### General Issues

* In general, there are a lot of issues with proper metric accounting due to truncation by integer division. In retrospect, this
  should be addressed with the use of floats for proper calculation and then a conversion to u_ints.

* The calculated average wait times and average turnaround time are based on processes that have completed at termination time.
  Scheduled processes are not counted.

* Since the simulation of process arrival is simply the scheduler (OSS) forking processes,
  there is no way for processes to accumulate during the run of a particularly long quantum.
  A proposed solution to this is to separate process generation from the scheduler, to have it
  run as a separate program and coordinate between it, the scheduler, and child processes accordingly.

* Deviations from the spec:
  * Mainly a few deviations with regards to the time slices. For simplicity sake, q is set to a base of
    1 second. Therefore, q/2 is 500 million nanoseconds, q/4 250 million nanoseconds. When the user process runs a partial time slice, instead of dividing the seconds and nanoseconds into potentially 1 to 99% of the original slice, the original slice is divided into fourths. This maintains clean divisions of time slices and arguably it offers the same amount of precision using the 1 to 99% range set forth in the original, taking into account of integer truncation and large nanosecond increment ranges (not really so much this but we can pretend).
  * While the user process runs, it's supposed to generate a random number to determine if it terminates. I've omitted this
    for simplicity sake.


* When a process is dispatched the following occurs (possible race condition noted):
  1. Scheduler changes the process' status to 1 (running) in its pc_block.
  2. Scheduler increments semaphore corresponding to that process.
  3. Scheduler busy waits until the process modifies its flag to 0 (not running).
  4. Scheduler requests to decrement semaphore by -1.
  5. Process increments semaphore and waits for 0 value semaphore before then requesting to decrement.
  6. Scheduler decrements semaphore to initial 0 condition.

  * A race condition could occur if the ordering of the semaphore requests from the scheduler and process between steps 4-6 are not maintained. This would only happen on environments under heavy CPU load.

# To - Do's

* Add the 1 or 2 seconds of idle time if a process does not "enter" the scheduler within the first few clock increments.
