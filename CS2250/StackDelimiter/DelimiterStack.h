/******************************************************************************
 * Course       : CS 2250 E01 (Fall 2015)
 * Assignment   : Project 4
 * Summary      : Process incoming blocks of texts and checks for delimiter accuracy
 * Last Modified: 11.11.2015
 ******************************************************************************/
#ifndef DELIMITERSTACK_H_
#define DELIMITERSTACK_H_
#include <string>
using namespace std;

class DelimiterStack
{
private:
	struct stringNode
	{
		char character;
		int charNum;
		int lineNum;
		stringNode* next;
	};
	stringNode* top;
	string exceptionString;
public:
	DelimiterStack();
	~DelimiterStack();
	void push(char character, int charNum, int lineNum);
	bool pop(char &character, int &charNum, int &lineNum);
	bool isEmpty();
};



#endif
