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
#include <fstream>
#include "dictionary.h"
using namespace std;

/******************************************************************************
* Name    : myTrie()
* Purpose : Constructor that calls create_trie
* Inputs  : None.
* Outputs : None.
******************************************************************************/
myTrie::myTrie()
{
	root = create_trie();
}
/******************************************************************************
* Name    : ~myTrie()
* Purpose : Destructor that calls delete_trie()
* Inputs  : None.
* Outputs : None.
******************************************************************************/
myTrie::~myTrie()
{
	deleteTrie();
}
/******************************************************************************
* Name    : create_trie()
* Purpose : Creates a root node and initializes all its next pointers to nullptr
* Inputs  : None.
* Outputs : Returns the address of the root node
******************************************************************************/
::myTrie::Trie myTrie::create_trie() //include a try-catch for alloc
{
	Trie root = 0;
	root = new TrieNode;
	for (int i = 0; i < ALPHABET_SIZE; i++)
	{
		root->next[i] = 0;
	}
	return root;
}
/******************************************************************************
* Name    : insert_trie
* Purpose : Creates a path in the tree for the word and its definition
* 			(e.g. the word 'cat' contains three nodes (not including the root node:
* 			Node 1: 'c'
* 			Node 2: 'ca'
* 			Node 3: 'cat' along with definition of 'cat'
* Inputs  : The word and its definition
* Outputs : Returns true if word was successfully inserted, false if not (word already exists)
******************************************************************************/
bool myTrie::insert_trie(string word, string definition)
{
	string wordCopy;

	for (float i = 0; i < word.size(); i++)
	{
		if (isalpha(word[i]))
		{
			wordCopy += tolower(word[i]);
		}
	}                                              //Creates a copy of the word argument and stores all alphabetical characters as lower case

	Trie nodePtr = 0;
	nodePtr = root;									//Initiates a TrieNode pointer and sets that equal to root
	bool insertStatus;								//Return value
	char letter;									//Character variable to use in each loop iteration
	int letterIndex;								//Index of each node's next array
	float stringCounter;							//Used in adding parts of the word to each node

	for (float i = 0; i < wordCopy.size(); i++)
	{
		letter = wordCopy[i];
		letterIndex = letter - 'a';					//Calculates letter index to be used

		if (nodePtr->next[letterIndex] != 0)           //If nodePtr points to a node corresponding with the 'i'th letter
		{
			nodePtr = nodePtr->next[letterIndex];
			insertStatus = false;
		}
		else if (nodePtr->next[letterIndex] == 0)		//If nodePtr doesn't point to a corresponding node
		{
			nodePtr->next[letterIndex] = new TrieNode;  //Allocate a new node
			nodePtr = nodePtr->next[letterIndex];		//set nodePtr equal to that new node
			for (int a = 0; a < ALPHABET_SIZE; a++)
			{
				nodePtr->next[a] = 0;
			}

			stringCounter = 0;

			if (i == wordCopy.size() - 1)  //In the case the last letter of the word
			{
				nodePtr->word = wordCopy;
				nodePtr->definition = definition;
			}
			else										//Node between the first letter node and the full word's node
			{
				while (stringCounter <= i)
				{
				nodePtr->word += wordCopy[stringCounter];
				stringCounter++;
				}
			}
			insertStatus = true;
		}
	}

	return insertStatus;
}
/******************************************************************************
* Name    : loadDictionary
* Purpose : Loads a user's dictionary. Delimits entries by '\n' and then separates
* 			each word from its definition by ':'
* Inputs  : Name of user's dictionary file
* Outputs : Returns true if successfully loaded, false if not
******************************************************************************/
bool myTrie::loadDictionary(string fileName)
{
	const char* cFileName = fileName.c_str();
	string defInput;
	string wordInput;
	string entry;
	int breakPoint;
	bool funcSuccess;
	bool insSuccess;

	fstream inputFile;
	inputFile.open(cFileName, ios::in);

	cout << "---------------\n" << "Status of Dictionary Load:\nCONTENTS LOADED:\n";

	if (inputFile)
	{
		getline(inputFile, entry);

		while (inputFile)
		{
			breakPoint = entry.find(':');
			wordInput = entry.substr(0, breakPoint);
			defInput = entry.substr(breakPoint + 2, entry.size() - 1);
			cout << wordInput << ":" << defInput << endl;

			insSuccess = insert_trie(wordInput, defInput);

			if (!insSuccess)
			{
				cout << "-Tree already contains path for: " << wordInput << endl;
			}

			getline(inputFile, entry);
		}
		cout << "Load Complete.\n---------------\n";
		inputFile.close();
		funcSuccess = true;
	}
	else
	{
		cout << "- Dictionary file could not be opened.\n";
		cout << "---------------\n";
		funcSuccess = false;
	}
	return funcSuccess;
}
/******************************************************************************
* Name    : lookup
* Purpose : Searches the tree for the word's definition
* Inputs  : Word
* Outputs : Returns a definition (if word exists) or an empty string (if word doesn't exist)
******************************************************************************/
string myTrie::lookup(string word)
{
	const char* constSearchArr = word.c_str();
	char* searchArr;
	for (int i = 0; i < word.length(); i++)
	{
		searchArr[i] = tolower(constSearchArr[i]);
	}

	int letterIndex;
	string returnString;

	TrieNode* nodePtr = 0;
	nodePtr = root;

	for (int i = 0; i < word.size(); i++)
	{
		if (i < word.size() - 1)
		{
			letterIndex = searchArr[i] - 'a';
			nodePtr = nodePtr->next[letterIndex];
			if (!nodePtr)
				i = word.size();
		}
		else if (i == word.size() - 1)
		{
			letterIndex = searchArr[i] - 'a';
			nodePtr = nodePtr->next[letterIndex];
			if (!nodePtr)
				i = word.size();
			else
			{
				returnString = nodePtr->definition;
			}
		}
	}

	return returnString;
}
/******************************************************************************
* Name    : deleteMyTrie
* Purpose : Recursive function that cycles through the tree and deletes all nodes
* Inputs  : root node
* Outputs : None
******************************************************************************/
void myTrie::deleteMyTrie(TrieNode* nodePtr)
{
	if (!nodePtr)
	{
		return;
	}
	else
	{
		for (int i = 0; i < ALPHABET_SIZE; i++)
		{
			if (nodePtr->next[i])				//This can be deleted if you want the function called for every single node
				deleteMyTrie(nodePtr->next[i]);
		}
	}

	delete nodePtr;
	//cout << "Node Deleted"; //Test
	return;
}
/******************************************************************************
* Name    : deleteTrie
* Purpose : Calls deleteMyTrie
* Inputs  : None.
* Outputs : None.
******************************************************************************/
void myTrie::deleteTrie()
{
	deleteMyTrie(root);
}
/******************************************************************************
* Name    : createWriteContent
* Purpose : Uses preorder traversal to capture all data in nodes and then stores them
* 			in a string that is used in writeTrie()
* Inputs  : root node
* Outputs : none
******************************************************************************/
void myTrie::createWriteContent(TrieNode* nodePtr) //Note: Unused member function (root inaccessible)
{
	if (!nodePtr)
	{
		cout << "Failure";
		return;
	}
	else
	{
		if (!(nodePtr->word.empty()) && nodePtr->definition.empty())
		{
			writeContents += nodePtr->word;
			writeContents += ": \n";
			//cout << nodePtr->word << ": " << endl;
		}
		else if (!(nodePtr->word.empty()) && !(nodePtr->definition.empty()))
		{
			writeContents += nodePtr->word;
			writeContents += ": ";
			writeContents += nodePtr->definition;
			writeContents += ". \n";
			//cout << nodePtr->word << ": " << nodePtr->definition << "." << endl;
		}
		for (int i = 0; i < ALPHABET_SIZE; i++)
		{
			if (nodePtr->next[i])
				createWriteContent(nodePtr->next[i]);
		}
	}
}
/******************************************************************************
* Name    : displayTrie
* Purpose : Calls displayTrieReal
* Inputs  : None.
* Outputs : None.
******************************************************************************/
void myTrie::displayTrie()
{
	displayTrieReal(root);
}
/******************************************************************************
* Name    : displayTrieReal
* Purpose : Based off of createWriteContent(), simply prints out each node instead
* 			of storing them
* Inputs  : root node
* Outputs : None
******************************************************************************/
void myTrie::displayTrieReal(TrieNode* nodePtr)
{
	if (!nodePtr)
	{
		cout << "Failure";
		return;
	}
	else
	{
		if (!(nodePtr->word.empty()) && nodePtr->definition.empty())
		{
			cout << nodePtr->word << ": " << endl;
		}
		else if (!(nodePtr->word.empty()) && !(nodePtr->definition.empty()))
		{
			cout << nodePtr->word << ": " << nodePtr->definition << "." << endl;
		}
		for (int i = 0; i < ALPHABET_SIZE; i++)
		{
			if (nodePtr->next[i])
				displayTrieReal(nodePtr->next[i]);
		}
	}
}
/******************************************************************************
* Name    : writeTrie
* Purpose : Writes the string created in createWriteContent() to the file designated
* 			by the user
* Inputs  : Output file name
* Outputs : Returns true if operation is successful, false if not
******************************************************************************/
bool myTrie::writeTrie(string fileName)
{
	bool operationStatus;
	const char* fileNameCopy = fileName.c_str();

	fstream dataFile;
	dataFile.open(fileNameCopy, ios::out | ios::app);

	createWriteContent(root);


	dataFile << writeContents;

	if (!dataFile)
		operationStatus = false;
	else
		operationStatus = true;

	dataFile.close();

	return operationStatus;
}
