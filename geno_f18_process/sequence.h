#ifndef _SEQUENCE_H_
#define _SEQUENCE_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "pattern.h"
#include "types.h"

using namespace std;

class Sequence
{
public:
    Sequence(string f, string h, string o, string p);
    void LoadHomologousAnnotationFile();
    void ProcessData();

private:
    string filesLocation;
    string homologousAnnotationFile;
    string outputLocation;
    string prankExeLocation;
    vector<HomologousAnnotation> homologousAnnotations;

    void ProcessSequence(vector<int>, string);

    void LoadSequenceFiles(vector<int>,
        GenomeSequenceInfo &,
        GenomeSequenceInfo &,
        GenomeSequenceInfo &);

    void ReadSequenceToVector(ifstream &,
        GenomeSequenceInfo &,
        GenomeSequenceInfo &,
        GenomeSequenceInfo &,
        string);

    void RemoveRedundancyAndShort(GenomeSequenceInfo &);

    void CompareAndRemove(GenomeSequenceInfo &,
        GenomeSequenceInfo &,
        GenomeSequenceInfo &,
        string);

    void ExportToFaFile(string fileName, GenomeSequenceInfo &,
        GenomeSequenceInfo &,
        GenomeSequenceInfo &);

    void CombineGenomeSequenceInfo(stringstream &, GenomeSequenceInfo &);

    void RemoveSameShort(GenomeSequenceInfo &);

    void ReadPrankSequenceToVector(ifstream &,
        GenomeSequenceInfo &,
        GenomeSequenceInfo &,
        GenomeSequenceInfo &);

    void RemoveContinuousVariant(GenomeSequenceInfo &,
        GenomeSequenceInfo &,
        GenomeSequenceInfo &);

    void LoadBps(vector<vector<char>> &, GenomeSequenceInfo &, bool isHead);

    void GetBpsFrequency(vector<char>, vector<int> &);

    void RemoveSequence(GenomeSequenceInfo &,
        GenomeSequenceInfo &,
        GenomeSequenceInfo &,
        int);
};

#endif //_SEQUENCE_H_
