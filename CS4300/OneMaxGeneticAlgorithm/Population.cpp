#include "Population.h"
#include "FitnessCalc.h"

void Population::setRandGen()
{
	fiddyFiddyRoll = boost::random::uniform_int_distribution<>(0,1);
}

Population::Population(int popSize, int strLen, int gen, boost::random::mt19937* eng)
{
	//Set Random Intervals, Set pointer to RNG
	setRandGen();
	this->p_eng = eng;

	//Initialize Population Attributes (popSize & strLen will remain constant over populations, gen is variable)
	this->p_popSize = popSize;
	this->p_strLen = strLen;
	this->gen = gen;


	//Check for whether or not the population is the first generation
	if (gen != 0)
	{
		this->currentGen = NULL;
		max_fitScore = -1;
		min_fitScore = -1;
		avg_fitScore = -1;
	}
	else
	{
		//Dynamically allocate array of bitsets for the population
		this->currentGen = new boost::dynamic_bitset<>[popSize];

		//Randomly assign bitstrings
		for (int i = 0; i < popSize; i++)
		{
			currentGen[i].resize(strLen);
			for (int j = 0; j < strLen; j++)
			{
				currentGen[i][j] = fiddyFiddyRoll(*p_eng);
			}
		}

		//Score initial generation
		scorePop();
	}

}


Population::~Population()
{
	delete[] currentGen;
	delete[] elites;
}

void Population::scorePop()
{
	//Probably inefficient but OOP
	FitnessCalc* temp = new FitnessCalc();

	double* results = temp->calcFit(this);

	//Set results, note the downcast of max and min (.5 added to each of the doubles in calcFit())
	this->min_fitScore = results[0];
	this->max_fitScore = results[1];
	this->avg_fitScore = results[2];

	delete [] results;

	elites = temp->extractElites(this);

	delete temp;
}

void Population::printTest()
{
	/*
	std::cout << "\nPopulation Contents: \n";
	for (int i = 0; i < p_popSize; i++)
	{
		for (int j = 0; j < p_strLen; j++)
		{
			std::cout << currentGen[i][j];
		}
		std::cout << std::endl;
	}
	*/

	std::cout << "Avg: " << avg_fitScore << std::endl;
	std::cout << "Min: " << min_fitScore << std::endl;
	std::cout << "Max: " << max_fitScore << std::endl;

	std::cout << "Most Fit 1: ";
	boost::dynamic_bitset<> fit1 = elites[0];
	for (int j = 0; j < p_strLen; j++)
	{
		std::cout << fit1[j];
	}

	if (p_popSize % 2 == 0)
	{
		std::cout << std::endl << "Most Fit 2: ";
		boost::dynamic_bitset<> fit2 = elites[1];
		for (int j = 0; j < p_strLen; j++)
		{
			std::cout << fit2[j];
		}
	}

	std::cout << std::endl;

	return;
}

