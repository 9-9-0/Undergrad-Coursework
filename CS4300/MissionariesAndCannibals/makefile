CC = g++
DEBUG = -g
CFLAGS = -c
OBJS = StateSpace.o

MandC : $(OBJS)
	$(CC) $(OBJS) MandC.cpp -o MandC

StateSpace.o : StateSpace.h StateSpace.cpp
	$(CC) $(CFLAGS) StateSpace.cpp

clean :
	rm *.o MandC
