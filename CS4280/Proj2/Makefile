CC=g++
COMP=-c
CFLAGS=-Wall -std=c++11

DIRECTORIES=bin obj
EXECUTABLES=main
INCLUDES_SCANNER=$(addprefix src/lib/,scanner.hpp scanner.cpp token.hpp)
INCLUDES_PARSER=$(addprefix src/lib/,parser.hpp parser.cpp)
INCLUDES_MAIN=$(addprefix src/,main.cpp)

.PHONY: clean message

all : $(DIRECTORIES) $(EXECUTABLES) message

bin :
	mkdir $@

obj :
	mkdir $@

scanner.o : $(INCLUDES_SCANNER)
	$(CC) $(CFLAGS) $(COMP) src/lib/scanner.cpp -o obj/$@

parser.o : $(INCLUDES_PARSER)
	$(CC) $(CFLAGS) $(COMP) src/lib/parser.cpp -o obj/$@

main : $(INCLUDES_MAIN) scanner.o parser.o
	$(CC) $(CFLAGS) obj/scanner.o obj/parser.o src/main.cpp -o bin/frontEnd

clean :
	rm -f obj/*
	rm -f bin/*

message :
	./message.sh
