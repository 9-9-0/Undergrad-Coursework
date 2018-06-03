#include "DataPrep.h"
#include "TreeBuilder.h"
#include "DecisionTree.h"

DataPrep* getUserFile();
DecisionTree* buildTree(DataPrep* obj);
void deleteTree(DecisionTree* tree);
bool* getUserSample(int numFeatures);
void treeQuery(DecisionTree* tree, bool* newSample);

int main()
{
	//Prompt User for data file
	DataPrep* resultObj = getUserFile();

	if (!resultObj)
	{
		return 1;
	}

	DecisionTree* tree = buildTree(resultObj); //Build the tree
	bool* temp = NULL;

	//Enter option loop
	int userDecision;
	while (userDecision != 4)
	{
		std::cout << "\nEnter 1 to output the decision tree, enter 2 to query the tree, enter 3 to save the tree, enter 4 to exit: ";
		std::cin >> userDecision;
		while (!std::cin || (userDecision != 1 && userDecision != 2 && userDecision != 3 && userDecision != 4))
		{
			std::cout << "\nInvalid option entered. Try again: ";
			std::cin.clear();
			std::cin.ignore();
			std::cin >> userDecision;
		}
		switch (userDecision)
		{
		case 1:
			tree->printTree(tree->getTreeRoot(), 0);
			break;
		case 2:
			temp = getUserSample(resultObj->getFeatureCount());

			treeQuery(tree, temp);
			delete temp;
			break;
		case 3:
			tree->saveTree("Test.cpp");
			break;
		case 4:
			deleteTree(tree);
			std::cout << "Exiting...\n" << std::endl;
			return 0;
		default:
			return -1;
		}

	}
	return 0;
}


/******************************************************************************
* Name    : getUserFile()
* Purpose : Interfaces with the DataPrep class, attempts to harvest data from
* 			the user input file, notifies failure or success.
* Inputs  : None.
* Outputs : DataPrep pointer, returns a null pointer if unsuccessful.
******************************************************************************/
DataPrep* getUserFile()
{
	DataPrep* output = new DataPrep;

	//Try to harvest data
	bool actionResult = output->harvestData();

	//If successful, return pointer to object.
	if (actionResult)
	{
		std::cout << "\n***********************************" << std::endl;
		std::cout << "Data File Harvest Result: SUCCEEDED" << std::endl;
		std::cout << "***********************************" << std::endl;
		return output;
	}
	//If not successful, delete the memory and return a null pointer.
	else
	{
		std::cout << "\n*********************************" << std::endl;
		std::cout << "Data File Harvest Result: FAILED." << std::endl;
		std::cout << "*********************************" << std::endl;
		std::cout << "\nExiting..." << std::endl;
		delete output;
		return NULL;
	}
}

/******************************************************************************
* Name    : getUserSample(int numFeatures)
* Purpose : Prompts user to enter the values for the feature indexes of a new
* 			sample.
* Inputs  : Number of sample features (int).
* Outputs : Boolean array representation of user input sample.
******************************************************************************/
bool* getUserSample(int numFeatures)
{
	bool* userFeatureVals = new bool[numFeatures];
	int temp;

	std::cout << "\n";

	for (int i = 0; i < numFeatures; i++)
	{
		std::cout << "Enter sample's value for feature " << i + 1 << " (1 for true, 0 for false): ";
		std::cin >> temp;
		while (!std::cin || (temp != 0 && temp != 1))
		{
			std::cout << "Invalid value entered, try again: ";
			std::cin.clear();
			std::cin.ignore();
			std::cin >> temp;
		}
		if (temp == 1)
		{
			userFeatureVals[i] = true;
		}
		else
		{
			userFeatureVals[i] = false;
		}
	}

	return userFeatureVals;
}

/******************************************************************************
* Name    : buildTree(DataPrep* obj)
* Purpose : From the given DataPrep object, uses TreeBuilder to construct a
* 			DecisionTree and returns a pointer to that DecisionTree.
* Inputs  : Pointer to a DataPrep object (DataPrep*)
* Outputs : Pointer to the constructed DecisionTree (DecisionTree*)
******************************************************************************/
DecisionTree* buildTree(DataPrep* obj)
{
	TreeBuilder builderObj(obj->getSampleSetList(), obj->getFeatureCount(), obj->getSampleCount());

	builderObj.constructTree();

	DecisionTree* output = new DecisionTree(builderObj.getTreeRoot(), builderObj.getFeatureCount());

	return output;
}

/******************************************************************************
* Name    : deleteTree(DecisionTree* tree)
* Purpose : Recursively deletes the given tree.
* Inputs  : Pointer to the tree that's to be deleted (DecisionTree*)
* Outputs : None.
******************************************************************************/
void deleteTree(DecisionTree* tree)
{
	tree->recursiveDelete(tree->getTreeRoot());
}

/******************************************************************************
* Name    : treeQuery(DecisionTree* tree, bool* newSample)
* Purpose : Given a decision tree and a boolean representation of a sample to be
* 			queried, the classification of the sample is printed to the console.
* Inputs  : Pointer to a DecisionTree (DecisionTree*), boolean array representation
* 			of sample to be queried.
* Outputs : None.
******************************************************************************/
void treeQuery(DecisionTree* tree, bool* newSample)
{
	std::cout << std::endl;
	bool result = tree->queryTree(newSample);
	std::cout << "Your sample's classification: " << result << std::endl;
}
