CC = g++
DEBUG = -g
CFLAGS = -c
CVER = -std=c++11
OBJS = Board.o GameV2.o Clock.o Minimax.o

OthelloMinimax : $(OBJS)
	$(CC) $(OBJS) main.cpp -o OthelloMinimax

Board.o : Board.h Board.cpp
	$(CC) $(CFLAGS) Board.cpp

GameV2.o : GameV2.h GameV2.cpp
	$(CC) $(CFLAGS) GameV2.cpp

Clock.o : Clock.h Clock.cpp
	$(CC) $(CFLAGS) Clock.cpp

Minimax.o : Minimax.h Minimax.cpp
	$(CC) $(CFLAGS) Minimax.cpp

clean :
	rm *.o OthelloMinimax
