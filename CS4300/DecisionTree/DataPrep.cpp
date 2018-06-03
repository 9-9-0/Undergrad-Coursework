#include "DataPrep.h"

/******************************************************************************
* Name    : DataPrep()
* Purpose : Calls getFileName() and sets the instance variable to its output.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
DataPrep::DataPrep()
{
	dataFileName = getFileName();
}

/******************************************************************************
* Name    : getFileName()
* Purpose : Prompts the user to enter in the name of the data file.
* Inputs  : None.
* Outputs : Returns a string containing the user entered filename.
******************************************************************************/
std::string DataPrep::getFileName()
{
	//Get filename
	std::string fileName;

	std::cout << "Enter filename: ";
	std::getline(std::cin, fileName);

	return fileName;
}

/******************************************************************************
* Name    : harvestData()
* Purpose : Attempts to harvest the data file. If successful, harvested samples
* 			are stored in the allSamples variable and true is returned. If not,
* 			false is returned.
* Inputs  : None.
* Outputs : True if successful, false if failed.
* Notes	  :
* 			- Extensive error checking needs to be added.
******************************************************************************/
bool DataPrep::harvestData()
{
	bool isSuccessful = true;

	//Open file
	std::fstream dataFile(dataFileName.std::string::c_str(), std::ios::in);

	//Set # of Features, # of Samples
	std::string headerLine;
	std::string oneSample;

	//Begin Sample Harvest
	if (dataFile.is_open())
	{
		std::getline(dataFile, headerLine);

		//std::cout << headerLine << std::endl;
		recordCounts(headerLine); //Add in error-checking at some point.

		while (!dataFile.eof() && isSuccessful)
		{
			std::getline(dataFile, oneSample);
			isSuccessful = recordSample(oneSample);
		}
	}
	else
	{
		return false;
	}

	//Close file
	dataFile.close();

	//An incorrect field in a sample.
	if (!isSuccessful)
	{
		return false;
	}

	isSuccessful = isSetValid(allSamples);

	/*
	std::cout << sampleCount << std::endl;
	std::cout << featureCount << std::endl;
	std::cout << allSamples.size() << std::endl;
	*/

	return isSuccessful;

}

/******************************************************************************
* Name    : getSampleSetList()
* Purpose : Returns the allSamples variable.
* Inputs  : None.
* Outputs : List of all samples harvested (sampleList)
******************************************************************************/
sampleList DataPrep::getSampleSetList()
{
	return allSamples;
}

/******************************************************************************
* Name    : recordCounts()
* Purpose : Based on the data file's first line, parses and stores the number of
* 			features and the number of samples.
* Inputs  : None.
* Outputs : True by default.
* Notes	  :
* 			- Add in error-checking for misformatted lines.
******************************************************************************/
bool DataPrep::recordCounts(std::string headerString)
{
	//Tokenization Variables
	size_t pos = 0;
	std::string token;
	std::string delimiter = " ";

	int extractCount = 0;

	//Make more versatile, I.e. allow for schema other than #_#_ (underscore interpreted as space)
	while ((pos = headerString.find(delimiter)) != std::string::npos && extractCount < 3) {
	    token = headerString.substr(0, pos);
	    //std::cout << "Token " << token << std::endl;
	    if (extractCount == 0)
	    {
	    	featureCount = std::atoi(token.c_str());
	    }
	    else
	    {
	    	sampleCount = std::atoi(token.c_str());
	    }

	    headerString.erase(0, pos + delimiter.length());
	    extractCount++;
	}

	//Returns true on default, currently does not detect misformatted files.
	return true;
}

/******************************************************************************
* Name    : recordSample(string sampleStr)
* Purpose : Given a string, representing a sample, this function parses it,
* 			creates a sample from it, and pushes the sample into the set.
* Inputs  : Sample string (sampleStr)
* Outputs : True if sample is recorded successfully, false otherwise.
******************************************************************************/
bool DataPrep::recordSample(std::string sampleStr)
{
	sample output;

	//Tokenization Variables
	size_t pos = 0;
	std::string token;
	std::string delimiter = " ";

	//Gets the value for every feature of a sample
	while ((pos = sampleStr.find(delimiter)) != std::string::npos) {
	    token = sampleStr.substr(0, pos);
	    //std::cout << token << std::endl;
	    if (token == "1")
	    {
	    	output.push_back(1);
	    }
	    else
	    {
	    	output.push_back(0);
	    }
	    sampleStr.erase(0, pos + delimiter.length());
	}

	//Gets result of a sample (last character left)
	if (sampleStr == "1")
	{
		output.push_back(1);
	}
	if (sampleStr == "0")
	{
		output.push_back(0);
	}

	/* Console Debug Lines
	for (int i = 0; i < output.size(); i++)
	{
		std::cout << output[i] << " ";
	}
	*/

	if (isSampleValid(output))
	{
		allSamples.push_back(output);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
* Name    : isSampleValid(sample<> sample)
* Purpose : Checks if the given sample's size is valid.
* Inputs  : Sample (sample).
* Outputs : True or false.
******************************************************************************/
bool DataPrep::isSampleValid(sample sample)
{
	//featureCount + 1 because of the result column
	if (sample.size() == featureCount + 1)
	{
		return true;
	}

	return false;
}

/******************************************************************************
* Name    : isSetValid(sample<> set)
* Purpose : Checks if the sample set's size is valid.
* Inputs  : Set (sampleList).
* Outputs : True or false.
******************************************************************************/
bool DataPrep::isSetValid(sampleList set)
{
	if (set.size() == sampleCount)
	{
		return true;
	}

	return false;
}

