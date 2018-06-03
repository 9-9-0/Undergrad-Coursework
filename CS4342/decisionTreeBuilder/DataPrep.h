#ifndef DATAPREP_H_
#define DATAPREP_H_

#include <string>
#include <fstream>
#include "Common.h"

class DataPrep
{
private:
	sampleList allSamples;

	std::string dataFileName;
	int featureCount;
	int sampleCount;

	std::string getFileName();
	bool recordSample(std::string sampleStr);
	bool recordCounts(std::string headerString);
	bool isSampleValid(sample sample);
	bool isSetValid(sampleList set);

public:
	DataPrep();

	bool harvestData();
	sampleList getSampleSetList();
	int getFeatureCount() { return featureCount; };
	int getSampleCount() { return sampleCount; };
};



#endif /* DATAPREP_H_ */
