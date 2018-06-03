#ifndef STATESPACE_H_
#define STATESPACE_H_

#include <vector>
#include <stack>
using namespace std;

class StateSpace
{
public:
	struct StateNode {
		vector<int> state;
		StateNode* parent;
		vector<StateNode*> children;
	};

	StateSpace();
	~StateSpace();
	void run();

private:

	int maxDepth;
	StateNode* root;
	vector<int> goalState;
	StateNode* solution;
	bool rawSolution;

	//Initialization
	void setRoot();
	void setSpace();

	//Recursive Depth-Limited Search & Iterative Depth-Limited Search
	StateNode* DLS(StateNode* node, int limit); //Depth-Limited Search (Iterative Version)
	void IDLS();

	//Node-Checking
	StateNode* isGoal(StateNode* node);
	bool isValid(StateNode* node);

	//Node Expansion & Creation
	void expandNode(StateNode* node);
	StateNode* createNode(int m, int c, int b, StateNode* parent);

	//Termination
	void revIterate(StateNode* node);
	void revIterateRaw(StateNode* node);
	void genInstructions(StateNode* node, StateNode* child);
	void cleanUp(StateNode* node);
};


#endif /* STATESPACE_H_ */
