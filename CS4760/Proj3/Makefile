CC=gcc
CFLAGS=-Wall -std=gnu99

EXECUTABLES=oss isabella
VERBOSE_EXECUTABLES=oss isabella_verbose
INCLUDES_OSS=$(addprefix lib/,parse_opt.c parse_opt.h)
INCLUDES_ISA=$(addprefix lib/,parse_opt.h parse_opt.c)

.PHONY: clear clean

all : $(EXECUTABLES)

verbose : $(VERBOSE_EXECUTABLES)

oss : src/oss.c bin/dummyfile $(INCLUDES_OSS)
	$(CC) $(CFLAGS) $< lib/parse_opt.c -o bin/$@

isabella : src/isabella.c bin/dummyfile $(INCLUDES_ISA)
	$(CC) $(CFLAGS) $< lib/parse_opt.c  -o bin/$@

isabella_verbose : src/isabella_verbose.c bin/dummyfile $(INCLUDES_ISA)
	$(CC) $(CFLAGS) $< lib/parse_opt.c -o bin/isabella

clean :
	$(MAKE) -C bin -f Makefile clean

clear :
	rm out/*
