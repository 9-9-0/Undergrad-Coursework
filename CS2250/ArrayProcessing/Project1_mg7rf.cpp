/******************************************************************************
 * Course       : CS 2250 E01 (Fall 2015)
 * Assignment   : Project 1
 * Last Modified: October 4, 2011
 ******************************************************************************/
#include <iostream>
#include <ctime>
#include <stdlib.h>
using namespace std;

//Function prototypes
void initializeArray(int [], int);
void printArray(const int [], int);
int* shuffleArray(const int [], int);
int* createOddArray(const int [], int, int &);
int* createEvenArray(const int [], int, int &);
void arrayWar(int*, int, int*, int);
void sortArray(int*, int);

/******************************************************************************
* Name    : main
* Purpose : This functions prompts user for array size, creates an array based
			off that, then uses those arrays in functions.
* Inputs  : None.
* Outputs : Returns 0 if ran successfully.
******************************************************************************/
int main()
{
	//*Begin Part 1*
	int userSize;
	cout << "To begin, enter an array size of your choice between 1 and 100: ";
	cin >> userSize;
	while (userSize < 1 || userSize > 100)
	{
		cout << "Invalid array size. Try again: ";
		cin >> userSize;
	}

	cout << "Your original array is: \n";
	int userArray[userSize];
	initializeArray(userArray, userSize);
	printArray(userArray, userSize);

	cout << "Your initial array after being shuffled is: \n";
	int* shuffledArray = 0;
	shuffledArray = shuffleArray(userArray, userSize);
	printArray(shuffledArray, userSize);
	//*End Part 1*

	//**Begin Part 2***
	cout << "The odd values of the shuffled array are: \n";
	int* oddArray = 0;
	int oddArraySize = 0;
	oddArray = createOddArray(shuffledArray, userSize, oddArraySize);
	printArray(oddArray, oddArraySize);

	cout << "The even values of the shuffled array are: \n";
	int* evenArray = 0;
	int evenArraySize = 0;
	evenArray = createEvenArray(shuffledArray, userSize, evenArraySize);
	printArray(evenArray, evenArraySize);
	//**End Part 2**

	//***Begin Part 3***
	cout << "ArrayWar Winners: \n";
	arrayWar(oddArray, oddArraySize, evenArray, evenArraySize);
	cout << " " << endl;

	sortArray(oddArray, oddArraySize);
	//***End Part 3***

	//****Part 4 is the entire main functon****

	//Deletes all heap memory
	delete [] shuffledArray;
	delete [] oddArray;
	delete [] evenArray;
	return 0;

}

/******************************************************************************
* Name    : initializeArray
* Purpose : This functions stores sequential integers into an array.
* Inputs  : userSize & userArray
* Outputs : Stores sequential integers into array if successful.
******************************************************************************/
void initializeArray(int arr[], int size)
{
	for (int index = 0; index < size; index++)
	{
		arr[index] = index;
	}
}

/******************************************************************************
* Name    : printArray
* Purpose : This functions prints the contents of an array.
* Inputs  : userSize & userArray
* Outputs : Prints all elements in an array.
******************************************************************************/
void printArray(const int arr[], int size)
{
	for (int index = 0; index < size; index++)
	{
		cout << arr[index] << " ";
	}
	cout << " " << endl;
}

/******************************************************************************
* Name    : shuffleArray
* Purpose : This functions creates a pointer to a dynamically allocated array,
			generates a random number, and using that random number it stores
			the original array's elements in a random order in the DA array.
* Inputs  : userSize & userArray
* Outputs : Array of same valued elements but in different order
******************************************************************************/
int* shuffleArray(const int arr[], int size)
{
	srand(time(0));
	int* newArr = 0;
	newArr = new int[size];
	for (int index = 0; index < size; index++)
	{
		newArr[index] = arr[index];
	}
	for (int counter = (size - 1); counter >= 1; counter--)
	{
		int random = rand() % counter + 1;
		int temp = newArr[counter];
		newArr[counter] = newArr[random];
		newArr[random] = temp;
	}
	return newArr;
}

/******************************************************************************
* Name    : createOddArray
* Purpose : Takes in an array, determines all odd numbers in that array, stores
* 			those numbers into a dynamically allocated array. Passes the size
* 			of the heap memory array by reference.
* Inputs  : Array, size of array, and a reference variable, newSize
* Outputs : Pointer to the odd array and changes the reference variable passed as
* 			newSize to the number of odd numbers.
******************************************************************************/
int* createOddArray(const int origArr[], int origSize, int &newSize)
{
	int numOdds = 0;
	for (int index = 0; index < origSize; index++)
	{
		if (origArr[index] % 2 > 0)
			numOdds++;
	}

	int* oddArray = 0;
	oddArray = new int[numOdds];

	int oddArrayCounter = 0;
	for (int counter = 0; counter < origSize; counter++)
	{
		if (origArr[counter] % 2 != 0 && oddArrayCounter < numOdds)
		{
			oddArray[oddArrayCounter] = origArr[counter];
			oddArrayCounter++;
		}
	}

	newSize = numOdds;
	return oddArray;

}

/******************************************************************************
* Name    : createEvenArray
* Purpose : Takes in an array, determines all even numbers in that array, stores
* 			those numbers into a dynamically allocated array. Passes the size
* 			of the heap memory array by reference.
* Inputs  : Array, size of array, and a reference variable, newSize
* Outputs : Pointer to the even array and changes the reference variable passed
* 			as newSize to the number of even numbers.
******************************************************************************/
int* createEvenArray(const int origArr[], int origSize, int &newSize)
{
	int numEvens = 0;
	for (int index = 0; index < origSize; index++)
	{
		if (origArr[index] % 2 == 0)
			numEvens++;
	}

	int* evenArray = 0;
	evenArray = new int[numEvens];

	int evenArrayCounter = 0;
	for (int counter = 0; counter < origSize; counter++)
	{
		if (origArr[counter] % 2 == 0 && evenArrayCounter < numEvens)
		{
			evenArray[evenArrayCounter] = origArr[counter];
			evenArrayCounter++;
		}
	}

	newSize = numEvens;
	return evenArray;

}

/******************************************************************************
* Name    : arrayWar
* Purpose : This function takes in two integer arrays and their sizes, then creates
* 			and allocates an array inside the function of the same size as the
* 			largest of these two arrays, then goes through both arrays starting at
* 			the lowest index. At each index, it compares their values and puts the
* 			highest of these values in the 3rd array that was created. If one array
* 			isn’t large enough, the other array always wins at those indices.
* Inputs  : Both pointers of even and odd arrays and their sizes.
* Outputs : The elements of whichever array and its size that were passed in as arr1
* 			and size1 are set equal to the elements of the dynamically allocated array.
* 			arr1 is printed.
******************************************************************************/
void arrayWar(int* arr1, int size1, int* arr2, int size2)
{
	int* warVictor = 0;
	int wVCounter = 0;
	int greatestSize;

	if (size1 > size2)
		greatestSize = size1;
	else if (size2 > size1)
		greatestSize = size2;
	else
		greatestSize = size1;

	warVictor = new int[greatestSize];

	if (size1 > size2)
	{
		for (int index1 = 0; index1 < size2; index1++)
		{
			if (arr1[index1] > arr2[index1])
			{
				warVictor[wVCounter] = arr1[index1];
				wVCounter++;
			}
			else if (arr2[index1] > arr1[index1])
			{
				warVictor[wVCounter] = arr2[index1];
				wVCounter++;
			}
			else if (arr1[index1] == arr2[index1])
			{
				warVictor[wVCounter] = arr1[index1];
				wVCounter++;
			}
		}
		for (int index2 = (size2 - 1); index2 < greatestSize; index2++)
		{
			warVictor[wVCounter] = arr1[index2];
			wVCounter++;
		}
	}
	else if (size2 > size1)
	{
		for (int counter1 = 0; counter1 < size1; counter1++)
		{
			if (arr1[counter1] > arr2[counter1])
			{
				warVictor[wVCounter] = arr1[counter1];
				wVCounter++;
			}
			else if (arr2[counter1] > arr1[counter1])
			{
				warVictor[wVCounter] = arr2[counter1];
				wVCounter++;
			}
			else if (arr1[counter1] == arr2[counter1])
			{
				warVictor[wVCounter] = arr1[counter1];
				wVCounter++;
			}
		}
		for (int counter2 = (size1 - 1); counter2 < size2; counter2++)
		{
			warVictor[wVCounter] = arr2[counter2];
			wVCounter++;
		}
	}
	else if (size1 == size2)
	{
		for (int ticker = 0; ticker < size1; ticker++)
		{
			if (arr1[ticker] > arr2[ticker])
			{
			warVictor[wVCounter] = arr1[ticker];
			wVCounter++;
			}
			else if (arr2[ticker] > arr1[ticker])
			{
				warVictor[wVCounter] = arr2[ticker];
				wVCounter++;
			}
			else
			{
				warVictor[wVCounter] = arr1[ticker];
				wVCounter++;
			}
		}
	}

	for (int ticker2 = 0; ticker2 < wVCounter; ticker2++)
	{
		arr1[ticker2] = warVictor[ticker2];
		size1 = wVCounter;
		cout << arr1[ticker2] << " ";
	}
}

/******************************************************************************
* Name    : sortArray
* Purpose : Using selection sort, this function sorts the elements of an array in
* 			ascending numeric order.
* Inputs  : An array pointer and its size.
* Outputs : Rearranges the elements and then prints out the array.
******************************************************************************/
void sortArray(int* arr, int size)
{
	int startscan, minValue, minIndex;
	for (startscan = 0; startscan < (size - 1); startscan++)
	{
		minIndex = startscan;
		minValue = arr[startscan];
		for (int index = startscan + 1; index < size; index++)
		{
			if (arr[index] < minValue)
			{
				minValue = arr[index];
				minIndex = index;
			}
		}
		arr[minIndex] = arr[startscan];
		arr[startscan] = minValue;
	}

	cout << "The array after being sorted is: \n";
	for (int counter = 0; counter < size; counter++)
	{
		cout << arr[counter] << " ";
	}

	cout << " " << endl;

}
