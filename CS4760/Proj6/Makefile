CC=gcc
CFLAGS=-Wall -std=gnu99 -g

EXECUTABLES=oss proc
INCLUDES_COMMON=$(addprefix lib/common/,common_funct.c common_funct.h common_types.h common_macros.h)
INCLUDES_OSS=$(addprefix lib/oss/,oss_funct.c oss_funct.h parse_opt.c parse_opt.h)
INCLUDES_PROC=$(addprefix lib/proc/,proc_funct.c proc_funct.h)

.PHONY: clear clean message

all : $(EXECUTABLES) message

oss : src/oss.c bin/keyfile $(INCLUDES_OSS) $(INCLUDES_COMMON)
	$(CC) $(CFLAGS) $< lib/oss/parse_opt.c lib/common/common_funct.c lib/common/stack.c lib/oss/oss_funct.c lib/common/circ_queue.c -o src/$@

proc : src/proc.c bin/keyfile $(INCLUDES_PROC) $(INCLUDES_COMMON)
	$(CC) $(CFLAGS) $< lib/common/common_funct.c lib/proc/proc_funct.c lib/common/circ_queue.c -o src/$@

clean :
	$(MAKE) -C bin -f Makefile clean

clear :
	rm out/*

message :
	./message.sh
