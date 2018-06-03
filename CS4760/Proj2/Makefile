CC=gcc
CFLAGS=-Wall -std=gnu99

EXECUTABLES=master palin
INCLUDES_MAS=$(addprefix lib/,checks.c checks.h stack.h uthash.h)
INCLDUES_PAL=$(addprefix lib/,palin_fun.c palin_fun.h)

.PHONY: clear clean

all : $(EXECUTABLES)

master : src/master.c bin/dummyfile $(INCLUDES_MAS)
	$(CC) $(CFLAGS) $< lib/checks.c -o bin/$@

palin : src/palin.c bin/dummyfile $(INCLUDES_PAL)
	$(CC) $(CFLAGS) $< lib/palin_fun.c  -o bin/$@

clean :
	$(MAKE) -C bin -f Makefile clean

clear :
	rm out/nopalin.out out/palin.out
