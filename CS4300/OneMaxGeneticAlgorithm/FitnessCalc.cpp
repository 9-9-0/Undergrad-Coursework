#include "FitnessCalc.h"

FitnessCalc::FitnessCalc()
{
}

FitnessCalc::~FitnessCalc()
{
}

double* FitnessCalc::calcFit(Population* pop)
{
	double* output = new double[3];
	double min = pop->getStrLen() + 1;
	double max = -1.0;
	double avg = 0.0;

	int temp; //Used to store fitness of an individual in population
	for (int i = 0; i < pop->getPopSize(); i++)
	{
		temp = pop->getGenMem()[i].count();
		if (temp < min)
		{
			min = temp;
		}
		if (temp > max)
		{
			max = temp;
		}
		avg += temp;
	}
	//Calculate average fitness
	avg = avg / pop->getPopSize();

	//Add .5 to prevent loss of precision when going from double -> int of results
	min += .5;
	max += .5;

	//Set and return array
	output[0] = min;
	output[1] = max;
	output[2] = avg;

	return output;
}

//This functionality can be merged into calcFit()'s loops but keeping it separate for OOP readability.
boost::dynamic_bitset<>* FitnessCalc::extractElites(Population* pop)
{
	if (pop->getPopSize() % 2 == 0)
	{
		int max1 = 0;
		int max2 = 0;
		boost::dynamic_bitset<>* output = new boost::dynamic_bitset<>[2];//0 for most fit, 1 for 2nd most fit
		output[0] = boost::dynamic_bitset<>(0);
		output[1] = boost::dynamic_bitset<>(0);

		for (int i = 0; i < pop->getPopSize(); i++)
		{
			int temp = pop->getGenMem()[i].count();
			if (max1 < temp)
			{
				max2 = max1;
				max1 = temp;
				output[1] = output[0];
				output[0] = pop->getGenMem()[i];

			}
			else if (max2 < temp)
			{
				max2 = temp;
				output[1] = pop->getGenMem()[i];
			}
		}
		return output;
	}
	else
	{
		int max1 = 0;
		boost::dynamic_bitset<>* output = new boost::dynamic_bitset<>[1];
		output[0] = boost::dynamic_bitset<>(0);

		for (int i = 0; i < pop->getPopSize(); i++)
		{
			int temp = pop->getGenMem()[i].count();
			if (max1 < temp)
			{
				max1 = temp;
				output[0] = pop->getGenMem()[i];
			}
		}

		return output;
	}

}
