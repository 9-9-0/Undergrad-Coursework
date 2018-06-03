/******************************************************************************
 * Course       : CS 2250 E01 (Fall 2015)
 * Assignment   : Project 2
 * Summary      : This program converts a user's sentence into pig latin.
 * Last Modified: October 3, 2015
 ******************************************************************************/
#include <iostream>
#include <cctype>
#include <string>
#include <cstring>
using namespace std;

struct Word
{
   string english;
   string piglatin;
};

Word* splitSentence(const string words, int& size);
void convertToPigLatin(Word wordArr[], int size);
void displayPigLatin(const Word wordArr[], int size);

/******************************************************************************
* Name    : main
* Purpose : Retrieves user's sentence and uses that to call various functions throughout
* Inputs  : None
* Outputs : Returns 0 if successful
******************************************************************************/
int main()
{

	string sentence;
	cout << "This program will print your sentence in pig latin (only alphabetical characters are counted)." << endl;
	cout << "Enter in your sentence: ";

	getline(cin, sentence);
	int size = sentence.length();

	Word* wordz = 0;
	wordz = splitSentence(sentence, size);

	convertToPigLatin(wordz, size);
	displayPigLatin(wordz, size);

	delete [] wordz;

	return 0;
}

/******************************************************************************
* Name    : splitSentence
* Purpose : Copies user's sentence and then reorganizes (removes extraneous spaces and non-alphabetical characters)
* 			user's sentence in stores each word into the english member of each heap structure
* Inputs  : User's sentence (as a constant) and size of said sentence
* Outputs : Returns the heap array and new size (number of words) by reference
******************************************************************************/

Word* splitSentence(const string words, int& size)         //Test with "   yes. y13es  yes   "
{
	string wordsCopy1 = words;
	int startPosition = -1;
	int endPosition = -1;

	for (int a = wordsCopy1.length() - 1; a >= 0; a--) //Finds location of last character and stores it
	{
		if (isalpha(words[a]))
		{
			endPosition = a + 1;
			break;
		}
	}

	if (endPosition != -1)
		wordsCopy1.erase(endPosition, wordsCopy1.length() - endPosition);

	for (int b = 0; b < endPosition; b++)			   //Finds location of first character and stores it
	{
		if (isalpha(wordsCopy1[b]))
		{
			startPosition = b;
			break;
		}
	}

	if (startPosition != -1)
		wordsCopy1.erase(0, startPosition);

	// cout << endPosition;   Tests if endposition is correct

	for (int d = wordsCopy1.length() - 1; d >= 0; d--) //Removes any consecutive spaces in the sentence
	{
		if (isspace(wordsCopy1[d]) && isspace(wordsCopy1[d-1]))
			wordsCopy1.erase(d, 1);
	}

	for (int e = wordsCopy1.length() - 1; e >= 0; e--) //Removes any non-alpha characters in the sentence
	{
		if (!isalpha(wordsCopy1[e]) && !isspace(wordsCopy1[e]))
			wordsCopy1.erase(e, 1);
	}

	int wordCounter = 1;
	for (int f = wordsCopy1.length() - 1; f >= 0; f--) //Counts number of words based on number of spaces
	{
		if (isspace(wordsCopy1[f]))
			wordCounter++;
	}

	size = wordCounter;
	//cout << size << endl;    Test: Is size correct?

	Word* wordArray = 0;
	wordArray = new Word[size];
	int wordArrayCounter = size - 1;
	int arrayEnd = wordsCopy1.length() - 1;
	int arrayStart = 0;

	for (int z = wordsCopy1.length() - 1; z >= 0; z--) //Splits sentence into substring and allocates them into the heap structure
	{
		if (isspace(wordsCopy1[z]) && z != 0)
		{
			arrayStart = z;
			wordArray[wordArrayCounter].english = wordsCopy1.substr(arrayStart + 1, arrayEnd - arrayStart);
			wordArrayCounter--;
			wordsCopy1.erase(arrayStart, arrayEnd - arrayStart + 1);
			arrayEnd = wordsCopy1.length() - 1;
		}
		else if (z == 0)
		{
			arrayStart = z;
			wordArray[wordArrayCounter].english = wordsCopy1.substr(arrayStart, arrayEnd + 1 - arrayStart);
			wordsCopy1.erase(arrayStart, arrayEnd);
		}
	}

	return wordArray;

	/*for (int z = 0; z < size; z++)     Tests if wordArray was allocated correctly
	{
		cout << wordArray[z].english << "." << endl;
	}*/
}

/******************************************************************************
 * Name    : convertToPigLatin
 * Purpose : Checks the first character of each word and translates word into pig latin according to the character.
 * Inputs  : Word structure array (as constant) and number of words
 * Outputs : Translates the words accurately if successful
 ******************************************************************************/
void convertToPigLatin(Word wordArr[], int size)
{
	string ayCons = "ay";
	string ayVow = "way";
	for (int a = 0; a < size; a++) //Appends either "ay" or "way" depending on first character
	{
		if (tolower((wordArr + a)->english.at(0)) != 'a' && tolower((wordArr + a)->english.at(0)) != 'e' && tolower((wordArr + a)->english.at(0)) != 'i' && tolower((wordArr + a)->english.at(0)) != 'o' && tolower((wordArr + a)->english.at(0)) != 'u')
		{
			(wordArr + a)->piglatin = (wordArr + a)->english;
			char firstChar = (wordArr + a)->piglatin.at(0);
			(wordArr + a)->piglatin.erase(0, 1);
			(wordArr + a)->piglatin.append(1, firstChar);
			(wordArr + a)->piglatin.append(ayCons);
		}
		else
		{
			(wordArr + a)->piglatin = (wordArr + a)->english;
			(wordArr + a)->piglatin.append(ayVow);
		}
	}
	/*for (int counter = 0; counter < size; counter++) Tests to see if the function worked.
	{
		cout << (wordArr + counter)->piglatin << endl;
	}*/
}

/******************************************************************************
 * Name    : displayPigLatin
 * Purpose : Displays the user's original sentence and user's sentence in pig latin (both with periods to denote sentence end point)
 * Inputs  : Word structure array (as constant) and number of words
 * Outputs : Prints out both sentences if successful
 ******************************************************************************/
void displayPigLatin(const Word wordArr[], int size)
{
	cout << "Your original sentence: \n";
	for (int b = 0; b < size; b++)
	{
		if (b == size - 1)
		{
			cout << wordArr[b].english << ".\n";
			break;
		}
		cout << wordArr[b].english << " ";
	}

	cout << "Your sentence in pig latin: \n";
	for (int a = 0; a < size; a++)
	{
		if (a == size - 1)
		{
			cout << wordArr[a].piglatin << ".\n";
			break;
		}
		cout << wordArr[a].piglatin << " ";
	}
}

