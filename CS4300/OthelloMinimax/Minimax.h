#ifndef MINIMAX_H_
#define MINIMAX_H_

#include <cstdio>
#include <ctime>
#include "GameV2.h"
#include "Clock.h"

class Minimax
{
private:
	//A state and all actions it retains
	struct minimaxNode {
		Board state;
		std::vector<int> actionTo;			//Action performed that resulted in this state
		int ply; 							//1 for player 1, 2 for player 2
		int fitness;						//Delete this if unused...
		std::vector<std::vector<int> > moves;		//All possible actions from this node

		minimaxNode* parent;				//Is this really necessary?
		std::vector<minimaxNode*> children; //Is this really necessary?
	};

	//Clock Variables
	Clock* moveTimer;
	double moveTimeLimit;
	int computerPlayer; //Computer ALWAYS begins as MAX
	char computerColor;

	//Initialization Variables
	Board* startBoard; //DO NOT DELETE THIS, POINTER TO GAME's CURRENT BOARD
	int startPly;
	int startPlayer;

	//Minimax Algorithm Functions
	bool isEndState(minimaxNode* node);							//Terminal Test
	std::vector<int> scoreEndGame(minimaxNode* node);			//Fitness Scorer for a terminated game
	std::vector<int> minimaxUtility(minimaxNode* node);			//State Fitness Scorer
	std::vector<int> minimaxDLS(minimaxNode* node, int depth);	//Recursive DLS

	//Update Functions
	void setActions(minimaxNode* node);
	minimaxNode* genSuccessor(minimaxNode* node, std::vector<int> move);

	void printNode(minimaxNode* node);
	void setRoot();

public:
	Minimax();
	Minimax(Board* rootBoard, int ply, int player); 	//Ply gets passed from Game
	~Minimax();

	//Root Node
	minimaxNode* root;

	std::vector<int> minimaxIDS(minimaxNode* node);

};




#endif /* MINIMAX_H_ */
