#include "GeneticAlgorithm.h"
#include <iostream>

GeneticAlgorithm::GeneticAlgorithm(boost::random::mt19937* eng)
{
	ga_eng = eng;

	initializeValues();
	initializeOG();
	setRandGens();
	generations[0]->scorePop();
	generations[0]->printTest();
}

//Sets the random intervals for GA
void GeneticAlgorithm::setRandGens()
{
	mutRoll = boost::random::uniform_int_distribution<>(0, ga_strLen - 1);
	tSelRoll = boost::random::uniform_int_distribution<>(0, ga_popSize - 1);
	xOverRoll = boost::random::uniform_int_distribution<>(1, 100);
	parentRoll = boost::random::uniform_int_distribution<>(0, 1);
}

//Sets One Max string length, population size, crossover rate, and mutation rate
void GeneticAlgorithm::initializeValues()
{
	int temp;

	//Get BitString Length and Population Size
	std::cout << "Enter One Max string length: ";
	std::cin >> temp;
	while (!std::cin)
	{
		std::cout << "Input must be an integer, try again: ";
		std::cin.clear();
		std::cin.ignore();
		std::cin >> temp;
	}
	while (temp <= 0)
	{
		std::cout << "Input must be 1 or greater, try again: ";
		std::cin.clear();
		std::cin.ignore();
		std::cin >> temp;
	}
	this->ga_strLen = temp;

	std::cout << "Enter One Max population size: ";
	std::cin >> temp;
	while (!std::cin)
	{
		std::cout << "Input must be an integer, try again: ";
		std::cin.clear();
		std::cin.ignore();
		std::cin >> temp;
	}
	while (temp <= 1)
	{
		std::cout << "Input must be 2 or greater, try again: ";
		std::cin.clear();
		std::cin.ignore();
		std::cin >> temp;
	}
	this->ga_popSize = temp;

	//Set Mutation Rate
	mutationRate = 1.0 / (double)ga_strLen; //This variable probably won't be used practically

	//Set Crossover Rate (Might make this user-specified for experimentation later on, for now .6 as instructed)
	crossoverRate = 0.6;
}

//Create and store intial generation
void GeneticAlgorithm::initializeOG()
{
	this->ga_genCount = 0;
	Population* thisGen = new Population(ga_popSize, ga_strLen, ga_genCount, ga_eng);
	generations.push_back(thisGen);
}

GeneticAlgorithm::~GeneticAlgorithm()
{
	for (int i = 0; i < generations.size(); i++)
	{
		delete generations.at(i);
	}
}

void GeneticAlgorithm::run()
{
	while (!isWeDone())
	{
		ga_genCount++;

		std::cout << "Generation: " << ga_genCount << std::endl;

		spawnGen();


		/*
		//Check the contents of the child container and compare to the generation's contents
		std::cout << "\nChild Container contents: \n";
		for (int i = 0; i < childContainer->size(); i++)
		{
			for (int j = 0; j < ga_strLen; j++)
			{
				std::cout << childContainer->at(i)[j];
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		*/

		//Convert container of child vectors to array of dynamic bitsets
		boost::dynamic_bitset<>* Temp = new boost::dynamic_bitset<>[ga_popSize];
		for (int i = 0; i < ga_popSize; i++)
		{
			Temp[i] = childContainer->at(i);
		}

		delete childContainer;

		generations[ga_genCount]->setGenMem(Temp);

		/*
		std::cout << "\nConverted Child Container to Population Contents: \n";
		for (int i = 0; i < ga_popSize; i++)
		{
			for (int j = 0; j < ga_strLen; j++)
			{
				std::cout << generations[ga_genCount]->getGenMem()[i][j];
			}
			std::cout << std::endl;
		}
		*/

		mutate(ga_genCount);


		generations[ga_genCount]->scorePop();
		generations[ga_genCount]->printTest();

	}
}

//Spawn a generation (Note that children are not set until vector to bitset array conversion)
void GeneticAlgorithm::spawnGen()
{
		Population* nextGen = new Population(ga_popSize, ga_strLen, ga_genCount, ga_eng);
		generations.push_back(nextGen);

		childContainer = new std::vector<boost::dynamic_bitset<> >(); //Create the container to hold children

		if (ga_popSize % 2 == 0)
		{
			int numReps = (ga_popSize - 2) / 2;
			boost::dynamic_bitset<> p1;
			boost::dynamic_bitset<> p2;

			for(int i = 0; i < numReps; i++)
			{
				//Choose the two parents based on tournament selection
				p1 = tSelection(ga_genCount - 1);
				p2 = tSelection(ga_genCount - 1);

				//Generate children
				genChildren(p1, p2);
			}

			childContainer->push_back(generations[ga_genCount - 1]->getElites()[0]);
			childContainer->push_back(generations[ga_genCount - 1]->getElites()[1]);
		}
		else
		{
			int numReps = (ga_popSize + 1) / 2;
			boost::dynamic_bitset<> p1;
			boost::dynamic_bitset<> p2;

			for (int i = 0; i < numReps; i++)
			{
				//Choose the two parents based on tournament selection
				p1 = tSelection(ga_genCount - 1);
				p2 = tSelection(ga_genCount - 1);

				//Generate children
				genChildren(p1, p2);
			}

			childContainer->push_back(generations[ga_genCount - 1]->getElites()[0]);
		}
}

//gen = index of most recent Generation (i.e. ga_genCount - 1)
//Perform tournament selection (returns 1 parent)
boost::dynamic_bitset<> GeneticAlgorithm::tSelection(int gen)
{

	int t1 = tSelRoll(*ga_eng);
	int t2 = tSelRoll(*ga_eng);

	/*
	std::cout << "\nt1: " << t1 << " t2: " << t2 << std::endl;
	*/

	//Choose potential candidates for Parent
	boost::dynamic_bitset<> cand1 = generations[gen]->getGenMem()[t1];
	boost::dynamic_bitset<> cand2 = generations[gen]->getGenMem()[t2];

	/*
	std::cout << "\nCand 1: ";
	for (int i = 0; i < cand1.size(); i++)
	{
		std::cout << cand1[i];
	}

	std::cout << "\nCand 2: ";
	for (int i = 0; i < cand2.size(); i++)
	{
		std::cout << cand2[i];
	}
	*/

	//Determine most fit parent
	if (cand1.count() == cand2.count())
	{
		if (parentRoll(*ga_eng) == 0)
		{
			/*
			std::cout << "\nReturning: ";
			for (int i = 0; i < cand1.size(); i++)
			{
				std::cout << cand1[i];
			}
			*/
			return cand1;
		}
		else
		{
			/*
			std::cout << "\nReturning: ";
			for (int i = 0; i < cand2.size(); i++)
			{
				std::cout << cand2[i];
			}
			*/
			return cand2;
		}
	}
	else if (cand1.count() > cand2.count())
	{
		/*
		std::cout << "\nReturning: ";
		for (int i = 0; i < cand1.size(); i++)
		{
			std::cout << cand1[i];
		}
		*/

		return cand1;
	}
	else //Note: Possibly add a clause for case in which both are equal
	{
		/*
		std::cout << "\nReturning: ";
		for (int i = 0; i < cand2.size(); i++)
		{
			std::cout << cand2[i];
		}
		*/
		return cand2;
	}

}

void GeneticAlgorithm::genChildren(boost::dynamic_bitset<> p1, boost::dynamic_bitset<> p2)
{
	/*
	std::cout << "\nP 1: ";
	for (int i = 0; i < p1.size(); i++)
	{
		std::cout << p1[i];
	}

	std::cout << "\nP 2: ";
	for (int i = 0; i < p2.size(); i++)
	{
		std::cout << p2[i];
	}

	std::cout << std::endl;
	*/

	int limit = crossoverRate * 100.0 + 0.5;

	if (xOverRoll(*ga_eng) <= limit)
	{
		//std::cout << "\nCrossover Invoked" << std::endl;

		boost::dynamic_bitset<> child1;
		boost::dynamic_bitset<> child2;

		child1.resize(ga_strLen);
		child1.set();
		child2.resize(ga_strLen);
		child2.set();

		for (int i = 0; i < ga_strLen; i++)
		{
			if (parentRoll(*ga_eng) == 0)
			{
				child1[i] = p1[i];
				child2[i] = p2[i];
			}
			else
			{
				child1[i] = p2[i];
				child2[i] = p1[i];
			}
		}

		/*
		std::cout << "Child 1: " << std::endl;

		for (int i = 0; i < child1.size(); i++)
		{
			std::cout << child1[i];
		}
		*/

		childContainer->push_back(child1);

		/*
		std::cout << "\nChild 2: " << std::endl;
		for (int i = 0; i < child2.size(); i++)
		{
		std::cout << child2[i];
		}

		std::cout << std::endl;
		*/

		childContainer->push_back(child2);
	}
	else
	{

		//std::cout << "\nNo Crossover Invoked\n";

		childContainer->push_back(p1);
		childContainer->push_back(p2);

		/*
		std::cout << "Child 1: " << std::endl;
		for (int i = 0; i < p1.size(); i++)
		{
			std::cout << p1[i];
		}


		std::cout << "\nChild 2: " << std::endl;
		for (int i = 0; i < p2.size(); i++)
		{
			std::cout << p2[i];
		}
		*/
	}


}

//Perform mutation on a single bit (1 / ga_strLen chance of occuring)
void GeneticAlgorithm::mutate(int gen)
{

	for (int i = 0; i < ga_popSize; i++)
	{
		for (int j = 0; j < ga_strLen; j++)
		{
			if (mutRoll(*ga_eng) == 0)
			{
				generations[gen]->getGenMem()[i][j] = (bool)~generations[gen]->getGenMem()[i][j];
			}
		}
	}
}

//Check for algorithm termination (Either global solution found, or no improvement over 3 generations)
bool GeneticAlgorithm::isWeDone()
{
	if (isGlobalSol() != -1)
	{
		std::cout << "Global Solution Found. Generation: " << ga_genCount << std::endl;
		//std::cout << ga_genCount << std::endl;
		return true;
	}
	if (ga_genCount < 4)
	{
		return false;
	}
	else
	{
		if (!hasImproved())
		{
			std::cout << "No improvement over 3 generations. Ending on generation: " << ga_genCount << std::endl;
			return true;
		}
		/*
		if (ga_genCount < 1000)
		{
			return false;
		}
		*/
	}
	return false;
}

//Check for global solution
int GeneticAlgorithm::isGlobalSol()
{
	for (int i = 0; i < ga_popSize; i++)
	{
		if (generations[ga_genCount]->getGenMem()[i].count() == ga_strLen)
		{
			return i;
		}
	}
	return -1;
}

//Check for average fitness improvement over three generations
bool GeneticAlgorithm::hasImproved()
{
	double currentFit = generations[ga_genCount]->getAvgFit();
	double prevFit = generations[ga_genCount - 1]->getAvgFit();
	double prevPrevFit = generations[ga_genCount - 2]->getAvgFit();

	if (currentFit <= prevFit && prevFit <= prevPrevFit)
	{
		return false;
	}

	return true;
}

void GeneticAlgorithm::printTest()
{
	for (int i = 0; i < generations.size(); i++)
	{
		std::cout << "Generation: " << generations[i]->getGen() << std::endl;
		generations[i]->printTest();
	}
}
