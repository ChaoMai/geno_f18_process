#ifndef _SEQUENCE_H_
#define _SEQUENCE_H_

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <sys/stat.h>

#include "pattern.h"
#include "types.h"
#include "param.h"

using namespace std;

extern Param param;

class Sequence
{
public:
	Sequence(Param param);
	void LoadHomologousAnnotationFile();
	void ProcessData();

private:
	string filesLocation;
	string homologousAnnotationFile;
	string outputLocation;

	mutex coutMutex;

	vector<HomologousAnnotation> homologousAnnotations;

	void ProcessSequence(vector<int>, string);

	int LoadSequenceFiles(vector<int>, GenomeSequenceInfo &,
			GenomeSequenceInfo &, GenomeSequenceInfo &);

	int ReadSequenceToVector(ifstream &, GenomeSequenceInfo &,
			GenomeSequenceInfo &, GenomeSequenceInfo &, string);

	void RemoveRedundancyAndShort(GenomeSequenceInfo &);

	void CompareAndRemove(GenomeSequenceInfo &, GenomeSequenceInfo &,
			GenomeSequenceInfo &, string);

	void ExportToFaFile(string fileName, GenomeSequenceInfo &,
			GenomeSequenceInfo &, GenomeSequenceInfo &);

	void CombineGenomeSequenceInfo(stringstream &, GenomeSequenceInfo &);

	void RemoveSameShort(GenomeSequenceInfo &);

	void ReadPrankSequenceToVector(ifstream &, GenomeSequenceInfo &,
			GenomeSequenceInfo &, GenomeSequenceInfo &);

	void RemoveContinuousVariant(GenomeSequenceInfo &, GenomeSequenceInfo &,
			GenomeSequenceInfo &);

	void LoadBps(vector<vector<char>> &, GenomeSequenceInfo &, bool isHead);

	void GetBpsFrequency(vector<char>, vector<int> &);

	void RemoveSequence(GenomeSequenceInfo &, GenomeSequenceInfo &,
			GenomeSequenceInfo &, int);

	bool IsContainsFLH(GenomeSequenceInfo &, GenomeSequenceInfo &,
			GenomeSequenceInfo &);
};

#endif //_SEQUENCE_H_