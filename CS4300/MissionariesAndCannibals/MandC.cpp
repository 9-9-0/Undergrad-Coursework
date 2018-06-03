#include <iostream>
#include <vector>
#include "StateSpace.h"
using namespace std;

int main()
{
	StateSpace* instance = new StateSpace();

	instance->run();

	delete instance;

	return 0;
}


