CC=gcc
CFLAGS=-Wall -std=gnu99

EXECUTABLES=oss proc
INCLUDES_OSS=$(addprefix lib/,parse_opt.c parse_opt.h deque.c deque.h scheduler.c scheduler.h)
INCLUDES_PROC=$(addprefix lib/,parse_opt.c parse_opt.h proc_util.c proc_util.h)

.PHONY: clear clean caveats

all : $(EXECUTABLES) caveats

oss : src/oss.c bin/dummyfile $(INCLUDES_OSS)
	$(CC) $(CFLAGS) $< lib/parse_opt.c lib/deque.c lib/scheduler.c -o bin/$@

proc : src/proc.c bin/dummyfile $(INCLUDES_PROC)
	$(CC) $(CFLAGS) $< lib/parse_opt.c lib/proc_util.c -o bin/$@

clean :
	$(MAKE) -C bin -f Makefile clean

clear :
	rm out/*

caveats :
	./message.sh

oss0 : src/oss_0_only.c bin/dummyfile $(INCLUDES_OSS)
	$(CC) $(CFLAGS) $< lib/parse_opt.c lib/deque.c lib/scheduler.c -o bin/$@

oss1 : src/oss_1_only.c bin/dummyfile $(INCLUDES_OSS)
	$(CC) $(CFLAGS) $< lib/parse_opt.c lib/deque.c lib/scheduler.c -o bin/$@

oss2 : src/oss_1_only.c bin/dummyfile $(INCLUDES_OSS)
	$(CC) $(CFLAGS) $< lib/parse_opt.c lib/deque.c lib/scheduler.c -o bin/$@
