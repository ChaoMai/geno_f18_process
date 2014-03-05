#include "sequence.h"

Sequence::Sequence(string f, string h, string o, string p)
:filesLocation(f), homologousAnnotationFile(h), outputLocation(o), prankExeLocation(p)
{
    wstring wStr = wstring(outputLocation.begin(), outputLocation.end());
    cout << "create output folder: \"" << outputLocation << "\"" << endl;
    _wmkdir(wStr.c_str());

    filesLocation.append("\\");
    outputLocation.append("\\");
}

void Sequence::LoadHomologousAnnotationFile()
{
    string fileName = homologousAnnotationFile;
    ifstream ifs;

    ifs.open(fileName.c_str());

    if (!ifs)
    {
        cerr << "fatal error: failed to open homologous annotation file" << endl;
        exit(1);
    }

    cout << "loading homologous annotation file. " << endl;

    int tmp = 0;
    while (!ifs.eof())
    {
        tmp++;
        char chs[200];

        int id;
        string name;
        string str;

        ifs.getline(chs, 199, ',');
        if (strlen(chs) == 0)
        {
            break;
        }
        id = atoi(chs);

        ifs.getline(chs, 199, ',');
        if (strlen(chs) == 0)
        {
            break;
        }
        name = chs;
        getline(ifs, str);

        auto insertAnnotation =
            [this](HomologousAnnotation anno)
        {
            int hIndex = 0;
            int tIndex = distance(homologousAnnotations.begin(), homologousAnnotations.end()) - 1;
            int midIndex = 0;

            while (hIndex <= tIndex)
            {
                midIndex = (hIndex + tIndex) / 2;

                if (anno.genoName > next(homologousAnnotations.begin(), midIndex)->genoName)
                {
                    hIndex = midIndex + 1;
                }
                else if (anno.genoName < next(homologousAnnotations.begin(), midIndex)->genoName)
                {
                    tIndex = midIndex - 1;
                }
                else
                {
                    break;
                }

            }

            homologousAnnotations.insert(next(homologousAnnotations.begin(), midIndex), anno);
        };

        insertAnnotation(HomologousAnnotation(id, name));
    }

    ifs.close();
}

void Sequence::ProcessData()
{
    vector<int> ids;
    string genoName;

    int preId = homologousAnnotations.begin()->fileId;
    string preName = homologousAnnotations.begin()->genoName;
    genoName = preName;
    int count(0);
    ids.push_back(preId);

    for (vector<HomologousAnnotation>::iterator itera = homologousAnnotations.begin() + 1;
        itera != homologousAnnotations.end(); itera++, count++)
    {
        string curName = itera->genoName;

        if (preName == curName)
        {
            ids.push_back(itera->fileId);
        }
        else
        {
            ProcessSequence(ids, genoName);
            genoName = curName;
            preName = curName;
            ids.clear();
            ids.push_back(itera->fileId);
        }
    }
}

void Sequence::ProcessSequence(vector<int> ids, string genoName)
{
    GenomeSequenceInfo FSequences;
    GenomeSequenceInfo HSequences;
    GenomeSequenceInfo LSequences;

    //combine files
    int loadStatus = LoadSequenceFiles(ids, FSequences, HSequences, LSequences);

    if (loadStatus == 1)
    {
        return;
    }

    //Remove Redundancy and Short
    RemoveRedundancyAndShort(FSequences);
    RemoveRedundancyAndShort(HSequences);
    RemoveRedundancyAndShort(LSequences);

    //compare
    //remove the one has gap but its information is already include in bigger one
    //remove 10bp in head and tail
    char chs[200];
    _itoa_s(ids[0], chs, 10);
    string saveId(chs);
    CompareAndRemove(FSequences, HSequences, LSequences, saveId);
}

int Sequence::LoadSequenceFiles(vector<int> ids,
    GenomeSequenceInfo &FSequence,
    GenomeSequenceInfo &HSequence,
    GenomeSequenceInfo &LSequence)
{
    for (vector<int>::iterator itera = ids.begin(); itera != ids.end(); itera++)
    {
        char chs[50];
        _itoa_s(*itera, chs, 10);
        string fileId(chs);
        string fileName = filesLocation + "hebing_" + fileId + ".fa";
        cout << "load hebing_" + fileId + ".fa" << endl;

        ifstream ifs;
        ifs.open(fileName);

        if (!ifs)
        {
            cerr << "error: failed to open sequence file " << fileId << endl;
            return 1;
        }

        ReadSequenceToVector(ifs, FSequence, HSequence, LSequence, fileId);
        ifs.close();
    }

    return 0;
}

void Sequence::ReadSequenceToVector(ifstream &ifs,
    GenomeSequenceInfo &FSequence,
    GenomeSequenceInfo &HSequence,
    GenomeSequenceInfo &LSequence,
    string fileId)
{
    while (!ifs.eof())
    {
        char ch;
        string tmpId;
        string tmpName;
        string tmpBaseInfo;
        string tmpSequence;

        ifs >> ch;
        if (ch != '>')
        {
            break;
        }
        ifs >> ch;
        ifs.unget();

        ifs >> tmpId;
        ifs >> tmpName;
        getline(ifs, tmpBaseInfo);

        while (true)
        {
            string tmpStr;
            ifs >> ch;

            if (ch == '>' || ifs.eof())
            {
                ifs.unget();
                break;
            }

            ifs.unget();
            ifs >> tmpStr;
            tmpSequence += tmpStr;
        }

        char sequenceType = *tmpId.begin();

        switch (sequenceType)
        {
        case'f':
        {
            if (tmpSequence.length() > FSequence.maxiumLength)
            {
                FSequence.maxiumLength = tmpSequence.length();
            }
            FSequence.Sequences.push_back(GenomeSequence(tmpId, tmpName, tmpBaseInfo, tmpSequence));
            break;
        }
        case'h':
        {
            if (tmpSequence.length() > HSequence.maxiumLength)
            {
                HSequence.maxiumLength = tmpSequence.length();
            }
            HSequence.Sequences.push_back(GenomeSequence(tmpId, tmpName, tmpBaseInfo, tmpSequence));
            break;
        }
        case'l':
        {
            if (tmpSequence.length() > LSequence.maxiumLength)
            {
                LSequence.maxiumLength = tmpSequence.length();
            }
            LSequence.Sequences.push_back(GenomeSequence(tmpId, tmpName, tmpBaseInfo, tmpSequence));
            break;
        }
        default:
            cerr << "fatal error: error occoured in reading genome file " << fileId << endl;
            exit(1);
        }
    }
}

void Sequence::RemoveRedundancyAndShort(GenomeSequenceInfo &Seq)
{
    for (vector<GenomeSequence>::iterator mainItera = Seq.Sequences.begin();
        mainItera != Seq.Sequences.end(); mainItera++)
    {
        string mainStr = mainItera->sequence;

        for (vector<GenomeSequence>::iterator compItera = mainItera + 1;
            compItera != Seq.Sequences.end();)
        {
            string compStr = compItera->sequence;

            if ((mainStr == compStr) || (compStr.length() <= Seq.maxiumLength * 0.75))
            {
                compItera = Seq.Sequences.erase(compItera);
            }
            else
            {
                compItera++;
            }
        }
    }
}

void Sequence::CompareAndRemove(GenomeSequenceInfo &FSequences,
    GenomeSequenceInfo &HSequences,
    GenomeSequenceInfo &LSequences,
    string saveId)
{
    ExportToFaFile("tmp.fa", FSequences, HSequences, LSequences);

    string cmd = prankExeLocation + "\" -d=tmp.fa -o=tmp  -iterate=2 -quiet -F\"";
    cmd.insert(cmd.begin(), '\"');
    cmd.insert(cmd.begin(), '\"');
    system(cmd.c_str());

    ifstream ifs;
    ifs.open("tmp.best.fas");

    if (!ifs)
    {
        cerr << "fatal error: failed to open file tmp.best.fas" << endl;
        exit(1);
    }

    GenomeSequenceInfo RFSequences;
    GenomeSequenceInfo RHSequences;
    GenomeSequenceInfo RLSequences;

    //read sequence to vector
    ReadPrankSequenceToVector(ifs, RFSequences, RHSequences, RLSequences);

    //remove same sequence that is short
    RemoveSameShort(RFSequences);
    RemoveSameShort(RHSequences);
    RemoveSameShort(RLSequences);

    //remove continuous variant points that are longer than 10bp 
    //in head and tail that are about 100bp.
    RemoveContinuousVariant(RFSequences, RHSequences, RLSequences);

    //write to file
    string fileName = outputLocation + "hebing_" + saveId + ".fas";
    ExportToFaFile(fileName, RFSequences, RHSequences, RLSequences);

    //differentiate pattern
    Pattern pattern(outputLocation, saveId);
    pattern.DifferentiatePattern(RFSequences, RHSequences, RLSequences);
}

void Sequence::ExportToFaFile(string fileName,
    GenomeSequenceInfo &FSequences,
    GenomeSequenceInfo &HSequences,
    GenomeSequenceInfo &LSequences)
{
    stringstream sstr;
    CombineGenomeSequenceInfo(sstr, FSequences);
    CombineGenomeSequenceInfo(sstr, HSequences);
    CombineGenomeSequenceInfo(sstr, LSequences);

    ofstream ofs;
    ofs.open(fileName, ios::out);

    if (!ofs)
    {
        cerr << "fatal error: failed to open file " << fileName << " to write" << endl;
        exit(1);
    }

    ofs << sstr.rdbuf();
    ofs.close();
}

void Sequence::CombineGenomeSequenceInfo(stringstream &sstr, GenomeSequenceInfo &Seq)
{
    for (vector<GenomeSequence>::iterator itera = Seq.Sequences.begin();
        itera != Seq.Sequences.end(); itera++)
    {
        string tmpId = itera->sequenceId;
        string tmpName = itera->sequenceName;
        string tmpBaseInfo = itera->sequenceBaseInfo;
        string tmpSequence = itera->sequence;

        sstr << '>' << tmpId << " " << tmpName << " " << tmpBaseInfo << endl
            << tmpSequence << endl;
    }
}

void Sequence::ReadPrankSequenceToVector(ifstream &ifs,
    GenomeSequenceInfo &RFSequences,
    GenomeSequenceInfo &RHSequences,
    GenomeSequenceInfo &RLSequences)
{
    while (!ifs.eof())
    {
        char ch;
        string tmpId;
        string tmpName;
        string tmpBaseInfo;
        string tmpSequence;

        ifs >> ch;
        if (ch != '>')
        {
            break;
        }
        ifs >> ch;
        ifs.unget();

        string line;
        string stopSign;
        getline(ifs, line);

        //get sequence Id
        string str = line;
        string tmpStr;
        replace(str.begin(), str.end(), '_', ' ');
        stringstream sstr(str);
        sstr >> tmpId;
        tmpId.append("_");
        sstr >> tmpStr;
        tmpId.append(tmpStr);

        //get sequence base info
        string rstr = str;
        reverse(rstr.begin(), rstr.end());
        stringstream rsstr(rstr);
        rsstr >> tmpBaseInfo;
        tmpBaseInfo.append(" ");
        rsstr >> tmpStr;
        stopSign = tmpStr;
        reverse(stopSign.begin(), stopSign.end());
        tmpBaseInfo.append(tmpStr);
        reverse(tmpBaseInfo.begin(), tmpBaseInfo.end());

        //get sequence name
        sstr.clear();
        sstr.seekg(0, ios::beg);
        sstr >> tmpStr;
        sstr >> tmpStr;

        while (!sstr.eof())
        {
            sstr >> tmpStr;
            if (tmpStr == stopSign)
            {
                tmpName.pop_back();
                break;
            }
            tmpName += tmpStr;
            tmpName.append("_");
        }

        while (true)
        {
            string tmpStr;
            ifs >> ch;

            if (ch == '>' || ifs.eof())
            {
                ifs.unget();
                break;
            }

            ifs.unget();
            ifs >> tmpStr;
            tmpSequence += tmpStr;
        }

        char sequenceType = *tmpId.begin();

        switch (sequenceType)
        {
        case'f':
        {
            if (tmpSequence.length() > RFSequences.maxiumLength)
            {
                RFSequences.maxiumLength = tmpSequence.length();
            }
            RFSequences.Sequences.push_back(GenomeSequence(tmpId, tmpName, tmpBaseInfo, tmpSequence));
            break;
        }
        case'h':
        {
            if (tmpSequence.length() > RHSequences.maxiumLength)
            {
                RHSequences.maxiumLength = tmpSequence.length();
            }
            RHSequences.Sequences.push_back(GenomeSequence(tmpId, tmpName, tmpBaseInfo, tmpSequence));
            break;
        }
        case'l':
        {
            if (tmpSequence.length() > RLSequences.maxiumLength)
            {
                RLSequences.maxiumLength = tmpSequence.length();
            }
            RLSequences.Sequences.push_back(GenomeSequence(tmpId, tmpName, tmpBaseInfo, tmpSequence));
            break;
        }
        default:
            cerr << "fatal error: error occoured in reading prank files tmp.best.fas" << endl;
            exit(1);
        }
    }
}

void Sequence::RemoveSameShort(GenomeSequenceInfo &Seq)
{
    if (Seq.Sequences.size() == 1)
    {
        return;
    }

    for (vector<GenomeSequence>::iterator mainItera = Seq.Sequences.begin();
        mainItera != Seq.Sequences.end();)
    {
        string mainStr = mainItera->sequence;

        for (vector<GenomeSequence>::iterator compItera = mainItera + 1;
            compItera != Seq.Sequences.end();)
        {
            string compStr = compItera->sequence;
            bool isRemoveComp = true;

            //check compStr
            string::iterator compStrItera = compStr.begin();
            for (string::iterator mainStrItera = mainStr.begin();
                mainStrItera != mainStr.end(); mainStrItera++, compStrItera++)
            {
                if ((isalpha(*mainStrItera) && (*compStrItera == '-')) ||
                    ((*mainStrItera == '-') && (*compStrItera == '-')) ||
                    (*mainStrItera == *compStrItera))
                {
                    continue;
                }

                if (((*mainStrItera == '-') && (isalpha(*compStrItera))) || (*mainStrItera != *compStrItera))
                {
                    isRemoveComp = false;
                    break;
                }
            }

            //remove compStr and go check next compStr
            if ((compStrItera == compStr.end()) && isRemoveComp)
            {
                compItera = Seq.Sequences.erase(compItera);
                continue;
            }

            isRemoveComp = true;
            //check mainStr
            string::iterator mainStrItera = mainStr.begin();
            for (string::iterator compStrItera = compStr.begin();
                compStrItera != compStr.end(); compStrItera++, mainStrItera++)
            {
                if ((isalpha(*compStrItera) && (*mainStrItera == '-')) ||
                    ((*compStrItera == '-') && (*mainStrItera == '-')) ||
                    (*compStrItera == *mainStrItera))
                {
                    continue;
                }

                if (((*compStrItera == '-') && (isalpha(*mainStrItera))) || (*compStrItera != *mainStrItera))
                {
                    isRemoveComp = false;
                    break;
                }
            }

            //remove mainStr and go check next mainStr
            if ((mainStrItera == mainStr.end()) && isRemoveComp)
            {
                mainItera = Seq.Sequences.erase(mainItera);
                break;
            }

            compItera++;
        }

        mainItera++;
    }
}

void Sequence::RemoveContinuousVariant(GenomeSequenceInfo &RFSequences,
    GenomeSequenceInfo &RHSequences,
    GenomeSequenceInfo &RLSequences)
{
    vector<vector<char>> bpMatrix;

    //head
    LoadBps(bpMatrix, RFSequences, true);
    LoadBps(bpMatrix, RHSequences, true);
    LoadBps(bpMatrix, RLSequences, true);

    vector<int> mutationPointCount(bpMatrix.size(), 0);

    auto checkAndRemove =
        [&]()
    {
        for (size_t i = 0; i < bpMatrix.begin()->size(); i++)
        {
            vector<int> bpsFreq;
            //read bpMatrix column i to vector<char>
            vector<char>bpsInColumn;
            auto getBpsMatrixColumn =
                [](vector<vector<char>> bpMatrix, vector<char> &bpsInColumn, int i)
            {
                for (size_t j = 0; j < bpMatrix.size(); j++)
                {
                    bpsInColumn.push_back(bpMatrix[j][i]);
                }
            };

            getBpsMatrixColumn(bpMatrix, bpsInColumn, i);
            GetBpsFrequency(bpsInColumn, bpsFreq);

            if (*bpsFreq.begin() == bpMatrix.size())
            {
                mutationPointCount.clear();
                mutationPointCount = vector<int>(bpMatrix.size(), 0);
                continue;
            }
            else
            {
                for (size_t i = 0; i < bpMatrix.size(); i++)
                {
                    if (bpsFreq[i] <= (bpMatrix.size() * 0.5))
                    {
                        mutationPointCount[i] += 1;
                    }
                }
            }

            for (size_t i = 0; i < bpMatrix.size(); i++)
            {
                if (mutationPointCount[i] >= 10)
                {
                    //remove sequence
                    if (RFSequences.Sequences.size() == 1 &&
                        0 == i)
                    {
                        mutationPointCount[i] = 0;
                        continue;
                    }

                    if (RHSequences.Sequences.size() == 1 &&
                        RFSequences.Sequences.size() == i)
                    {
                        mutationPointCount[i] = 0;
                        continue;
                    }

                    if (RLSequences.Sequences.size() == 1 &&
                        RFSequences.Sequences.size() + RHSequences.Sequences.size() == i)
                    {
                        mutationPointCount[i] = 0;
                        continue;
                    }

                    RemoveSequence(RFSequences, RHSequences, RLSequences, i);

                    //remove sequence's bps in bpMatrix
                    bpMatrix.erase(bpMatrix.begin() + i);

                    //remvove mutation point count
                    mutationPointCount.erase(mutationPointCount.begin() + i);
                }
            }
        }
    };

    checkAndRemove();

    //tail
    bpMatrix.clear();

    LoadBps(bpMatrix, RFSequences, false);
    LoadBps(bpMatrix, RHSequences, false);
    LoadBps(bpMatrix, RLSequences, false);

    mutationPointCount = vector<int>(bpMatrix.size(), 0);

    checkAndRemove();
}

void Sequence::LoadBps(vector<vector<char>> &bps, GenomeSequenceInfo &Seqs, bool isHead)
{
    for (vector<GenomeSequence>::iterator itera = Seqs.Sequences.begin();
        itera != Seqs.Sequences.end(); itera++)
    {
        int endPos;
        if (itera->sequence.length() < 100)
        {
            endPos = itera->sequence.length();
        }
        else
        {
            endPos = 100;
        }

        if (isHead)
        {
            vector<char> tmpBps;

            for (string::iterator strItera = itera->sequence.begin();
                strItera != itera->sequence.begin() + endPos; strItera++)
            {
                tmpBps.push_back(*strItera);
            }

            bps.push_back(tmpBps);
        }
        else
        {
            vector<char> rTmpBps;

            for (string::reverse_iterator rStrItera = itera->sequence.rbegin();
                rStrItera != itera->sequence.rbegin() + endPos; rStrItera++)
            {
                rTmpBps.push_back(*rStrItera);
            }

            reverse(rTmpBps.begin(), rTmpBps.end());

            bps.push_back(rTmpBps);
        }
    }
}

void Sequence::GetBpsFrequency(vector<char> bps, vector<int> &bpsFreq)
{
    map<char, int>bpsFreqPairs;

    for (vector<char>::iterator itera = bps.begin(); itera != bps.end(); itera++)
    {
        map<char, int>::iterator mapItera = bpsFreqPairs.find(*itera);

        if (mapItera != bpsFreqPairs.end())
        {
            mapItera->second += 1;
        }
        else
        {
            bpsFreqPairs.insert(pair<char, int>(*itera, 1));
        }
    }

    for (vector<char>::iterator itera = bps.begin(); itera != bps.end(); itera++)
    {
        bpsFreq.push_back(bpsFreqPairs.find(*itera)->second);
    }
}

void Sequence::RemoveSequence(GenomeSequenceInfo &RFSequences,
    GenomeSequenceInfo &RHSequences,
    GenomeSequenceInfo &RLSequences,
    int index)
{
    int location(0);

    for (vector<GenomeSequence>::iterator rfItera = RFSequences.Sequences.begin();
        rfItera != RFSequences.Sequences.end(); rfItera++)
    {
        if (index == location)
        {
            RFSequences.Sequences.erase(rfItera);
            return;
        }

        location++;
    }

    for (vector<GenomeSequence>::iterator rhItera = RHSequences.Sequences.begin();
        rhItera != RHSequences.Sequences.end(); rhItera++)
    {
        if (index == location)
        {
            RHSequences.Sequences.erase(rhItera);
            return;
        }

        location++;
    }

    for (vector<GenomeSequence>::iterator rlItera = RLSequences.Sequences.begin();
        rlItera != RLSequences.Sequences.end(); rlItera++)
    {
        if (index == location)
        {
            RLSequences.Sequences.erase(rlItera);
            return;
        }

        location++;
    }
}
