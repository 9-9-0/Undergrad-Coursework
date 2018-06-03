CC=g++
COMP=-c
CFLAGS=-Wall -std=c++11

DIRECTORIES=bin obj
EXECUTABLES=P0
INCLUDES_PROCINPUT=$(addprefix src/lib/,procInput.hpp procInput.cpp)
INCLUDES_INCRDIV=$(addprefix src/lib/,incrDiv.cpp incrDiv.hpp)
INCLUDES_MAIN=$(addprefix src/,main.cpp lib/timer.h)
INCLUDES_P0=$(addprefix obj/,main.o procInput.o incrDiv.o)

.PHONY: clear clean message

all : $(DIRECTORIES) $(EXECUTABLES) message

bin :
	mkdir $@

obj :
	mkdir $@

procInput.o : $(INCLUDES_PROCINPUT)
	$(CC) $(CFLAGS) $(COMP) src/lib/procInput.cpp -o obj/procInput.o

incrDiv.o : $(INCLUDES_INCRDIV)
	$(CC) $(CFLAGS) $(COMP) src/lib/incrDiv.cpp -o obj/incrDiv.o

main.o : $(INCLUDES_MAIN)
	$(CC) $(CFLAGS) $(COMP) src/main.cpp -o obj/main.o

P0 : procInput.o incrDiv.o main.o
	$(CC) $(CFLAGS) $(addprefix obj/,$^) -o bin/$@

clean :
	rm -f *.out
	rm -f obj/*
	$(MAKE) -C bin -f Makefile clean

clear :
	rm out/*

message :
	./message.sh

#g++ -std=c++11 -c lib/procInput.cpp -o obj/procInput.o
#g++ -std=c++11 -c main.cpp -o obj/main.o
#g++ obj/main.o obj/procInput.o
