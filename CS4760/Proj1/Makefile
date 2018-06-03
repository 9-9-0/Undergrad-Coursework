CC=gcc
CFLAGS=-Wall

#Flags for SO creation
#CFLAGS=-fPIC -Wall -Wextra -g
#LDFLAGS = -shared
#TARGET_LIB = $(LIB_D)/libchecks.so

EXECUTABLES=simplechain simplechain2 simplechain3 simplechain4 simplechain5 simplechain6
SHARED=checks.c

all : $(EXECUTABLES)

$(EXECUTABLES) : % : src/%.c src/$(SHARED)
	$(CC) $(CFLAGS) $? -o bin/$@

clean :
	$(MAKE) -C bin -f Makefile clean
