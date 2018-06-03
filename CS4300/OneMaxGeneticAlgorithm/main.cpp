#include <iostream>
#include "Population.h"
#include "GeneticAlgorithm.h"
using namespace std;


int main()
{
	boost::random::mt19937 eng(std::time(0));

	GeneticAlgorithm* test = new GeneticAlgorithm(&eng);
	test->run();

	return 0;
}
