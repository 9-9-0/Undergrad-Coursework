#ifndef POPULATION_H_
#define POPULATION_H_

#include <ctime>
#include <iostream>
#include <stack>
#include <boost/dynamic_bitset.hpp>
#include <boost/random.hpp>

class Population
{
public:
	Population(int popSize, int strLen, int gen, boost::random::mt19937* eng);
	~Population();

	void scorePop();
	void printTest();

	//Get Functions
	int getGen() { return gen; }
	int getPopSize() { return p_popSize; }
	int getStrLen() { return p_strLen; }
	double getAvgFit() { return avg_fitScore; };
	boost::dynamic_bitset<>* getGenMem() { return currentGen; }
	boost::dynamic_bitset<>* getElites() { return elites; }

	//Set function used in child to population conversion
	void setGenMem(boost::dynamic_bitset<>* input) { currentGen = input; }

private:
	//Instance dependent variables
	int p_popSize;
	int p_strLen;
	int gen;

	//RNG Related
	boost::random::mt19937* p_eng;
	boost::random::uniform_int_distribution<> fiddyFiddyRoll;
	void setRandGen();

	//Fitness scores
	int max_fitScore;
	int min_fitScore;
	double avg_fitScore;

	//Arrays of bitstrings
	boost::dynamic_bitset<>* currentGen;
	boost::dynamic_bitset<>* elites;

};




#endif /* POPULATION_H_ */
