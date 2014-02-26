#ifndef _TYPES_H_
#define _TYPES_H_

#include <string>
#include <vector>

using namespace std;

struct HomologousAnnotation
{
    int fileId;
    string genoName;

    HomologousAnnotation(int f, string g)
        :fileId(f), genoName(g) {}
};

struct GenomeSequence
{
    string sequenceId;
    string sequenceName;
    string sequenceBaseInfo;
    string sequence;

    GenomeSequence(string i, string n, string b, string s)
        :sequenceId(i), sequenceName(n), sequenceBaseInfo(b), sequence(s) {}
};

struct GenomeSequenceInfo
{
    unsigned int maxiumLength;
    vector<GenomeSequence> Sequences;

    GenomeSequenceInfo() :maxiumLength(0){};
};

struct MultiplePattern
{
    string HName;
    string LName;
    string FName;
    int patternNumber;

    MultiplePattern(string h, string l, string f, int p)
        :HName(h), LName(l), FName(f), patternNumber(p) {}
};

struct PatternFile
{
    string fileName;
    string patterns;
};


#endif //_TYPES_H_
