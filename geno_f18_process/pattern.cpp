#include "pattern.h"

void Pattern::DifferentiatePattern(GenomeSequenceInfo &FSequence,
    GenomeSequenceInfo &HSequence,
    GenomeSequenceInfo &LSequence)
{
    Differentiate(FSequence, HSequence, LSequence);
    ExportToPatternFile(patternsInfo);
}

void Pattern::Differentiate(GenomeSequenceInfo &FSequence,
    GenomeSequenceInfo &HSequence,
    GenomeSequenceInfo &LSequence)
{
    //pattern 6
    if ((FSequence.Sequences.size() != 0) &&
        (HSequence.Sequences.size() != 0) &&
        (LSequence.Sequences.size() == 0))
    {
        //return 6;
        patternsInfo.SetActiNumber(1);
        patternsInfo.patternNubmer = 6;
        return;
    }

    //pattern 7
    if ((FSequence.Sequences.size() != 0) &&
        (LSequence.Sequences.size() != 0) &&
        (HSequence.Sequences.size() == 0))
    {
        //return 7;
        patternsInfo.SetActiNumber(1);
        patternsInfo.patternNubmer = 7;
        return;
    }

    for (vector<GenomeSequence>::iterator hItera = HSequence.Sequences.begin();
        hItera != HSequence.Sequences.end(); hItera++)
    {
        string hStr = hItera->sequence;

        for (vector<GenomeSequence>::iterator lItera = LSequence.Sequences.begin();
            lItera != LSequence.Sequences.end(); lItera++)
        {
            string lStr = lItera->sequence;

            for (vector<GenomeSequence>::iterator fItera = FSequence.Sequences.begin();
                fItera != FSequence.Sequences.end(); fItera++)
            {
                string fStr = fItera->sequence;

                //pattern 9
                if (hStr == lStr && lStr == fStr)
                {
                    //pattern 9
                    patternsInfo.SetActiNumber(1);
                    patternsInfo.patternNubmer = 9;
                    return;
                }

                float mutationPointCount(0);
                string fSeqEncode;

                string::iterator hStrItera = hStr.begin();
                string::iterator lStrItera = lStr.begin();
                for (string::iterator fStrItera = fStr.begin();
                    fStrItera != fStr.end();
                    fStrItera++, hStrItera++, lStrItera++)
                {
                    if (*fStrItera == *hStrItera)
                    {
                        fSeqEncode.push_back('h');
                    }
                    else if (*fStrItera == *lStrItera)
                    {
                        fSeqEncode.push_back('l');
                    }
                    else
                    {
                        mutationPointCount++;
                        fSeqEncode.push_back('n');
                    }
                }

                if ((mutationPointCount / fStr.size()) <= 0.2)
                {
                    string reducedStr;
                    ReduceSequence(fSeqEncode, reducedStr);

                    int patternNumber = CheckPattern(reducedStr);

                    switch (patternNumber)
                    {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    {
                        //pattern 1~5
                        string hName = hItera->sequenceId + "_" + hItera->sequenceName;
                        string lName = lItera->sequenceId + "_" + lItera->sequenceName;
                        string fName = fItera->sequenceId + "_" + fItera->sequenceName;
                        patternsInfo.SetActiNumber(2);
                        patternsInfo.Patterns.push_back({ hName, lName, fName, patternNumber });
                        break;
                    }
                    case 6:
                    case 7:
                    {
                        //pattern 6~7
                        patternsInfo.SetActiNumber(1);
                        patternsInfo.patternNubmer = patternNumber;
                        return;
                    }
                    }
                }
                else
                {
                    //pattern 8
                    string hName = hItera->sequenceId + "_" + hItera->sequenceName;
                    string lName = lItera->sequenceId + "_" + lItera->sequenceName;
                    string fName = fItera->sequenceId + "_" + fItera->sequenceName;
                    patternsInfo.SetActiNumber(2);
                    patternsInfo.Patterns.push_back({ hName, lName, fName, 8 });
                }
            }
        }
    }
}

void Pattern::ReduceSequence(string fSeqEncode, string &reducedStr)
{
    stringstream seSStr(fSeqEncode);

    //read first character
    char preCh;
    char curCh;
    seSStr >> preCh;

    while (preCh == 'n')
    {
        seSStr >> preCh;
    }

    string tmpReducedStr;
    while (!seSStr.eof())
    {
        seSStr >> curCh;

        if (curCh == 'n')
        {
            continue;
        }

        tmpReducedStr.push_back(preCh);

        if (preCh != curCh || seSStr.eof())
        {
            if (tmpReducedStr.size() <= 2)
            {
                tmpReducedStr.clear();
                preCh = curCh;
                continue;
            }
            else
            {
                if (reducedStr.size() != 0)
                {
                    if (reducedStr.back() == tmpReducedStr[0])
                    {
                        tmpReducedStr.clear();
                        preCh = curCh;
                        continue;
                    }
                    else
                    {
                        reducedStr.push_back(tmpReducedStr[0]);
                        tmpReducedStr.clear();
                        preCh = curCh;
                    }
                }
                else
                {
                    reducedStr.push_back(tmpReducedStr[0]);
                    tmpReducedStr.clear();
                    preCh = curCh;
                }
            }
        }
    }
}

int Pattern::CheckPattern(string reducedStr)
{
    int length = reducedStr.size();

    if (length <= 0)
    {
        cerr << "fatal error: reducing error" << endl;
        system("pause");
    }

    if (length == 1)
    {
        //pattern 6
        if (reducedStr[0] == 'h')
        {
            return 6;
        }
        //pattern 7
        else if (reducedStr[0] = 'l')
        {
            return 7;
        }
    }
    else if (length == 2)
    {
        //pattern 1
        if (reducedStr[0] == 'h' && reducedStr[1] == 'l')
        {
            return 1;
        }
        //pattern 2
        else if (reducedStr[0] = 'l'&&reducedStr[1] == 'h')
        {
            return 2;
        }
    }
    else if (length == 3)
    {
        //pattern 4
        if (reducedStr[0] == 'h' && reducedStr[1] == 'l'&&reducedStr[2] == 'h')
        {
            return 4;
        }
        //pattern 3
        else if (reducedStr[0] = 'l'&&reducedStr[1] == 'h'&&reducedStr[2] == 'l')
        {
            return 3;
        }
    }
    //else if (length >= 4)
    //{
    //    return 5;
    //}
    return 5;
}

void Pattern::ExportToPatternFile(PatternsInfo info)
{
    string fileName = outputLocation + "pattern_hebing_" + saveId;
    cout << "saving pattern info " + saveId << endl;

    ofstream ofs;
    ofs.open(fileName, ios::out);

    if (!ofs)
    {
        cerr << "fatal error: failed to open file to write" << endl;
        system("pause");
    }

    stringstream sstr;

    switch (info.GetActiNumber())
    {
    case 1:
    {
        sstr << "pattern_id\t" << "pattern" << endl
            << "1\t" << info.patternNubmer << endl;
        break;
    }
    case 2:
    {
        sstr << "pattern_id\t" << "h_name\t" << "l_name\t" << "f_name\t" << "pattern" << endl;

        for (vector<MultiplePattern>::iterator itera = info.Patterns.begin();
            itera != info.Patterns.end(); itera++)
        {
            sstr << itera - info.Patterns.begin() + 1 << '\t'
                << itera->HName << '\t'
                << itera->LName << '\t'
                << itera->FName << '\t'
                << itera->patternNumber << endl;
        }

        break;
    }
    default:
    {
        cerr << "acti_member isn't seted!" << endl;
        system("pause");
        break;
    }
    }

    ofs << sstr.rdbuf();
    ofs.close();
}
