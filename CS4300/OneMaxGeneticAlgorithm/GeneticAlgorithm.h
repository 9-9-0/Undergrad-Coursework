#ifndef GENETICALGORITHM_H_
#define GENETICALGORITHM_H_

#include <vector>
#include "Population.h"

class GeneticAlgorithm
{
public:
	GeneticAlgorithm(boost::random::mt19937* eng);
	~GeneticAlgorithm();

	void run();

	void printTest();

private:
	int ga_popSize;
	int ga_strLen;
	int ga_genCount; //Zero-indexed counter for spawning generations
	double mutationRate;
	double crossoverRate;
	std::vector<Population*> generations;

	//Random Number Intervals
	boost::random::mt19937* ga_eng; 						//Pointer to mersenne twister engine defined in main
	boost::random::uniform_int_distribution<> mutRoll; 	//Range to be used for
	boost::random::uniform_int_distribution<> tSelRoll;
	boost::random::uniform_int_distribution<> xOverRoll;
	boost::random::uniform_int_distribution<> parentRoll;

	void setRandGens();

	//GA Initialization
	void initializeValues();
	void initializeOG();

	void spawnGen();

	boost::dynamic_bitset<> tSelection(int gen);
	void genChildren(boost::dynamic_bitset<> p1, boost::dynamic_bitset<> p2);
	std::vector<boost::dynamic_bitset<> >* childContainer;

	void mutate(int gen); //works as of now

	bool isWeDone();
	int isGlobalSol(); //works as of now
	bool hasImproved();

};



#endif /* GENETICALGORITHM_H_ */
