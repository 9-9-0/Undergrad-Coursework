CC = g++
DEBUG = -g
CFLAGS = -c
OBJS = GeneticAlgorithm.o Population.o FitnessCalc.o

main : $(OBJS)
	$(CC) $(OBJS) main.cpp -o main

GeneticAlgorithm.o: GeneticAlgorithm.h GeneticAlgorithm.cpp
	$(CC) $(CFLAGS) GeneticAlgorithm.cpp

Population.o: Population.h Population.cpp
	$(CC) $(CFLAGS) Population.cpp

FitnessCalc.o: FitnessCalc.h FitnessCalc.cpp
	$(CC) $(CFLAGS) FitnessCalc.cpp

clean :
	rm *.o main
