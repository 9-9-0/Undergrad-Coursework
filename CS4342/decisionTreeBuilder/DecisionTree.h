#ifndef DECISIONTREE_H_
#define DECISIONTREE_H_

#include <iomanip>
#include <stdlib.h>
#include <time.h>
#include "Common.h"

class TreeNode;

class DecisionTree {
private:
	TreeNode* treeRoot;
	TreeNode* queryPointer;
	int numFeatures;


public:
	DecisionTree(TreeNode* root, int featureCount);
	~DecisionTree();

	bool queryTree(bool* sampleVals);
	TreeNode* getTreeRoot() { return treeRoot; };

	void recursiveDelete(TreeNode* node);
	void printTree(TreeNode* node, int indent=0);

	sample bool2Bitset(bool* sampleVals);
	sample* isInSet(TreeNode* node, sample sampleInput);


	void saveTree(std::string fileName);
	void writeNodeToFile(std::ofstream* outfile, TreeNode* node);
	int getLeafPrediction(TreeNode* node);

};

#endif /* DECISIONTREE_H_ */
