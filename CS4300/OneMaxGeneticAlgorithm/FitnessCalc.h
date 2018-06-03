#ifndef FITNESSCALC_H_
#define FITNESSCALC_H_

#include "Population.h"

class FitnessCalc
{
public:
	FitnessCalc();
	~FitnessCalc();

	//Calculate Fitness Scores
	double* calcFit(Population* pop); //gets deleted in Population::scorePop()

	//Returns dynamically-allocated array of elites
	boost::dynamic_bitset<>* extractElites(Population* pop); //gets deleted by Population::~Population
};



#endif /* FITNESSCALC_H_ */
