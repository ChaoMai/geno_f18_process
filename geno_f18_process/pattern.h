#ifndef _PATTERN_H_
#define _PATTERN_H_

#include <fstream>
#include <iostream>
#include <sstream>

#include "types.h"

class Pattern
{
public:
    Pattern(string o, string s) :outputLocation(o), saveId(s){};

    void DifferentiatePattern(GenomeSequenceInfo &,
        GenomeSequenceInfo &,
        GenomeSequenceInfo &);

private:
    string outputLocation;
    string saveId;
    PatternsInfo patternsInfo;

    void Differentiate(GenomeSequenceInfo &,
        GenomeSequenceInfo &,
        GenomeSequenceInfo &);

    void ReduceSequence(string, string &);

    int CheckPattern(string);

    void ExportToPatternFile(PatternsInfo);
};

#endif //_PATTERN_H_
