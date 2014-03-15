#ifndef _MERGE_H_
#define _MERGE_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <dirent.h>
#include <sys/stat.h>

#include "types.h"

class Merge
{
public:
	Merge(string s) :
			patternFolder(s)
	{
	}
	;
	void MergePatternFiles();

private:
	string patternFolder;

	void GetAllFilesNamesWithinFolder(string, vector<string>&);
	void ReadPatterns(vector<string>, vector<PatternFile>&);
	void ExportToPatternFile(vector<PatternFile>&);
};

#endif //_MERGE_H_
