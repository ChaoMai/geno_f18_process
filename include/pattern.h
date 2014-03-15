#ifndef _PATTERN_H_
#define _PATTERN_H_

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>

#include "types.h"
#include "param.h"

extern Param param;

class Pattern
{
public:
	Pattern(string o, string s) :
			outputLocation(o), saveId(s)
	{
	}
	;

	void DifferentiatePattern(GenomeSequenceInfo &, GenomeSequenceInfo &,
			GenomeSequenceInfo &);

private:
	string outputLocation;
	string saveId;
	vector<MultiplePattern> patternsInfo;

	mutex coutMutex;

	void Differentiate(GenomeSequenceInfo &, GenomeSequenceInfo &,
			GenomeSequenceInfo &);

	void ReduceSequence(string, string &);

	int CheckPattern(string);

	void ExportToPatternFile(vector<MultiplePattern>);
};

#endif //_PATTERN_H_
