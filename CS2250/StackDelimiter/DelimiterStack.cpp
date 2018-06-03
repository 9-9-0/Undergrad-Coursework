/******************************************************************************
 * Course       : CS 2250 E01 (Fall 2015)
 * Assignment   : Project 4
 * Summary      : Process incoming blocks of texts and checks for delimiter accuracy
 * Last Modified: 11.11.2015
 ******************************************************************************/
#include "DelimiterStack.h"
#include <iostream>
#include <string>
#include <new>

 /******************************************************************************
 * Name    : Constructor
 * Purpose : Initializes exceptionString and top
 * Inputs  : None.
 * Outputs : None.
 ******************************************************************************/
DelimiterStack::DelimiterStack()
{
	exceptionString = "Not enough memory to create node, aborting program.";
	top = 0;
}

/******************************************************************************
* Name    : Destructor
* Purpose : Deletes heap memory when object is deleted/goes out of scope
* Inputs  : None.
* Outputs : None.
******************************************************************************/
DelimiterStack::~DelimiterStack()
{
	stringNode* nodePtr = 0;
	stringNode* nextNode = 0;

	nodePtr = top;

	while(nodePtr)
	{
		nextNode = nodePtr->next;
		delete nodePtr;
		nodePtr = nextNode;
	}
}

/******************************************************************************
* Name    : push
* Purpose : Creates a new node and stores inputs.
* Inputs  : Character, character number, and line number of right delimiter
* Outputs : void.
******************************************************************************/
void DelimiterStack::push(char character, int charNum, int lineNum)
{
	stringNode* newNode = 0;

	try
	{
		newNode = new stringNode;
	}
	catch (bad_alloc)
	{
		cout << exceptionString << endl;
		return;
	}

	newNode->character = character;
	newNode->charNum = charNum;
	newNode->lineNum = lineNum;

	if (isEmpty())
	{
		top = newNode;
		newNode->next = 0;
	}
	else
	{
		newNode->next = top;
		top = newNode;
	}

}

/******************************************************************************
* Name    : pop
* Purpose : If stack contains a node, pops the node and values are stored by reference.
* Inputs  : Reference character, character number, and line number
* Outputs : True/False based on whether or not stack is empty.
******************************************************************************/
bool DelimiterStack::pop(char &character, int &charNum, int &lineNum)
{
	stringNode* temp = 0;

	if (isEmpty())
		return false;
	else
	{
		character = top->character;
		charNum = top->charNum;
		lineNum = top->lineNum;

		temp = top->next;
		delete top;
		top = temp;

		return true;
	}
}

/******************************************************************************
* Name    : isEmpty
* Purpose : Determines wheter or not stack is empty.
* Inputs  : None.
* Outputs : True if empty, false if not.
******************************************************************************/
bool DelimiterStack::isEmpty()
{
	bool status;

	if (!top)
		status = true;
	else
		status = false;
	return status;
}



