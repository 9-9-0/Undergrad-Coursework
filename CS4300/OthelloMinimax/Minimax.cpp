#include "Minimax.h"

/******************************************************************************
* Name    : Minimax(Board* rootBoard, int ply, int player)
* Purpose : Initializes an instance by allocating memory for its clock object
* 			and setting appropriate member variables.
* Inputs  : Board* 'rootBoard', pointing to the root state from which Minimax will
* 			run from.
* 			Integer 'ply' denoting the computer player's ply number.
* 			Integer 'player' denoting the computer player's number.
* Outputs : None.
******************************************************************************/
Minimax::Minimax(Board* rootBoard, int ply, int player)
{
	//Set Clock
	moveTimeLimit = 5.0;
	moveTimer = new Clock(moveTimeLimit);

	this->startBoard = rootBoard;
	this->startPly = ply;
	this->startPlayer = player;

	setRoot();

	//std::cout << &rootBoard << " " << &root->state << std::endl;
}

/******************************************************************************
* Name    : ~Minimax()
* Purpose : Deletes the memory held by root and moveTimer.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
Minimax::~Minimax()
{
	delete root;
	delete moveTimer;
}

/******************************************************************************
* Name    : setRoot()
* Purpose : Sets the contents of the root node.
* Inputs  : None.
* Outputs : None.
* Notes	  :
* 	- The preceding action of the root node's Board state, actionTo, is set to
* 	  -1, -1. Implications are detailed in the README.
******************************************************************************/
void Minimax::setRoot()
{
	computerPlayer = this->startPlayer;
	if (computerPlayer == 1)
	{
		computerColor = 'b';
	}
	else
	{
		computerColor = 'w';
	}

	//Set root
	root = new minimaxNode();
	root->ply = this->startPly;
	root->actionTo.push_back(-1);
	root->actionTo.push_back(-1);
	root->parent = 0;
	root->fitness = 0;
	root->state = *this->startBoard; //This should call the overloaded operator
	setActions(root);
}

/******************************************************************************
* Name    : setActions(minimaxNode* node)
* Purpose : Sets all the possible actions for a node given its Board state.
* Inputs  : minimaxNode* 'node', pointing to the node whose actions are to be set
* Outputs : None.
******************************************************************************/
void Minimax::setActions(minimaxNode* node)
{
	//Decide color
	char color;
	if (node->ply == 1)
	{
		color = 'b';
	}
	else
	{
		color = 'w';
	}

	//Create temporary containers
	std::vector<std::vector<int> > tempMoves(0);
	tempMoves.clear();

	//Store moves of board in temp container
	node->moves = node->state.getValidMoves(color);
}

/******************************************************************************
* Name    : genSuccessor(minimaxNode* node, std::vector<int> move)
* Purpose : Given a node's Board state and one of its possible actions, generates
* 			a new node containing a Board state resulting from the action performed on
* 			the previous node's Board state. The ply of the new node is also
* 			flipped, marking a change in which player's turn it is.
* Inputs  : minimaxNode* 'node', pointing to the parent node.
* 			std::vector<int> 'move', containing the two integers of where a piece is to
* 			be placed.
* Outputs : None.
******************************************************************************/
Minimax::minimaxNode* Minimax::genSuccessor(minimaxNode* node, std::vector<int> move)
{
	//Initialize child node
	minimaxNode* output = new minimaxNode();

	char color;
	//Set child's ply along with color of parent node's move
	if (node->ply == 1)
	{
		output->ply = 2;
		color = 'b';
	}
	else
	{
		output->ply = 1;
		color = 'w';
	}

	//Set child's parent
	output->parent = node;

	//Set action leading to state
	output->actionTo.push_back(move[0]);
	output->actionTo.push_back(move[1]);

	//Set child's board state to parent's board state
	output->state = node->state; //SHOULD CALL THE COPY CONSTRUCTOR

	//Perform move on child's board state
	output->state.setSinglePiece(color, move[0], move[1]);
	for (int i = 0; i < 8; i++)
	{
		output->state.flipScan(move[0], move[1], color, i, true);
	}

	//Set possible moves from chlid's new state
	setActions(output);

	return output;
}

/******************************************************************************
* Name    : printNode(minimaxNode* node)
* Purpose : Prints details of a node. Used for console debugging when called by DLS.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
void Minimax::printNode(minimaxNode* node)
{
	std::cout << "Current State: " << std::endl;
	node->state.printBoard();
	std::cout << "Previous move: " << node->actionTo[0] << " " << node->actionTo[1] << std::endl;
	std::cout << "Current ply: " << node->ply << std::endl;
	std::cout << "Possible actions: " << std::endl;
	for (int i = 0; i < node->moves.size(); i++)
	{
		std::cout << node->moves[i][0] << " " << node->moves[i][1] << std::endl;
	}
}

/******************************************************************************
* Name    : minimaxIDS(minimaxNode* node)
* Purpose : Runs Iterative-Deepening Search for 5 seconds on passed in node.
* Inputs  : minimaxNode* 'node', pointing to the starting node (root always used)
* Outputs : std::vector<int> containing members in the following order: row of
* 			best move, column of best move, fitness of best move, depth at which
* 			best move was discovered.
******************************************************************************/
std::vector<int> Minimax::minimaxIDS(minimaxNode* node)
{
	//Start timer
	moveTimer->resetTime();

	//Create vector holding best move
	std::vector<int> bestMove;
	bestMove.clear();
	bestMove.push_back(-1);
	bestMove.push_back(-1);
	bestMove.push_back(-1);
	bestMove.push_back(-1);

	//Create vector holding temporary best move
	std::vector<int> bestMoveTemp;
	bestMove.clear();

	//Create depth-counter
	int maxDepth = 0;

	//Run IDS
	while (!moveTimer->timesUp())
	{
		maxDepth++;
		bestMoveTemp = minimaxDLS(root, maxDepth);
		bestMoveTemp.push_back(maxDepth);

		if (bestMoveTemp[3] > bestMove[3])
		{
			bestMove = bestMoveTemp;
		}

		setRoot();
	}

	return bestMove;
}


/******************************************************************************
* Name    : minimaxDLS(minimaxNode* node, int depth)
* Purpose : Runs recursive Depth-Limited Search on node to depth 'depth'.
* Inputs  : minimaxNode* 'node', pointing to DLS' start node.
* 			Integer 'depth', denoting the maximum depth of search.
* Outputs : std::vector<int> of 3 members: row of best move, column of best move,
* 			fitness of best move.
******************************************************************************/
std::vector<int> Minimax::minimaxDLS(minimaxNode* node, int depth)
{
	//printNode(node);

	//Is this state the end of game? (figure out where to relocate this to improve time complexity)
	if (isEndState(node))
	{
		return scoreEndGame(node);
	}

	//Is time up? Depth-Limit reached?
	if (moveTimer->timesUp() || depth == 0)
	{
		//Score and return utility
		return minimaxUtility(node);

	}

	//Get MAX's best move
	if (node->ply == computerPlayer)
	{
		minimaxNode* tempPntr;

		int maxFit = -1000;
		std::vector<int> bestMaxMove;
		std::vector<int> tempMove;

		//Temporary, hacky Solution for handling skipped turns
		if (node->moves.size() == 0)
		{
			return minimaxUtility(node);
		}

		//Generate all successors and push them into node's children vector
		for (int i = 0; i < node->moves.size(); i++)
		{
			tempPntr = genSuccessor(node, node->moves[i]);	//Possible memory leak here...
			node->children.push_back(tempPntr);

			tempMove = minimaxDLS(node->children[i], depth - 1);

			if (tempMove[2] > maxFit)
			{
				maxFit = tempMove[2];
				bestMaxMove = node->moves[i]; //Note that copying might cause issues...
			}
		}

		bestMaxMove.push_back(maxFit);

		delete node;

		//std::cout << "Best Move Max: " << bestMaxMove[0] << bestMaxMove[1] << " Fitness: " << bestMaxMove[2] << std::endl;

		return bestMaxMove;
	}
	//Get MIN's best move
	else
	{
		minimaxNode* tempPntr;

		int minFit = 1000;
		std::vector<int> bestMinMove;
		std::vector<int> tempMove;

		//Temporary, hacky Solution for handling skipped turns
		if (node->moves.size() == 0)
		{
			return minimaxUtility(node);
		}

		//Generate all successors and push them into node's children vector
		for (int i = 0; i < node->moves.size(); i++)
		{
			tempPntr = genSuccessor(node, node->moves[i]);
			node->children.push_back(tempPntr);

			tempMove = minimaxDLS(node->children[i], depth - 1);
			if (tempMove[2] < minFit)
			{
				minFit = tempMove[2];
				bestMinMove = node->moves[i]; //Note that copying might cause issues...
			}
		}

		bestMinMove.push_back(minFit);

		delete node;

		//std::cout << "Best Move Min: " << bestMinMove[0] << bestMinMove[1] << " Fitness: " << bestMinMove[2] << std::endl;

		return bestMinMove;
	}
}

/******************************************************************************
* Name    : isEndState(minimaxNode* node)
* Purpose : Determines if node's Board state is a terminal state.
* Inputs  : minimaxNode* 'node', pointing to the node in question.
* Outputs : True or False.
******************************************************************************/

bool Minimax::isEndState(minimaxNode* node)
{
	std::vector<std::vector<int> > p1MovesLeft = node->state.getValidMoves('b');
	std::vector<std::vector<int> > p2MovesLeft = node->state.getValidMoves('w');

	if (p1MovesLeft.size() == 0 && p2MovesLeft.size() == 0)
	{
		return true;
	}

	return false;
}


/******************************************************************************
* Name    : scoreEndGame(minimaxNode* node)
* Purpose : Terminal function called by DLS when a node's Board state is determined
* 			as terminal.
* Inputs  : minimaxNode* 'node', pointing to the terminal node.
* Outputs : std::vector<int> containing members in the following order: row of
* 			previous move made, column of previous move made, fitness of terminal
* 			state.
******************************************************************************/
std::vector<int> Minimax::scoreEndGame(minimaxNode* node)
{
	std::vector<int> output;
	output.clear();
	output.push_back(node->actionTo[0]);
	output.push_back(node->actionTo[1]);

	int score = 0;
	int boardDim = node->state.getDim();
	char** board = node->state.getState();

	for (int i = 0; i < boardDim; i++)
	{
		for (int j = 0; j < boardDim; j++)
		{
			if (board[i][j] == computerColor)
			{
				score++;
			}
			else
			{
				score--;
			}
		}
	}

	score = 10*score;
	output.push_back(score);

	delete node;

	return output;
}

/******************************************************************************
* Name    : minimaxUtility(minimaxNode* node)
* Purpose : Scores the node's Board state fitness in the case that a state is non
* 			-terminal.
* Inputs  : minimaxNode* 'node', pointing to the node to be scored.
* Outputs : std::vector<int> containing members in the following order: row of
* 			previous move made, column of previous move mdae, fitness of node's
* 			state.
******************************************************************************/
//Need to create a more efficient way of gauging board utility...
std::vector<int> Minimax::minimaxUtility(minimaxNode* node)
{
	//Prepare return value
	std::vector<int> output;
	output.clear();
	//Push in actions that resulted in this node's state
	output.push_back(node->actionTo[0]);
	output.push_back(node->actionTo[1]);

	int utilityOutput = 0;

	int boardDim = node->state.getDim();
	char** board = node->state.getState();

	//Begin scoring
	for (int i = 0; i < boardDim; i++)
	{
		for (int j = 0; j < boardDim; j++)
		{
			if (board[i][j] == 'e')
			{
				continue;
			}
			//Account for corners
			if ((i == 0 && j == 0) || (i == 0 && j == boardDim - 1) || (i == boardDim - 1 && j == 0) || (i == boardDim - 1 && j == boardDim - 1))
			{
				if (board[i][j] == computerColor)
				{
					utilityOutput += 10;
					continue;
				}
				else
				{
					utilityOutput -= 10;
					continue;
				}
			}
			//Account for high risk regions, top half board (6 possible)
			else if ((i == 0 && j == 1) || (i == 0 && j == boardDim - 2) || (i == 1 && j == 0) || (i == 1 && j == 1) || (i == 1 && j == boardDim - 2) || (i == 1 && j == boardDim - 1))
			{
				if (board[i][j] == computerColor)
				{
					utilityOutput -= 3;
					continue;
				}
				else
				{
					utilityOutput += 3;
					continue;
				}
			}
			//Account for high risk regions, bottom half board (6 possible)
			else if ((i == boardDim - 2 && j == 0) || (i == boardDim - 2 && j == 1) || (i == boardDim - 2 && j == boardDim - 2) || (i == boardDim - 2 && j == boardDim - 1) || (i == boardDim - 1 && j == 1) || (i == boardDim - 1 && j == boardDim -2))
			{
				if (board[i][j] == computerColor)
				{
					utilityOutput -= 3;
					continue;
				}
				else
				{
					utilityOutput += 3;
					continue;
				}
			}
			//Account for edge pieces
			else if (i == 0 || i == boardDim - 1 || j == 0 || j == boardDim -1)
			{
				if (board[i][j] == computerColor)
				{
					utilityOutput += 2;
					continue;
				}
				else
				{
					utilityOutput -= 2;
					continue;
				}
			}
			//Everything else
			else
			{
				if (board[i][j] == computerColor)
				{
					utilityOutput += 1;
					continue;
				}
				else
				{
					utilityOutput -= 1;
					continue;
				}

			}
		}
	}

	output.push_back(utilityOutput);

	delete node;

	//Debug Lines
	/*
	std::cout << "Utility Score Returning..." << std::endl;
	std::cout << output[0] << output[1] << output[2] << std::endl;
	*/

	return output;
}
