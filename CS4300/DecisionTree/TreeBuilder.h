#ifndef TREEBUILDER_H_
#define TREEBUILDER_H_

#include "Common.h"

class TreeNode;

class TreeBuilder
{
private:
	TreeNode* treeRoot;					//Pointer to the root node of the tree

	sampleList originalSet;				//Original un-split set
	intVect origAvailFeatureIndexes;	//Initial indexes available to split on
	int featureCount;					//Number of features of each sample
	int sampleCount;					//Number of samples in original set (only used in constructor)

	void recursiveConstruct(TreeNode* node);
	TreeNode* createNode(sampleList set, double entropy, intVect availFeatureIndexes);

	sampleList* split(sampleList set, int featureIndex);
	bool belongsTo(sample sample, int featureIndex);
	double IGOnSplit(sampleList set, int feature);
	int getBestFeature2Split(TreeNode* node);
	double calcProportion(sampleList set, int feature);

public:
	TreeBuilder(sampleList set, int featureCount, int sampleCount);

	double calcEntropy(sampleList set); //Change this to a private method after debugging.

	void constructTree();

	void testFunction();

	int getFeatureCount() { return featureCount; };

	TreeNode* getTreeRoot() { return treeRoot; };
};



#endif /* TREEBUILDER_H_ */
