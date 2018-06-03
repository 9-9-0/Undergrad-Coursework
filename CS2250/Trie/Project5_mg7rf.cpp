/******************************************************************************
 * Course       : CS 2250 E01 (Fall 2015)
 * Assignment   : Project 5
 * Summary      : This program loads a dictionary from an input file, creates a
 * 				  tree and stores words/definitions inside it, and allows user
 * 				  to look up words and write the tree to a file.
 * Last Modified: December 6, 2015
 ******************************************************************************/
#include <iostream>
#include <string>
#include "dictionary.h"
using namespace std;

/* Revisions to make before pushing:
 * -Add word function
 * -Allocation test
 * -Fix any possible cin/getline bugs
 */
int main()
{
	string userFile;
	char userInput;
	string userWord;
	string definition;
	bool result;
	bool resultResult = false;

	myTrie dictionary;

	cout << "To begin, enter in the name of the dictionary file (with format guidelines followed): ";

	getline(cin, userFile);
	cout << "\n\nLoading dictionary...\n\n";
	result = dictionary.loadDictionary(userFile);

	while (!result)
	{
		cout << "Error opening dictionary. Please re-enter the filename (or enter 'Z' to exit): ";
		getline(cin, userFile);
		if (userFile == "Z")    		//See if this works
		{
			result = true;
			resultResult = true;
			break;
		}
		result = dictionary.loadDictionary(userFile);
		cout << "Loading Dictionary...\n\n";
	}

	if (result == true && resultResult == true)
	{
		cout << "Ending Program.";
		return 0;
	}
	else if (result == true && resultResult != true)
	{
		cout << "Would you like to look up a word? 'Y' or 'N'('N' to exit): ";
		cin >> userInput;
		cout << "\n";

		while (toupper(userInput) == 'Y')
		{
			cout << "Enter the word to be searched: ";
			cin.ignore();
			getline(cin, userWord);
			definition = dictionary.lookup(userWord);

			if (definition.size() == 0)
				cout << "Error: " << userWord << " was not found in the dictionary.\n"; //Implement add function?
			else if (definition.size() != 0)
				cout << "Definition: " << definition << endl;;

			cout << "Would you like to look up another word? 'Y' or 'N'('N' to exit): ";
			cin >> userInput;
		}

		cout << "Would you like to write the dictionary to a file? 'Y' or 'N'('N' to exit): ";

		cin >> userInput;

		if (toupper(userInput) == 'Y')
		{
			cout << "\nPlease enter in filename for the dictionary (entire Tree) to be written to: ";
			cin >> userFile;
			result = dictionary.writeTrie(userFile);
			cout << "\nWriting Dictionary...";

			if (!result)
				cout << "\nOperation unsuccessful...";
			else
				cout << "Success! Ending Program.";
		}
		else
			cout << "\nEnding Program.";
	}

	return 0;
}




