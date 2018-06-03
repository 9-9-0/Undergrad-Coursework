**Deviations**
-

**Memory Structure Simulation**
- Main memory is represented by a 256 x 2 2D Array, 256 being the max # of 1K frames for this simulation. Each frame contains the index of a page and the index of the page table in which it belongs.
- Page tables are a 18 x 32 2D Array, 18 being the max # of currently running processes and 32 the max "size" of a process (32K).

**Request Pipeline**
- A circular queue of max size 18 is used to store incoming memory references from procs. A single semaphore, shared between 18 processes (maximum
running at any given time) is used to coordinate the writing of these requests to the circular queue.
- OSS checks for whether or not the requests are deadlocked (all 18 elements of the queue are filled). If yes, fast forward and process the head request. Otherwise, it simply processes the head request.
- When OSS successfully fulfills a request, it places a message in a message queue shared between it and all of the running processes. The message's
mtype is the child index of the process' request it has just fulfilled. Due to mtype requiring a non-zero value, OSS will transmit a message with mtype of (child index + 1) and processes will be waiting on messages of mtype (child index + 1)

**Metrics Calculated (+ Deviations)**
- Number of memory accesses per second
- Number of page faults per memory access
- Average memory access speed (this is interpreted as the time it takes to conduct a r/w operation + page fault time (if applicable))
- Throughput (this is interpreted as total # of memory accesses fulfilled during the duration of the run)

**Outstanding To-Do's**
- Actually use the page table in OSS...
- Refactor the switch statement for request action types in OSS
