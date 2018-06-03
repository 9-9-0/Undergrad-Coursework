#ifndef TREENODE_H_
#define TREENODE_H_

#include <vector>
#include "Common.h"

class TreeNode
{
private:
	int featureIndex;				//featureIndex to base decision-making on, used for traversing
	bool amLeaf;					//Used for traversing
	double entropy;					//Entropy of node's set (i.e. prior to splitting into its children sets)
	sampleList set;					//List of dynamic_bitsets representing the samples in the set
	intVect availFeatureIndexes;	//Available featureIndexes on which to split the set

	TreeNode* children[2];			//children[0] points to the set with TRUE values for a feature after splitting on featureIndex
									//children[1] points to the set with FALSE values for a feature after splitting on featureIndex

public:
	TreeNode(sampleList set, double entropy, intVect availFeatureIndexes);

	bool isLeaf() { return amLeaf; };
	void setLeaf() { amLeaf = true; };
	double calcProportion(int featureCount);
	bool checkForImpureSample(sample sampleInput, int featureCount);

	//Getters
	sampleList getSet() { return set; };
	TreeNode* getChild(int childIndex);
	intVect getAvailFeatureIndexes() { return availFeatureIndexes; };
	int getFeatureIndex() { return featureIndex; };

	//Setters
	void setChildren(int childIndex, TreeNode* child);
	void setFeature2Split(int featureIndex); //featureIndex, not feature. Meaning, featureIndex is offset by 1.
};



#endif /* TREENODE_H_ */
