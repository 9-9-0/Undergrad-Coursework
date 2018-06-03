#include "StateSpace.h"
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

//Private Member Functions
void StateSpace::setRoot()
{
	//Create and Assign Root Node
	StateNode* newNode = new StateNode();

	newNode->state.clear();
	newNode->state.push_back(0);
	newNode->state.push_back(0);
	newNode->state.push_back(0);

	newNode->parent = NULL;
	newNode->children.clear();

	this->root = newNode;
};

void StateSpace::setSpace()
{
	//Set limit for IDS
	cout << "Input desired max-depth for the iterative deepening search:";
	cin >> this->maxDepth;

	int solType;
	cout << "Enter 1 for raw state transitions if a solution is found.\nEnter anything else for text instructions to reach the solution:";
	cin >> solType;
	if (solType == 1) { this->rawSolution = true; }
	else			  { this->rawSolution = false; }

	//Clear junk in exploredStates
	vector<int> dummyVector;
	dummyVector.push_back(-1);
	dummyVector.push_back(-1);
	dummyVector.push_back(-1);

	//Set Goal State
	this->goalState.push_back(3);
	this->goalState.push_back(3);
	this->goalState.push_back(1);

	//Set Solution to None
	this->solution = NULL;
}

StateSpace::StateNode* StateSpace::isGoal(StateNode* node)
{
	if (node->state == goalState)
	{
		return node;
	}

	return NULL;
}

bool StateSpace::isValid(StateNode* node)
{
	//These can probably be simplified further to just a comparison between state[0] and state[1]
	int mCountRight = node->state[0];
	int cCountRight = node->state[1];
	int mCountWrong = 3 - mCountRight;
	int cCountWrong = 3 - cCountRight;

	if (node->state[2] == 0)
	{
		if (node->state[0] < 0 || node->state[1] < 0)
		{
			node->parent = NULL;
			return false;
		}
		/*
		if (mCountRight == 0)
		{
			return true;
		}
		*/
		if (mCountRight < cCountRight)
		{
			node->parent = NULL;
			return false;
		}
		return true;
	}
	else
	{
		if (node->state[0] > 3 || node->state[1] > 3)
		{
			node->parent = NULL;
			return false;
		}
		/*
		if (mCountRight == 0)
		{
			return true;
		}
		*/
		if (mCountWrong < cCountWrong)
		{
			node->parent = NULL;
			return false;
		}
		return true;
	}
}

void StateSpace::expandNode(StateNode* node)
{
	if (node->state[2] == 0)
	{
		//Generate Potential Children
		int boatVal = 1;
		StateNode* first = createNode(node->state[0] + 1, node->state[1], boatVal, node);
		StateNode* second = createNode(node->state[0] + 2, node->state[1], boatVal, node);
		StateNode* third = createNode (node->state[0], node->state[1] + 1, boatVal, node);
		StateNode* fourth = createNode(node->state[0], node->state[1] + 2, boatVal, node);
		StateNode* fifth = createNode(node->state[0] + 1, node->state[1] + 1, boatVal, node);
		//Test for Valid Children, then insert
		if (isValid(first)) { node->children.push_back(first);}
		if (isValid(second)) { node->children.push_back(second); }
		if (isValid(third)) { node->children.push_back(third); }
		if (isValid(fourth)) { node->children.push_back(fourth); }
		if (isValid(fifth)) { node->children.push_back(fifth); }
	}
	else
	{
		//Generate Potential Children
		int boatVal = 0;
		StateNode* first = createNode(node->state[0] - 1, node->state[1], boatVal, node);
		StateNode* second = createNode(node->state[0] - 2, node->state[1], boatVal, node);
		StateNode* third = createNode(node->state[0], node->state[1] - 1, boatVal, node);
		StateNode* fourth = createNode(node->state[0], node->state[1] - 2, boatVal, node);
		StateNode* fifth = createNode(node->state[0] - 1, node->state[1] -1, boatVal, node);
		//Test for Valid Children, then insert
		if (isValid(first)) { node->children.push_back(first); }
		if (isValid(second)) { node->children.push_back(second); }
		if (isValid(third)) { node->children.push_back(third); }
		if (isValid(fourth)) { node->children.push_back(fourth); }
		if (isValid(fifth)) { node->children.push_back(fifth); }
	}
}

StateSpace::StateNode* StateSpace::createNode(int m, int c, int b, StateNode* parent)
{
	StateNode* newNode = new StateNode();
	newNode->state.clear();
	newNode->parent = parent;
	newNode->children.clear();

	newNode->state.push_back(m);
	newNode->state.push_back(c);
	newNode->state.push_back(b);

	return newNode;
}

StateSpace::StateNode* StateSpace::DLS(StateNode* node, int limit)
{
	if (limit == 0 && isGoal(node))
	{
		return node;
	}
	if (limit > 0) //Expand node if greater...
	{
		expandNode(node);
		for (unsigned int i = 0; i < node->children.size(); i++)
		{
			StateNode* result = DLS(node->children[i], limit - 1);
			if (result)
			{
				return result;
			}
		}
	}
	return NULL;
}

void StateSpace::IDLS()
{
	int start = 0;

	while (start <= maxDepth && !solution)
	{
		setRoot();
		solution = DLS(root, start);
		start++;
		if (!solution)
		{
			cleanUp(root);
		}
	}

	if (solution)
	{
		cout << "\nGoal state reached at depth " << --start << " with the following state transitions:\n";

		if (rawSolution)
		{
			revIterateRaw(solution);
		}
		else
		{
			revIterate(solution);
		}
	}
	else
	{
		cout << "\nNo solution found with a maximum depth of " << maxDepth << ".\n";
	}
}

void StateSpace::revIterate(StateNode* node)
{
	if (node->parent){
		revIterate(node->parent);
		genInstructions(node, node->parent);
	}
}

void StateSpace::revIterateRaw(StateNode* node)
{
	if (node->parent){
		revIterateRaw(node->parent);
	}
	cout << node->state[0] << node->state[1] << node->state[2] << endl;
}

void StateSpace::genInstructions(StateNode* node, StateNode* parent)
{
	if (!parent)
	{
		return;
	}

	if (node->state[2] == 1)
	{
		int mMoved = node->state[0] - parent->state[0];
		int cMoved = node->state[1] - parent->state[1];
		cout << "\nMove " << mMoved << " missionaries and " << cMoved << " cannibals over to the correct side of the river.\n";
	}
	else
	{
		int mMoved = parent->state[0] - node->state[0];
		int cMoved = parent->state[1] - node->state[1];
		cout << "\nMove " << mMoved << " missionaries and " << cMoved << " cannibals over to the wrong side of the river.\n";
	}

}

void StateSpace::cleanUp(StateNode* node)
{
	/*
	if (node->children)
	{
		delete node;
		return;
	}
	*/
	if (node->children.size() != 0)
	{

		for (int i = 0; i < node->children.size(); i++)
		{
			cleanUp(node->children[i]);
		}
	}
	delete node;
	return;
}

//Public Member Functions
StateSpace::StateSpace()
{
	setSpace();
}

void StateSpace::run()
{
	IDLS();
}

StateSpace::~StateSpace()
{
	cleanUp(root);
}
