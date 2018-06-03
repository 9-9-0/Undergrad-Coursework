/******************************************************************************
 * Course       : CS 2250 E01 (Fall 2015)
 * Assignment   : Project 3
 * Summary      : Horse Race Header File
 * Last Modified: October 17, 2015
 ******************************************************************************/
#ifndef HORSE__MG7RF__H_
#define HORSE__MG7RF__H_
#include <string>
using namespace std;

class Horse
{
private:
	string name;
	string jockey;
	int maxSpeed;
	int distanceTraveled;
	int racesWon;
public:
	Horse(string nameArg, string jockeyArg);
	void runOneSecond();
	void sendToGate();
	void displayHorse(double raceDistance);
	int getHorseDistance() const;
	int getHorseSpeed() const;
	void getRacesWon();
	void addVictory();
};

#endif /* HORSE__MG7RF__H_ */
