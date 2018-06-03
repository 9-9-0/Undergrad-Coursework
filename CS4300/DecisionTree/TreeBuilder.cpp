#include "TreeBuilder.h"
#include "TreeNode.h"

/******************************************************************************
* Name    : TreeBuilder(sampleList set, int featureCount, int sampleCount)
* Purpose : Instantiates a TreeBuilder object with the given inputs.
* Inputs  : The original sample set (sampleList), the number of features for
* 			each sample (int), and the number of samples (int)
* Outputs : None.
******************************************************************************/
TreeBuilder::TreeBuilder(sampleList set, int featureCount, int sampleCount)
{
	originalSet = set;
	treeRoot = NULL;

	this->featureCount = featureCount;
	this->sampleCount = sampleCount;

	//Set the available features to split on
	for (int i = 0; i < featureCount; i++)
	{
		origAvailFeatureIndexes.push_back(i);
	}
}

/******************************************************************************
* Name    : constructTree()
* Purpose : Creates the root node with the variables set by the constructor and
* 			calls recursiveConstruct() on the root.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
void TreeBuilder::constructTree()
{
	treeRoot = createNode(originalSet, calcEntropy(originalSet), origAvailFeatureIndexes);

	recursiveConstruct(treeRoot);
}

/******************************************************************************
* Name    : belongsTo(sample sample, int featureIndex)
* Purpose : Returns true or false based on whether or not the given sample
* 			contains the given feature index.
* Inputs  : Sample (sample) and feature index (int) to evaluate.
* Outputs : True or false (bool).
******************************************************************************/
bool TreeBuilder::belongsTo(sample sample, int featureIndex)
{
	if (sample[featureIndex] == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
* Name    : recursiveConstruct(TreeNode* node)
* Purpose : Recursively builds the decision tree.
* Inputs  : A pointer to the node from which to begin construction (TreeNode*)
* Outputs : None.
******************************************************************************/
void TreeBuilder::recursiveConstruct(TreeNode* node)
{
	//std::cout << "Set size: " << node->getSet().size() << std::endl;

	//Are there features left to split on?
	if (node->getAvailFeatureIndexes().size() == 0)
	{
		node->setLeaf();
		//std::cout << "Hit leaf node, returning..." << std::endl;
		return;
	}

	//Get the best feature to split on
	int bestFeature = getBestFeature2Split(node);

	//Do any of them offer the minimum information gain?
	if (bestFeature == -1)
	{
		node->setLeaf();
		//std::cout << "Hit leaf node, returning..." << std::endl;
		return;
	}

	/*
	std::cout << "Best Feature: " << bestFeature << std::endl;
	std::cout << "Splitting on..." << bestFeature << "\n" << std::endl;
	*/

	//Set the node's featureIndex variable to the bestFeature that was just found.
	node->setFeature2Split(bestFeature);

	//Remove that feature from the vector of available features to split on
	intVect childAvailFeatures = node->getAvailFeatureIndexes();
	childAvailFeatures.erase(std::remove(childAvailFeatures.begin(), childAvailFeatures.end(), bestFeature), childAvailFeatures.end());

	//Split the set and calculate their entropies
	sampleList* temp = split(node->getSet(), bestFeature);
	double trueSetEntropy = calcEntropy(temp[0]);
	double falseSetEntropy = calcEntropy(temp[1]);

	//Create the children node and set them as node's children
	node->setChildren(0, createNode(temp[0], trueSetEntropy, childAvailFeatures));
	node->setChildren(1, createNode(temp[1], falseSetEntropy, childAvailFeatures));

	delete [] temp; //Free up the memory

	//Run recursiveConstruct on children
	recursiveConstruct(node->getChild(0));
	recursiveConstruct(node->getChild(1));
}

/******************************************************************************
* Name    : createNode(sampleList set, double entropy, intVect availFeatures)
* Purpose : Constructs a tree node with the given inputs and returns a pointer to
* 			the node.
* Inputs  : Set of samples after splitting (set), entropy of that set (double),
* 			and available features to split on (intVect).
* Outputs : Pointer to the new node (TreeNode*)
******************************************************************************/
TreeNode* TreeBuilder::createNode(sampleList set, double entropy, intVect availFeatures)
{
	TreeNode* output = new TreeNode(set, entropy, availFeatures);

	return output;
}

/******************************************************************************
* Name    : split(sampleList set, int featureIndex)
* Purpose : Splits the input set into two sets, one of samples containing the
* 			featureIndex, one of samples not containing the featureIndex.
* Inputs  : Set and index on which to split it.
* Outputs : Pointer to a sampleList[2]. sampleList[0] represents the set of
* 			samples containing a TRUE valued featureIndex, sampleList[1] the other set.
******************************************************************************/
sampleList* TreeBuilder::split(sampleList set, int featureIndex)
{
	sampleList::iterator it; //Would it be better to set this as a class member?

	sampleList* output = new sampleList[2]; //Index 0 for samples with feature value TRUE, 1 for samples with feature value FALSE

	for (it = set.begin(); it != set.end(); ++it)
	{
		//Check if sample has the feature specified
	    if (belongsTo(*it, featureIndex))
	    {
	    	output[0].push_back(*it);
	    }
	    else
	    {
	    	output[1].push_back(*it);
	    }
	}

	/*
	std::cout << "Index: " << featureIndex << std::endl;
	std::cout << "Size of TRUE split: " << output[1].size() << std::endl;
	std::cout << "Size of FALSE split: " << output[0].size() << std::endl;
	*/

	return output;
}

/******************************************************************************
* Name    : getBestFeature2Split(TreeNode* node)
* Purpose : Given a node, from its available featureIndexes to split on, find
* 			the featureIndex with the greatest information gain.
* Inputs  : A node (TreeNode*).
* Outputs : A featureIndex (int).
******************************************************************************/
int TreeBuilder::getBestFeature2Split(TreeNode* node)
{
	double maxIG = 0;
	int bestFeature = -1;
	double temp;

	intVect tempVect = node->getAvailFeatureIndexes();

	for (int i = 0; i < tempVect.size(); i++)
	{
		//std::cout << "Index: " << tempVect[i] << std::endl;
		temp = IGOnSplit(node->getSet(), tempVect[i]);
		//std::cout << "TEMP: " << temp << std::endl;

		if (temp > maxIG)
		{
			maxIG = temp;
			bestFeature = tempVect[i];
		}
	}

	//No improvements made, return -1
	if (maxIG == 0)
	{
		return -1;
	}

	//std::cout << "MAX IG: " << maxIG << std::endl;

	return bestFeature;
}

/******************************************************************************
* Name    : IGOnSplit(sampleList set, int feature)
* Purpose : Given a set of samples and a feature to split on, calculate the
* 			information gain of splitting on said feature.
* Inputs  : Set of samples (sampleList) and feature (int) to split on.
* Outputs : Information gain value (double).
******************************************************************************/
double TreeBuilder::IGOnSplit(sampleList set, int feature)
{
	double IniEntropy = calcEntropy(set); //Calculate entropy of parent set.

	//std::cout << "IniEntropy: " << IniEntropy << std::endl;

	sampleList* temp = split(set, feature); //Split on feature

	/*//More straightforward calculation of proportions, salvaging the calcProportion() function.
	double p1 = (double)temp[0].size() / (double)set.size(); //Proportion of samples with feature value TRUE
	double p2 = (double)temp[1].size() / (double)set.size(); //Proportion of samples with feature value FALSE
	*/

	double p1 = calcProportion(set, feature); //Proportion of samples with feature value TRUE
	double p2 = 1 - p1;						  //Proportion of samples with feature value FALSE

	/*
	std::cout << "p1: " << p1 << std::endl;
	std::cout << "p2: " << p2 << std::endl;
	std::cout << "ENTROPY 1: " << calcEntropy(temp[1]) << std::endl;
	std::cout << "ENTROPY 2: " << calcEntropy(temp[0]) << std::endl;
	*/

	double PostEntropy = calcEntropy(temp[0]) * p1  + calcEntropy(temp[1]) * p2;

	double IG = IniEntropy - PostEntropy;

	//std::cout << "Info gain: " << IG << std::endl;

	delete [] temp;
	return IG;
}

/******************************************************************************
* Name    : calcProportion(sampleList set, int feature)
* Purpose : Returns the proportion of samples with feature value of TRUE
* Inputs  : Set of samples, feature index for calculating proportions.
* 			featureIndex is the feature # - 1. For Feature 2, featureIndex is 1.
* Outputs : Proportion of samples with feature value TRUE, -1 if invalid
* 			feature index is passed in.
******************************************************************************/
double TreeBuilder::calcProportion(sampleList set, int featureIndex)
{
	//Return -1 if invalid featureIndex...this should never be returned.
	if (featureIndex > featureCount - 1)
	{
		return -1;
	}
	sampleList::iterator it; //Would it be better to set this as a class member?

	int trueCount = 0;
	int totalCount = 0;
	double trueProp;

	//Count the number of samples with TRUE @ the featureIndex and the total count of samples
	for (it = set.begin(); it != set.end(); ++it)
	{
		//Check if sample has the feature specified
		if (belongsTo(*it, featureIndex))
		{
			trueCount++;
			totalCount++;
		}
		else
		{
			totalCount++;
		}
	}

	//Calculate the proportion of TRUE feature valued samples.
	trueProp = double(trueCount) / double(totalCount);

	return trueProp;
}

/******************************************************************************
* Name    : calcEntropy(sampleList set)
* Purpose : Calculates the entropy of set passed in.
* Inputs  : Set (sampleList) whose entropy is to be calculated.
* Outputs : The set's entropy (double).
******************************************************************************/
double TreeBuilder::calcEntropy(sampleList set)
{
	sampleList::iterator it; //Would it be better to set this as a class member?

	int trueCount = 0; //Winner, Winner.
	int falseCount = 0;

	//Count the number of true/false classifications
	for (it = set.begin(); it != set.end(); ++it)
	{
		//Check if last bit (the sample classification) is 1 or 0
	    if (belongsTo(*it, featureCount))
	    {
	    	trueCount++;
	    }
	    else
	    {
	    	falseCount++;
	    }
	}

	/*
	std::cout << "True Count " << trueCount << std::endl;
	std::cout << "False Count " << falseCount << std::endl;
	*/

	if (trueCount == 0 || falseCount == 0)
	{
		return 0;
	}

	double total = (double)(set.size());

	//Determine Proportions
	double p1 = (double)(trueCount) / total;
	double p2 = (double)(falseCount) / total;

	//Calculate Set Entropy
	double setEntropy = - p1 * log2(p1) - p2 * log2(p2);

	return setEntropy;
}

