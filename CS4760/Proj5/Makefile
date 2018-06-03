CC=gcc
CFLAGS=-Wall -std=gnu99

EXECUTABLES=oss proc
INCLUDES_OSS=$(addprefix lib/,linked_list.c linked_list.h parse_opt.c parse_opt.h oss_management.c oss_management.h resource.c resource.h common_management.h common_management.c)
DATA_STRUCTS=$(addprefix lib/data_structs/,granted_stack.c granted_stack.h stack.c stack.h uthash.h)
INCLUDES_PROC=$(addprefix lib/,linked_list.c linked_list.h common_management.h common_management.c proc_management.h proc_management.c)

.PHONY: clear clean message

all : $(EXECUTABLES) message

oss : src/oss.c bin/keyfile $(INCLUDES_OSS) $(DATA_STRUCTS)
	$(CC) $(CFLAGS) $< lib/parse_opt.c lib/linked_list.c lib/resource.c lib/oss_management.c lib/common_management.c lib/data_structs/granted_stack.c lib/data_structs/stack.c -o bin/oss

proc : src/proc.c bin/keyfile $(INCLUDES_PROC)
	$(CC) $(CFLAGS) $< lib/linked_list.c lib/common_management.c lib/proc_management.c -o bin/$@

clean :
	$(MAKE) -C bin -f Makefile clean

clear :
	rm out/*

message :
	./message.sh
