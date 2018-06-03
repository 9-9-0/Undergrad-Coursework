#include "Clock.h"

Clock::Clock(double timeLimit)
{
	this->timeLimit = timeLimit;
	this->timeElapsed = 0;
	start = std::clock();
}

Clock::~Clock()
{

}

bool Clock::timesUp()
{
	if (getDuration() > timeLimit)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Clock::resetTime()
{
	start = std::clock();
}

double Clock::getDuration()
{
	timeElapsed = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	return timeElapsed;
}


