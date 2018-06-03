/******************************************************************************
 * Course       : CS 2250 E01 (Fall 2015)
 * Assignment   : Project 4
 * Summary      : Process incoming blocks of texts and checks for delimiter accuracy
 * Last Modified: 11.11.2015
 ******************************************************************************/
#include <iostream>
#include <string>
#include <vector>
#include "DelimiterStack.h"
using namespace std;

int main()
{
	DelimiterStack stack;

	bool isFinished = false;

	//Read-In Variables
	int lineCounter = 0;
	string userString;
	//vector<char> charCompare; may not need...line 45

	//Push Variables
	char pushChar   = '~';
	int pushCharNum = -1;
	int pushLineNum = -1;

	//Pop Variables
	char popChar;
	int popCharNum;
	int popLineNum;
	bool popCheck;

	cout << "Enter string blocks to be delimited (Enter 'DONE' when finished): ";

	while (!isFinished)
	{
		pushChar  = '~';
		pushCharNum = -1;
		pushLineNum = -1;
		lineCounter++;
		getline(cin, userString);

		if (userString == "DONE")
		{
			isFinished = true;
			break;
		}

		for (float i = 0; i < userString.length(); i++)
		{
			if (userString.at(i) == '{' || userString.at(i) == '[' || userString.at(i) == '(')
			{
				pushChar = userString.at(i);
				pushCharNum = i;
				pushLineNum = lineCounter;
				stack.push(pushChar, pushCharNum, pushLineNum);
				//charCompare.push_back(pushChar);
			}

			if (userString.at(i) == '}' || userString.at(i) == ']' || userString.at(i) == ')')
			{

				/*char popChar;
				int popCharNum;
				int popLineNum;
				bool popCheck;*/

				popCheck = stack.pop(popChar, popCharNum, popLineNum);

				if (popCheck)
				{
					if (userString.at(i) == ']' && popChar == '[')
					{
						break;
					}
					else if (userString.at(i) == '}' && popChar == '{')
					{
						break;
					}
					else if (userString.at(i) == ')' && popChar == '(')
					{
						break;
					}
					else
					{
						cout << "ERROR: Mismatched delimiter " << userString.at(i) << ", char " << i + 1 << " at line " << lineCounter << ". Found delimiter " << popChar << ", char " << popCharNum << " at line " << popLineNum << endl;
					}
				}
				else if (!popCheck && pushChar == '~' && pushCharNum == -1 && pushLineNum == -1)
				{
					cout << "ERROR: Right delimiter " << userString.at(i) << ", char " << i + 1 << " at line " << lineCounter << " has no found left delimiter.\n";
				}
			}
		}
	}

	bool finalCheck = stack.isEmpty();
	while (!finalCheck)
	{
		stack.pop(popChar, popCharNum, popLineNum);
		cout << "ERROR: Left delimiter " << popChar << ", char " << popCharNum << " at line " << popLineNum << " has no right delimiter.\n";
		finalCheck = stack.isEmpty();
	}

	return 0;
}



