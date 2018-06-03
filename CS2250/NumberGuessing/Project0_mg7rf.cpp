#include <iostream>
#include <cmath>
using namespace std;

//Global constants and Global Variables
const int SIZE = 5;
const int SOLUTIONS[SIZE] = {4, 245, 496, 747, 998};
int guess;
int gDistance[SIZE];
int smallestDistance = 0;
int smallestDistanceNext = 0;
bool boolValue = false;

//All function prototypes
bool guessCheck(const int [], int);
int distanceToClosest(const int [], const int, int);
void warmerColder(int, int);

int main()
{
	cout << "Hello, please try and guess one of my numbers between 1 and 1000.\n";
	cout << "Please enter your guess: ";
	cin >> guess;
	while (guess < 1 || guess > 1000)
	{
		cout << "Invalid guess. Try again.";
		cin >> guess;
	}

	if (guess != SOLUTIONS[0] && guess != SOLUTIONS[1] && guess != SOLUTIONS[2] && guess != SOLUTIONS[3] && guess != SOLUTIONS[4])
		cout << "Incorrect guess. Try again.\n";

	guessCheck(SOLUTIONS, guess);
	distanceToClosest(SOLUTIONS, SIZE, guess);

	while (boolValue == false)
	{
		smallestDistance = smallestDistanceNext;
		cout << "Please enter your guess: \n";
		cin >> guess;

		while (guess < 1 || guess > 1000)
		{
			cout << "Invalid guess. Try again.";
			cin >> guess;
		}

		guessCheck(SOLUTIONS, guess);
		if (boolValue == true)
			break;

		distanceToClosest(SOLUTIONS, SIZE, guess);
		warmerColder(smallestDistance, smallestDistanceNext);
	}
	cout << "Congratulations, you have guessed one of my numbers!";

	return 0;
}

//Function for checking the guess with list of solutions
bool guessCheck(const int nums[], int guess)
{
	if (guess == nums[0] || guess == nums[1] || guess == nums[2] || guess == nums[3] || guess == nums[4])
		boolValue = true;
	else
		boolValue = false;

	return boolValue;
}

//Function for finding distance between guess and solutions
int distanceToClosest(const int nums[], const int SIZE, int guess)
{
	smallestDistanceNext = 1000;
	for (int count = 0; count < SIZE; count++)
	{
		gDistance[count] = abs(nums[count] - guess);

		if (gDistance[count] < smallestDistanceNext)
				smallestDistanceNext = gDistance[count];
	}

	return smallestDistanceNext;
}

//Function for determining whether the user is getting "warmer" or "colder"
void warmerColder(int distance1, int distance2)
{
	if (distance2 < distance1)
		cout << "Getting warmer.\n";
	else if (distance2 > distance1)
		cout << "Getting colder.\n";
	else
		cout << "No change...";
}

