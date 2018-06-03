CC=gcc
CFLAGS=-Wall -std=gnu99

EXECUTABLES=proc oss

all : $(EXECUTABLES)

#proc_gen : proc_gen.c ../lib/parse_opt.c
#	$(CC) $(CFLAGS) proc_gen.c ../lib/parse_opt.c -o proc_gen

proc : proc.c ../lib/proc_util.c ../lib/parse_opt.c
	$(CC) $(CFLAGS) proc.c ../lib/parse_opt.c ../lib/proc_util.c -o proc

oss : oss.c ../lib/scheduler.c ../lib/deque.c ../lib/parse_opt.c
	$(CC) $(CFLAGS) oss.c ../lib/parse_opt.c ../lib/deque.c ../lib/scheduler.c -o master



#./master -a 1 -b 1 -q 2 -l ./foo -t 200
