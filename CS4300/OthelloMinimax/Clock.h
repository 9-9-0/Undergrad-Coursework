#ifndef CLOCK_H_
#define CLOCK_H_

#include <ctime>

class Clock
{
private:
	std::clock_t start;
	double timeElapsed;
	double timeLimit;
public:
	Clock();
	Clock(double timeLimit);
	~Clock();

	bool timesUp();
	void resetTime();
	double getDuration();
};



#endif /* CLOCK_H_ */
