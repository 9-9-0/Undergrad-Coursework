#include "TreeNode.h"

/******************************************************************************
* Name    : TreeNode(sampleList set, double entropy, intVect availFeatureIndexes)
* Purpose : Initializes TreeNode instance with parameters passed in. Note that
* 			featureIndex is set to -1 (no split has occurred) and no children are
* 			attached yet to the instance.
* Inputs  : Set of samples (sampleList), entropy of that set (double), available
* 			feature indexes to further split on (intVect)
* Outputs : None.
******************************************************************************/
TreeNode::TreeNode(sampleList set, double entropy, intVect availFeatureIndexes)
{
	this->entropy = entropy;
	this->set = set;
	this->availFeatureIndexes = availFeatureIndexes;
	this->featureIndex = -1;
	this->amLeaf = false;

	this->children[0] = NULL;
	this->children[1] = NULL;
}

/******************************************************************************
* Name    : getChild(int childIndex)
* Purpose : Gets the child of a node.
* Inputs  : Index of the child (int), 0 for the child node that has a TRUE value
* 			for the parent node's featureIndex, 1 for the child node that has a
* 			FALSE value.
* Outputs : Pointer to a child (TreeNode*) or NULL if the node is a leaf.
******************************************************************************/
TreeNode* TreeNode::getChild(int childIndex)
{
	if (childIndex != 0 && childIndex != 1)
	{
		return NULL;
	}

	return children[childIndex];
}

/******************************************************************************
* Name    : setFeature2Split(int featureIndex)
* Purpose : Sets the node's best feature to split on.
* Inputs  : Index of feature (int)
* Outputs : None.
******************************************************************************/
void TreeNode::setFeature2Split(int featureIndex)
{
	this->featureIndex = featureIndex;

	/*
	std::cout << "available feature indexes: " <<std::endl;
	for (int i = 0; i < availFeatureIndexes.size(); i++)
	{
		std::cout << availFeatureIndexes[i];
	}
	*/
}

/******************************************************************************
* Name    : setChildren(int childIndex, TreeNode* child)
* Purpose : Sets the node's child.
* Inputs  : Index of child to be set (int), SEE getChild()'s documentation for
* 			which index to use, pointer to the child (TreeNode*).
* Outputs : None.
******************************************************************************/
void TreeNode::setChildren(int childIndex, TreeNode* child)
{
	if (childIndex != 0 && childIndex != 1)
	{
		return;
	}

	children[childIndex] = child;
}

/******************************************************************************
* Name    : calcProportion(int featureCount)
* Purpose : Calculates the proportion of true classifications of the node's set.
* Inputs  : Number of features (int), which is used to index a sample's
* 			classification.
* Outputs : Proportion of true classifications (double).
******************************************************************************/
double TreeNode::calcProportion(int featureCount)
{
	if (!isLeaf())
	{
		return -1;
	}
	sampleList::iterator it; //Would it be better to set this as a class member?

	int trueCount = 0;
	int totalCount = 0;
	double trueProp;

	for (it = set.begin(); it != set.end(); ++it)
	{
		//Check if sample classification is true or not.
		if ((*it)[featureCount] == 1)
		{
			trueCount++;
			totalCount++;
		}
		else
		{
			totalCount++;
		}
	}

	trueProp = double(trueCount) / double(totalCount);

	return trueProp;
}

bool TreeNode::checkForImpureSample(sample sampleInput, int featureCount)
{
	if (!isLeaf())
	{
		return -1;
	}
	sampleList::iterator it; //Would it be better to set this as a class member?

	int trueCount = 0;
	int totalCount = 0;
	double trueProp;

	for (it = set.begin(); it != set.end(); ++it)
	{
		//Check if sample classification is true or not.
		if ((*it) == sampleInput)
		{
			if ((*it)[featureCount] == 1)
			{
				trueCount++;
			}
			totalCount++;
		}
	}

	trueProp = double(trueCount) / double(totalCount);

	if (trueProp == 0)
	{
		return true;
	}
	return false;
}
