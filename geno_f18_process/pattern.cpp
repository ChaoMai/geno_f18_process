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
        patternsInfo.push_back(MultiplePattern(
            HSequence.Sequences.begin()->sequenceName,
            "none",
            FSequence.Sequences.begin()->sequenceName,
            6));
        return;
    }

    //pattern 7
    if ((FSequence.Sequences.size() != 0) &&
        (LSequence.Sequences.size() != 0) &&
        (HSequence.Sequences.size() == 0))
    {
        //return 7;
        patternsInfo.push_back(MultiplePattern(
            "none",
            LSequence.Sequences.begin()->sequenceName,
            FSequence.Sequences.begin()->sequenceName,
            6));
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

                string hName = hItera->sequenceId + "_" + hItera->sequenceName;
                string lName = lItera->sequenceId + "_" + lItera->sequenceName;
                string fName = fItera->sequenceId + "_" + fItera->sequenceName;

                //pattern 9
                if (hStr == lStr && lStr == fStr)
                {
                    //pattern 9
                    patternsInfo.push_back(MultiplePattern(hName, lName, fName, 9));
                    return;
                }

                int mutationPointCount(0);
                string fSeqEncode;

                int hMutationCount(0);
                int lMutationCount(0);
                char preMutatoinSeq = 'n';
                char curMutatoinSeq = 'n';

                string::iterator hStrItera = hStr.begin();
                string::iterator lStrItera = lStr.begin();
                for (string::iterator fStrItera = fStr.begin();
                    fStrItera != fStr.end();
                    fStrItera++, hStrItera++, lStrItera++)
                {
                    if (*fStrItera != *hStrItera && *fStrItera != *lStrItera)
                    {
                        mutationPointCount++;
                        continue;
                    }

                    if (*fStrItera == *hStrItera && *fStrItera != *lStrItera)
                    {
                        lMutationCount++;
                    }

                    if (*fStrItera == *lStrItera && *fStrItera != *hStrItera)
                    {
                        hMutationCount++;
                    }

                    if (fStrItera == fStr.end() - 1)
                    {
                        if (hMutationCount == 0 && lMutationCount > fStr.length() * 0.005)
                        {
                            fSeqEncode.push_back('h');
                        }

                        if (lMutationCount == 0 && hMutationCount > fStr.length() * 0.005)
                        {
                            fSeqEncode.push_back('l');
                        }
                    }

                    if (hMutationCount != 0 && lMutationCount != 0)
                    {
                        if (hMutationCount / lMutationCount >= 3 && hMutationCount * lMutationCount != 1)
                        {
                            fSeqEncode.push_back('l');
                            lMutationCount = 0;
                            hMutationCount = 0;
                        }
                        else if (lMutationCount / hMutationCount >= 3 && hMutationCount * lMutationCount != 1)
                        {
                            fSeqEncode.push_back('h');
                            lMutationCount = 0;
                            hMutationCount = 0;
                        }
                    }
                }

                if ((mutationPointCount / fStr.length()) <= 0.05)
                {
                    string reducedStr;
                    ReduceSequence(fSeqEncode, reducedStr);
                    int patternNumber = CheckPattern(reducedStr);

                    cout << fSeqEncode << " " << reducedStr << endl;

                    patternsInfo.push_back(MultiplePattern(hName, lName, fName, patternNumber));
                }
                else
                {
                    //pattern 8
                    patternsInfo.push_back(MultiplePattern(hName, lName, fName, 8));
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

        if (reducedStr.length() != 0)
        {
            if (reducedStr.back() != tmpReducedStr[0])
            {
                reducedStr.push_back(tmpReducedStr[0]);
            }
        }
        else
        {
            reducedStr.push_back(tmpReducedStr[0]);
        }

        tmpReducedStr.clear();

        preCh = curCh;
    }

    //int hCount(0);
    //int lCount(0);

    //for (char ch : fSeqEncode)
    //{
    //    if (ch == 'h')
    //    {
    //        hCount++;
    //    }
    //    else if (ch == 'l')
    //    {
    //        lCount++;
    //    }
    //}

    //if (hCount <= fSeqEncode.length() * 0.01)
    //{
    //    reducedStr.push_back('l');
    //    return;
    //}
    //else if (lCount <= fSeqEncode.length() * 0.01)
    //{
    //    reducedStr.push_back('h');
    //    return;
    //}

    //stringstream seSStr(fSeqEncode);

    ////read first character
    //char preCh;
    //char curCh;
    //seSStr >> preCh;

    //while (preCh == 'n')
    //{
    //    seSStr >> preCh;
    //}

    //string tmpReducedStr;

    //while (!seSStr.eof())
    //{
    //    seSStr >> curCh;

    //    if (curCh == 'n')
    //    {
    //        continue;
    //    }

    //    tmpReducedStr.push_back(preCh);

    //    if (preCh != curCh || seSStr.eof())
    //    {
    //        if (tmpReducedStr.length() <= 1)
    //        {
    //            string remainingStr;

    //            streamoff pos(0);
    //            bool isSetPos = false;

    //            if (!seSStr.eof())
    //            {
    //                pos = seSStr.tellg();
    //                isSetPos = true;
    //            }

    //            getline(seSStr, remainingStr);

    //            if (remainingStr.length() == 0)
    //            {
    //                break;
    //            }

    //            unsigned int distance(1);

    //            while (distance < 200)
    //            {
    //                if (distance >= remainingStr.length() || remainingStr[distance] == tmpReducedStr[0])
    //                {
    //                    break;
    //                }

    //                distance++;
    //            }

    //            if (distance < 200)
    //            {
    //                if (reducedStr.length() != 0)
    //                {
    //                    if (reducedStr.back() != tmpReducedStr[0])
    //                    {
    //                        reducedStr.push_back(tmpReducedStr[0]);
    //                    }
    //                }
    //                else
    //                {
    //                    reducedStr.push_back(tmpReducedStr[0]);
    //                }
    //            }

    //            if (isSetPos)
    //            {
    //                seSStr.seekg(pos);
    //            }

    //            tmpReducedStr.clear();
    //        }
    //        else
    //        {
    //            if (reducedStr.length() != 0)
    //            {
    //                if (reducedStr.back() != tmpReducedStr[0])
    //                {
    //                    reducedStr.push_back(tmpReducedStr[0]);
    //                }
    //            }
    //            else
    //            {
    //                reducedStr.push_back(tmpReducedStr[0]);
    //            }

    //            tmpReducedStr.clear();
    //        }
    //    }

    //    preCh = curCh;
    //}
}

int Pattern::CheckPattern(string reducedStr)
{
    int length = reducedStr.length();

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
        else if (reducedStr[0] = 'l' && reducedStr[1] == 'h')
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
    return 5;
}

void Pattern::ExportToPatternFile(vector<MultiplePattern> info)
{
    string fileName = outputLocation + "pattern_hebing_" + saveId;
    cout << "saving pattern info " + saveId << endl;

    ofstream ofs;
    ofs.open(fileName, ios::out);

    if (!ofs)
    {
        cerr << "fatal error: failed to open file pattern_hebing_" << saveId << " to write" << endl;
        exit(1);
    }

    stringstream sstr;


    sstr << "pattern_id\t" << "h_name\t" << "l_name\t" << "f_name\t" << "pattern" << endl;

    for (vector<MultiplePattern>::iterator itera = info.begin();
        itera != info.end(); itera++)
    {
        sstr << itera - info.begin() + 1 << '\t'
            << itera->HName << '\t'
            << itera->LName << '\t'
            << itera->FName << '\t'
            << itera->patternNumber << endl;
    }

    ofs << sstr.rdbuf();
    ofs.close();
}
