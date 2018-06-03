/******************************************************************************
 * Course       : CS 2250 E01 (Fall 2015)
 * Assignment   : Project 3
 * Summary      : Horse Race Header File
 * Last Modified: October 17, 2015
 ******************************************************************************/
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include "Horse_(mg7rf).h"
using namespace std;

//Notes: - Could've used max speed of each horse to determine winner for each race
/* To Include(?):
 * DELETE HEAP MEMORY!
 * Betting System
 * Random acquisitions of Performance Enhancers
 */

bool winnerSearch(int numHorses, int raceDistance, Horse* hoarse);

int main()
{
	int numHorses;
	double raceDistance;
	string horseName;
	string jockeyName;

	/********* BEGIN USER DATA GATHERING *********/

	cout << "Enter the number of horses in the race: ";
	cin >> numHorses;

	while (numHorses < 1)												//Input Validation
	{
		cout << "Invalid number of horses. Try Again: ";
		cin >> numHorses;
	}

	const int numHorsesSize = numHorses; 								//Programming Standards

	Horse** horsePtr = 0;
	horsePtr = new Horse*[numHorsesSize];								//Programming Standards

	cin.ignore();

	for (int initCounter = 0; initCounter < numHorses; initCounter++)   //Loop that initalizes instances of Horse
	{
		cout << "Enter the name of Horse #" << initCounter + 1 << ": ";
		getline(cin, horseName);

		cout << "Enter the name of Horse #" << initCounter + 1 << "'s jockey: ";
		getline(cin, jockeyName);

		horsePtr[initCounter] = new Horse(horseName, jockeyName);
	}

	cout << "Enter the distance of the race (100+): ";
	cin >> raceDistance;

	while (raceDistance < 100)
	{
		cout << "Invalid race distance. Enter a distance of at least 100: ";
		cin >> raceDistance;
	}
	cout << endl;

	/********* END USER DATA GATHERING *********/

	/********* 	   BEGIN THE RACE	   *********/
	bool winnerFound = false;
	bool playAgain = true;
	int racesRun = 0;			//Counter for total races run
	int numWinners = 0; 		//Counter for number of winners in each race

	char input1;
	cout << "Press 'Y' to begin the race. Press any other key to opt out: ";
	cin >> input1;

	if (toupper(input1) == 'Y')
	{
		cout << "Bang!\n";
	}
	else
	{
		char input2;
		cout << "Are you sure you want to opt out? Press 'Y' to opt out: ";
		cin >> input2;

		if (toupper(input2) == 'Y')
		{
			cout << "\nEnding program";
			return 0;
		}
		else
			cout << "Bang!\n";
	}

	while (playAgain == true && winnerFound == false)
	{
		while (winnerFound == false)
		{
			for (int raceCounter = 0; raceCounter < numHorses; raceCounter++)
			{
				horsePtr[raceCounter]->runOneSecond();
				horsePtr[raceCounter]->displayHorse(raceDistance);
			}

			for (int winnerCounter = 0; winnerCounter < numHorses; winnerCounter++)
			{
				if (horsePtr[winnerCounter]->getHorseDistance() >= raceDistance)
				{
					numWinners++;
				}
			}

			for (int searchWCounter = 0; searchWCounter < numHorses; searchWCounter++)
			{
				winnerFound = winnerSearch(numHorses, raceDistance, horsePtr[searchWCounter]);
				if (winnerFound == true)
				{
					racesRun++;
					break;
				}
			}

			if (winnerFound == false)
			{
				char input3;
				cout << "Ready for the next second of the race? Press 'Y' to continue or any other key to opt out: ";
				cin >> input3;

				if (toupper(input3) == 'Y')
					cout << endl;
				else
				{
					cout << "Press 'Y' when you're ready: ";
					cin >> input3;
				}
			}
		}

		/* TEST LINEcout << numWinners << endl;
		   TEST LINEfor (int a = 0; a < numHorses; a++)
		{
			cout << horsePtr[a]->getHorseSpeed();
		}
		*/

		cout << endl;

		if (numWinners > 1)																//If loop in case multiple winners are found
		{
			int* winnerPool = 0;
			int winnerPoolIndex = 0;
			winnerPool = new int[numWinners];
			int greatestSpeed = 0;

			for (int searchSCounter = 0; searchSCounter < numHorses; searchSCounter++)	//Loop finds the greatest speed amongst horses that have reached the gate
			{
				if (horsePtr[searchSCounter]->getHorseDistance() >= raceDistance)
				{
					if (horsePtr[searchSCounter]->getHorseSpeed() > greatestSpeed)
						greatestSpeed = horsePtr[searchSCounter]->getHorseSpeed();
				}
			}

			for (int poolCounter = 0; poolCounter < numHorses; poolCounter++)			//Loop that adds the horse(s) with the greatest speed to winner pool
			{
				if (horsePtr[poolCounter]->getHorseDistance() >= raceDistance)
				{
					if (horsePtr[poolCounter]->getHorseSpeed() == greatestSpeed)
					{
						winnerPool[winnerPoolIndex] = poolCounter;
						winnerPoolIndex++;
					}
				}
			}

			srand(time(0));
			int winningIndex = rand() % winnerPoolIndex;										//randomly chooses the winning index of winnerPool
			//TEST LINE: cout << winnerPool[winningIndex] << endl;

			horsePtr[(winnerPool[winningIndex])]->addVictory();							//Adds a victory to the horse with greatest speed
			delete [] winnerPool;														//Delete heap memory for winnerPool
		}
		else if (numWinners == 1)
		{
			for (int a = 0; a < numHorses; a++)
			{
				if (horsePtr[a]->getHorseDistance() >= raceDistance)
				{
					horsePtr[a]->addVictory();
				}
			}
		}
		else
		{
			cout << "Fatal Error.";
			return 0;
		}

		cout << " \n";
		for (int statsCounter = 0; statsCounter < numHorses; statsCounter++)
		{
			horsePtr[statsCounter]->getRacesWon();
			cout << "/" << racesRun << " races.\n";
		}

		char input4;
		cout << "Would you like to race again? 'Y' for yes, any other key to opt out: ";
		cin >> input4;

		if (toupper(input4) == 'Y')
		{
			playAgain = true;
			winnerFound = false;
			numWinners = 0;
			for (int z = 0; z < numHorses; z++)
			{
				horsePtr[z]->sendToGate();
			}
			cout << "\n";
		}
		else
			playAgain = false;
	}

	cout << " \n";

	cout << "The final race results are: \n";
	for (int finalCounter = 0; finalCounter < numHorses; finalCounter++)
	{
		horsePtr[finalCounter]->getRacesWon();
		cout << "/" << racesRun << " races.\n";
	}

	cout << "Thanks for playing. Ending Program...";

	delete [] horsePtr;
	return 0;
}


/******************************************************************************
* Name    : winnerSearch
* Purpose : Tests to see if horse has finished the race
* Inputs  : Number of horses, distance of race, and a pointer to an instance of horse.
* Outputs : Returns false if the horse hasn't completed the race, true if it has.
******************************************************************************/
bool winnerSearch(int numHorses, int raceDistance, Horse* hoarse)
//Call @ Line 81: Possible to pass by reference pointer...? Review making copies of structs
{
	bool result;
	int testDistance = hoarse->getHorseDistance();

	if (testDistance >= raceDistance)
		result = true;
	else
		result = false;
	return result;
}







