CC=g++
COMP=-c
CFLAGS=-Wall -std=c++11

DIRECTORIES=bin obj
EXECUTABLES=main
INCLUDES_SCANNER=$(addprefix src/lib/,scanner.hpp scanner.cpp token.hpp)
INCLUDES_MAIN=$(addprefix src/,main.cpp)

.PHONY: clean message

all : $(DIRECTORIES) $(EXECUTABLES) message

bin :
	mkdir $@

obj :
	mkdir $@

scanner.o : $(INCLUDES_SCANNER)
	$(CC) $(CFLAGS) $(COMP) src/lib/scanner.cpp -o obj/$@

main : $(INCLUDES_MAIN) scanner.o
	$(CC) $(CFLAGS) obj/scanner.o src/main.cpp -o bin/scanner

clean :
	rm -f obj/*
	rm -f bin/*

message :
	./message.sh
