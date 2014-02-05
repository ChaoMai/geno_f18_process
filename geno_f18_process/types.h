#ifndef _TYPES_H_
#define _TYPES_H_

#include <string>
#include <vector>

using namespace std;

struct HomologousAnnotation
{
    int fileId;
    string genoName;
};

struct GenomeSequence
{
    string sequenceId;
    string sequenceName;
    string sequenceBaseInfo;
    string sequence;
};

typedef struct GenomeSequenceInfo
{
    unsigned int maxiumLength;
    vector<GenomeSequence> Sequences;

    GenomeSequenceInfo() :maxiumLength(0){};
} GenomeSequenceInfo;

typedef struct MultiplePattern
{
    string HName;
    string LName;
    string FName;
    int patternNumber;
}MultiplePattern;

struct PatternsInfo
{
public:
    PatternsInfo() :acti_member(0){};

    void SetActiNumber(int a)
    {
        if ((acti_member == 0 && a == 2) ||
            (acti_member == 2 && a == 2))
        {
            acti_member = 2;
        }
        else
        {
            acti_member = 1;
        }
    };

    int GetActiNumber(){ return acti_member; };

    int patternNubmer;
    vector<MultiplePattern> Patterns;

private:
    int acti_member;
};

#endif //_TYPES_H_
