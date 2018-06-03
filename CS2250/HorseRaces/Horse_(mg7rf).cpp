/******************************************************************************
 * Course       : CS 2250 E01 (Fall 2015)
 * Assignment   : Project 3
 * Summary      : Horse Race Header File
 * Last Modified: October 17, 2015
 ******************************************************************************/
#include "Horse_(mg7rf).h"
#include <string>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <iomanip>
using namespace std;

/******************************************************************************
* Name    : Horse (constructor)
* Purpose : Initializes values for member variables of an instance of Horse
* Inputs  : Horse name, Jockey name
* Outputs : Values initialized if successful
******************************************************************************/
Horse::Horse(string nameArg, string jockeyArg)
{
	srand(time(0));
	name = nameArg;
	jockey = jockeyArg;
	maxSpeed = rand() % 30 + 1;
	distanceTraveled = 0;
	racesWon = 0; //May need to delete this
}

/******************************************************************************
* Name    : runOneSecond
* Purpose : Adds maxSpeed to distanceTraveled
* Inputs  : None.
* Outputs : Mutates distanceTraveled if successful
******************************************************************************/
void Horse::runOneSecond()
{
	distanceTraveled += maxSpeed;
}

/******************************************************************************
* Name    : sendToGate
* Purpose : Resets distanceTraveled to 0
* Inputs  : None.
* Outputs : Sets distanceTraveled to 0 if successful.
******************************************************************************/
void Horse::sendToGate()
{
	distanceTraveled = 0;
}

/******************************************************************************
* Name    : displayHorse
* Purpose : ASCII display of each horse's distance. Prints out a number of
* 			bars (out of 20), based on the distance traveled to race distance.
* Inputs  : None.
* Outputs : Prints out the horse's progress if successful.
******************************************************************************/
void Horse::displayHorse(double raceDistance)
{

	double perBar = raceDistance / 20.0;
	double horseBars = (distanceTraveled) / perBar;

	if (horseBars < 19.50)
		horseBars = horseBars + .50;    //Preps for truncation
	else if (horseBars >= 20)
		horseBars = 20; 				//Ensures that 20 is the maximum # of bars

	int horseBarsCounter = horseBars;

	cout << "|";

	while (horseBarsCounter > 0)
	{
		cout << "-";
		horseBarsCounter--;
	}

	if (distanceTraveled >= raceDistance)
	{
		cout << "|> ";
	}
	else
		cout << ">" << setw(23 - horseBars) << "|  ";

	cout << name << ", ridden by " << jockey;
	cout << endl;
}

/******************************************************************************
* Name    : getHorseDistance
* Purpose : Returns the distance traveled as an int.
* Inputs  : None.
* Outputs : Returns the distance traveled as an int if successful.
******************************************************************************/
int Horse::getHorseDistance() const
{
	return distanceTraveled;
}

/******************************************************************************
* Name    : getHorseSpeed
* Purpose : Returns the horse's speed as an int
* Inputs  : None.
* Outputs : Returns the horse's speed as an int if successful
******************************************************************************/
int Horse::getHorseSpeed() const
{
	return maxSpeed;
}

/******************************************************************************
* Name    : getRacesWon
* Purpose : Prints out the races that a horse has won.
* Inputs  : None.
* Outputs : Prints out the races that a horse has won if successful.
******************************************************************************/
void Horse::getRacesWon()
{
	cout << name << " has won " << racesWon;
}

/******************************************************************************
* Name    : addVictory
* Purpose : Adds a win to the horse.
* Inputs  : None.
* Outputs : Mutates the races won of a horse if succesful.
******************************************************************************/
void Horse::addVictory()
{
	racesWon++;
}
