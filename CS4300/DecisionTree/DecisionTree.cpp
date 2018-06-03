#include "DecisionTree.h"
#include "TreeNode.h"

/******************************************************************************
* Name    : DecisionTree(TreeNode* root, int featureCount)
* Purpose : Instantiates a decision tree.
* Inputs  : Root node of tree (TreeNode*), number of features for the set (int).
* Outputs : None.
******************************************************************************/
DecisionTree::DecisionTree(TreeNode* root, int featureCount)
{
	this->treeRoot = root;
	this->numFeatures = featureCount;
	this->queryPointer = NULL;
}

/******************************************************************************
* Name    : ~DecisionTree()
* Purpose : Calls recursiveDelete() on the root node.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
DecisionTree::~DecisionTree() {
	recursiveDelete(treeRoot);
}

/******************************************************************************
* Name    : recursiveDelete(TreeNode* node)
* Purpose : Recursively deletes a tree given the starting node using post-order
* 			traversal.
* Inputs  : Node to delete from (TreeNode* node)
* Outputs : None.
******************************************************************************/
void DecisionTree::recursiveDelete(TreeNode* node)
{
	if (!node)
	{
		return;
	}
	else
	{
		if (node->getChild(0))
		{
			recursiveDelete(node->getChild(0));
		}
		if (node->getChild(1))
		{
			recursiveDelete(node->getChild(1));
		}
	}
	delete node;
}

/******************************************************************************
* Name    : queryTree(bool* sampleVals)
* Purpose : Given a novel sample (bool[]), traverses the tree until a leaf node
* 			is reached, at which point a classification is returned.
* Inputs  : A novel sample's boolean values (bool[])
* Outputs : Boolean classification (bool).
******************************************************************************/
bool DecisionTree::queryTree(bool* sampleVals)
{
	//Set pointer to top
	queryPointer = treeRoot;

	int evalIndex; //Index of feature to evaluate (i.e. which feature is split on at a given node)

	//Traverse until a leaf is hit.
	while(!queryPointer->isLeaf())
	{
		//std::cout << queryPointer->getFeatureIndex() << std::endl;
		evalIndex = queryPointer->getFeatureIndex();
		if (sampleVals[evalIndex] == true)
		{
			queryPointer = queryPointer->getChild(0);
		}
		else
		{
			queryPointer = queryPointer->getChild(1);
		}
	}

	//Evaluate the classification of user sample

	//Does the sample already exist in the set?
	sample* matchedSamplePntr = isInSet(queryPointer, bool2Bitset(sampleVals));
	if (matchedSamplePntr)
	{
		std::cout << "Pre-existing sample found in the data set..." << std::endl;
		sample matchedSample = *matchedSamplePntr;
		delete matchedSamplePntr;

		//Does that single sample type have impurity?
		if (queryPointer->checkForImpureSample(matchedSample, numFeatures))
		{
			return false; //Set to always false for now.
		}

		return matchedSample[numFeatures]; //Return the matched sample's classification
	}
	else
	{
		double trueProportion = queryPointer->calcProportion(numFeatures);

		//std::cout << trueProportion << std::endl;

		if (trueProportion > 0.5)
		{
			return true;
		}
		else if (trueProportion < 0.5)
		{
			return false;
		}
		else
		{
			/*
			//Flip a coin.
			std::srand(time(NULL));
			int result = rand() % 2;
			//std::cout << "Roll result: " << result;
			if (result == 1)
			{
				return true;
			}
			else
			{
				return false;
			}
			*/
			return false;
		}
	}
}

/******************************************************************************
* Name    : bool2Bitset(bool* sampleVals)
* Purpose : Converts a boolean array representation of a sample to its bitset.
* Inputs  : Boolean sample representation (bool[])
* Outputs : Bitset sample representation (sample).
******************************************************************************/
sample DecisionTree::bool2Bitset(bool* sampleVals)
{
	sample output;

	for (int i = 0; i < numFeatures; i++)
	{
		output.push_back(sampleVals[i]);
	}
	return output;
}

/******************************************************************************
* Name    : isInSet(TreeNode* node, sample sampleInput)
* Purpose : Checks if the given sample is in the node's set.
* Inputs  : Sample to check (sample), node to check (TreeNode*)
* Outputs : Pointer to the matched sample and its classification (sample*).
******************************************************************************/
sample* DecisionTree::isInSet(TreeNode* node, sample sampleInput)
{
	sampleList set = node->getSet();

	sampleList::iterator it; //Would it be better to set this as a class member?

	//Count the number of true/false classifications
	for (it = set.begin(); it != set.end(); ++it)
	{
		sample* temp = new sample;
		*temp = *it;
		bool tempClassification = (bool)(*temp)[temp->size() - 1];
		//std::cout << tempClassification << std::endl;

		temp->pop_back();
		if (*temp == sampleInput)
		{
			temp->push_back(tempClassification);
			return temp;
		}
		delete temp;
	}
	return 0;
}

/******************************************************************************
* Name    : printTree(TreeNode* node, int indent)
* Purpose : Recursively prints the tree. X is printed in place of a root node.
* 			The left-most node printed marks the root.
* Inputs  : Node to begin printing from (TreeNode* node), indentation (int)
* Outputs : None.
******************************************************************************/
void DecisionTree::printTree(TreeNode* node, int indent)
{
    if(node != NULL) {

        if(node->getChild(0)) printTree(node->getChild(1), indent+4);

        if (indent) std::cout << std::setw(indent) << ' ';

        if (node->getFeatureIndex() == -1)
        {
        	std::cout << "X" << "\n";
        	return;
        }
        std::cout << "" << node->getFeatureIndex() + 1 << "\n ";

        if(node->getChild(1)) printTree(node->getChild(0), indent+4);
    }
}

/*
void DecisionTree::printTreeV2(TreeNode* node, int depth)
{

}
*/
