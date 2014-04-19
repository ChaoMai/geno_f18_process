#include "sequence.h"

Sequence::Sequence(Param param)
{
	filesLocation = param.filesLocation;
	homologousAnnotationFile = param.homologousAnnotationFile;
	outputLocation = param.outputLocation;

	struct stat st =
	{ 0 };

	if (stat(outputLocation.c_str(), &st) == -1)
	{
		mkdir(outputLocation.c_str(), 0755);
	}

	filesLocation.append("/");
	outputLocation.append("/");
}

void Sequence::LoadHomologousAnnotationFile()
{
	string fileName = homologousAnnotationFile;
	ifstream ifs;

	ifs.open(fileName);

	if (!ifs)
	{
		cerr
				<< "fatal error: failed to open homologous annotation file: "
						+ fileName << endl;
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

		homologousAnnotations.push_back(HomologousAnnotation(id, name));
	}

	sort(homologousAnnotations.begin(), homologousAnnotations.end(),
			[](const HomologousAnnotation &h1, const HomologousAnnotation &h2)
			{
				return h1.genoName < h2.genoName;
			});

	ifs.close();
}

void Sequence::ProcessData()
{
	vector<int> ids;

	for (auto itera = homologousAnnotations.begin();
			itera != homologousAnnotations.end(); itera++)
	{
		string headName = itera->genoName;

		ids.push_back(itera->fileId);

		if (itera == homologousAnnotations.end() - 1
				|| headName != (itera + 1)->genoName)
		{
			ProcessSequence(ids, headName);
			ids.clear();
		}
	}

//	vector<int> ids;
//	vector<thread> workers;
//
//	for (auto itera = homologousAnnotations.begin();
//			itera != homologousAnnotations.end(); itera++)
//	{
//		string headName = itera->genoName;
//
//		ids.push_back(itera->fileId);
//
//		if (itera == homologousAnnotations.end() - 1
//				|| headName != (itera + 1)->genoName)
//		{
//			workers.push_back(
//					thread(&Sequence::ProcessSequence, this, ids, headName));
//			ids.clear();
//		}
//
//		for (std::thread &t : workers)
//		{
//			if (t.joinable())
//			{
//				t.join();
//			}
//		}
//	}
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
	CompareAndRemove(FSequences, HSequences, LSequences, to_string(ids[0]));
}

int Sequence::LoadSequenceFiles(vector<int> ids, GenomeSequenceInfo &FSequence,
		GenomeSequenceInfo &HSequence, GenomeSequenceInfo &LSequence)
{
	for (auto itera = ids.begin(); itera != ids.end(); itera++)
	{
		string fileId(to_string(*itera));
		string fileName = filesLocation + "hebing_" + fileId + ".fa";

		coutMutex.lock();
		cout << "load hebing_" + fileId + ".fa" << endl;
		coutMutex.unlock();

		ifstream ifs;
		ifs.open(fileName);

		if (!ifs)
		{
			cerr << "error: failed to open sequence file " << fileId << endl;
			return 1;
		}

		if (ReadSequenceToVector(ifs, FSequence, HSequence, LSequence, fileId)
				== 1)
		{
			return 1;
		}
		ifs.close();
	}

	return 0;
}

int Sequence::ReadSequenceToVector(ifstream &ifs, GenomeSequenceInfo &FSequence,
		GenomeSequenceInfo &HSequence, GenomeSequenceInfo &LSequence,
		string fileId)
{
	int count(0);

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
		case 'f':
		{
			if (tmpSequence.length() > FSequence.maxiumLength)
			{
				FSequence.maxiumLength = tmpSequence.length();
			}
			FSequence.Sequences.push_back(
					GenomeSequence(tmpId, tmpName, tmpBaseInfo, tmpSequence));
			count++;
			break;
		}
		case 'h':
		{
			if (tmpSequence.length() > HSequence.maxiumLength)
			{
				HSequence.maxiumLength = tmpSequence.length();
			}
			HSequence.Sequences.push_back(
					GenomeSequence(tmpId, tmpName, tmpBaseInfo, tmpSequence));
			count++;
			break;
		}
		case 'l':
		{
			if (tmpSequence.length() > LSequence.maxiumLength)
			{
				LSequence.maxiumLength = tmpSequence.length();
			}
			LSequence.Sequences.push_back(
					GenomeSequence(tmpId, tmpName, tmpBaseInfo, tmpSequence));
			count++;
			break;
		}
		default:
			cerr << "fatal error: error occoured in reading genome file "
					<< fileId << endl;
			exit(1);
		}
	}

	if (count == 0)
	{
		return 1;
	}
	return 0;
}

void Sequence::RemoveRedundancyAndShort(GenomeSequenceInfo &Seq)
{
	for (auto mainItera = Seq.Sequences.begin();
			mainItera != Seq.Sequences.end(); mainItera++)
	{
		string mainStr = mainItera->sequence;

		for (auto compItera = mainItera + 1; compItera != Seq.Sequences.end();)
		{
			string compStr = compItera->sequence;

			if ((mainStr == compStr)
					|| (compStr.length() <= Seq.maxiumLength * 0.75))
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
		GenomeSequenceInfo &HSequences, GenomeSequenceInfo &LSequences,
		string saveId)
{
	string faFileName = "tmp" + saveId + ".fa";
	ExportToFaFile(faFileName, FSequences, HSequences, LSequences);

	string mafftOutPutFileName = "tmp" + saveId + ".fas";
	string cmd = "mafft --retree 2 --maxiterate "
			+ to_string(param.mafftIterate) + " --thread "
			+ to_string(param.mafftThread) + " " + faFileName + " > "
			+ mafftOutPutFileName;

	system(("export MAFFT_BINARIES=/usr/lib/mafft/lib/mafft/; " + cmd).c_str());

	ifstream ifs;
	ifs.open(mafftOutPutFileName);

	if (!ifs)
	{
		cerr << "fatal error: failed to open file " + mafftOutPutFileName
				<< endl;
		return;
		//exit(1);
	}

	GenomeSequenceInfo RFSequences;
	GenomeSequenceInfo RHSequences;
	GenomeSequenceInfo RLSequences;

	//read sequence to vector
	ReadSequenceToVector(ifs, RFSequences, RHSequences, RLSequences, saveId);

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

	ifs.close();

	remove(mafftOutPutFileName.c_str());
	remove(faFileName.c_str());
}

void Sequence::ExportToFaFile(string fileName, GenomeSequenceInfo &FSequences,
		GenomeSequenceInfo &HSequences, GenomeSequenceInfo &LSequences)
{
	stringstream sstr;
	CombineGenomeSequenceInfo(sstr, FSequences);
	CombineGenomeSequenceInfo(sstr, HSequences);
	CombineGenomeSequenceInfo(sstr, LSequences);

	ofstream ofs;
	ofs.open(fileName, ios::out);

	if (!ofs)
	{
		cerr << "fatal error: failed to open file " << fileName << " to write"
				<< endl;
		exit(1);
	}

	ofs << sstr.rdbuf();
	ofs.close();
}

void Sequence::CombineGenomeSequenceInfo(stringstream &sstr,
		GenomeSequenceInfo &Seq)
{
	for (auto itera = Seq.Sequences.begin(); itera != Seq.Sequences.end();
			itera++)
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
		GenomeSequenceInfo &RFSequences, GenomeSequenceInfo &RHSequences,
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
		case 'f':
		{
			if (tmpSequence.length() > RFSequences.maxiumLength)
			{
				RFSequences.maxiumLength = tmpSequence.length();
			}
			RFSequences.Sequences.push_back(
					GenomeSequence(tmpId, tmpName, tmpBaseInfo, tmpSequence));
			break;
		}
		case 'h':
		{
			if (tmpSequence.length() > RHSequences.maxiumLength)
			{
				RHSequences.maxiumLength = tmpSequence.length();
			}
			RHSequences.Sequences.push_back(
					GenomeSequence(tmpId, tmpName, tmpBaseInfo, tmpSequence));
			break;
		}
		case 'l':
		{
			if (tmpSequence.length() > RLSequences.maxiumLength)
			{
				RLSequences.maxiumLength = tmpSequence.length();
			}
			RLSequences.Sequences.push_back(
					GenomeSequence(tmpId, tmpName, tmpBaseInfo, tmpSequence));
			break;
		}
		default:
			cerr
					<< "fatal error: error occoured in reading prank files tmp.best.fas"
					<< endl;
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

	for (auto mainItera = Seq.Sequences.begin();
			mainItera != Seq.Sequences.end();)
	{
		string mainStr = mainItera->sequence;

		for (auto compItera = mainItera + 1; compItera != Seq.Sequences.end();)
		{
			string compStr = compItera->sequence;
			bool isRemoveComp = true;

			//check compStr
			auto compStrItera = compStr.begin();
			for (auto mainStrItera = mainStr.begin();
					mainStrItera != mainStr.end();
					mainStrItera++, compStrItera++)
			{
				if ((isalpha(*mainStrItera) && (*compStrItera == '-'))
						|| (*mainStrItera == *compStrItera))
				{
					continue;
				}

				if (((*mainStrItera == '-') && (isalpha(*compStrItera)))
						|| (*mainStrItera != *compStrItera))
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
			auto mainStrIteraBack = mainStr.begin();
			for (auto compStrIteraBack = compStr.begin();
					compStrIteraBack != compStr.end();
					compStrIteraBack++, mainStrIteraBack++)
			{
				if ((isalpha(*compStrIteraBack) && (*mainStrIteraBack == '-'))
						|| (*compStrIteraBack == *mainStrIteraBack))
				{
					continue;
				}

				if (((*compStrIteraBack == '-') && (isalpha(*mainStrIteraBack)))
						|| (*compStrIteraBack != *mainStrIteraBack))
				{
					isRemoveComp = false;
					break;
				}
			}

			//remove mainStr and go check next mainStr
			if ((mainStrIteraBack == mainStr.end()) && isRemoveComp)
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
		GenomeSequenceInfo &RHSequences, GenomeSequenceInfo &RLSequences)
{
	vector<vector<char>> bpMatrix;

	//head
	LoadBps(bpMatrix, RFSequences, true);
	LoadBps(bpMatrix, RHSequences, true);
	LoadBps(bpMatrix, RLSequences, true);

	vector<int> mutationPointCount(bpMatrix.size(), 0);

	auto checkAndRemove = [&]()
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

			if ((size_t)*bpsFreq.begin() == bpMatrix.size())
			{
				mutationPointCount.clear();
				mutationPointCount = vector<int>(bpMatrix.size(), 0);
				continue;
			}
			else
			{
				for (size_t i = 0; i < bpMatrix.size(); i++)
				{
					if ((float)bpsFreq[i] <= (float)(bpMatrix.size() * 0.5))
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

void Sequence::LoadBps(vector<vector<char>> &bps, GenomeSequenceInfo &Seqs,
		bool isHead)
{
	for (auto itera = Seqs.Sequences.begin(); itera != Seqs.Sequences.end();
			itera++)
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

			for (auto strItera = itera->sequence.begin();
					strItera != itera->sequence.begin() + endPos; strItera++)
			{
				tmpBps.push_back(*strItera);
			}

			bps.push_back(tmpBps);
		}
		else
		{
			vector<char> rTmpBps;

			for (auto rStrItera = itera->sequence.rbegin();
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
	map<char, int> bpsFreqPairs;

	for (auto itera = bps.begin(); itera != bps.end(); itera++)
	{
		auto mapItera = bpsFreqPairs.find(*itera);

		if (mapItera != bpsFreqPairs.end())
		{
			mapItera->second += 1;
		}
		else
		{
			bpsFreqPairs.insert(pair<char, int>(*itera, 1));
		}
	}

	for (auto itera = bps.begin(); itera != bps.end(); itera++)
	{
		bpsFreq.push_back(bpsFreqPairs.find(*itera)->second);
	}
}

void Sequence::RemoveSequence(GenomeSequenceInfo &RFSequences,
		GenomeSequenceInfo &RHSequences, GenomeSequenceInfo &RLSequences,
		int index)
{
	int location(0);

	for (auto rfItera = RFSequences.Sequences.begin();
			rfItera != RFSequences.Sequences.end(); rfItera++)
	{
		if (index == location)
		{
			RFSequences.Sequences.erase(rfItera);
			return;
		}

		location++;
	}

	for (auto rhItera = RHSequences.Sequences.begin();
			rhItera != RHSequences.Sequences.end(); rhItera++)
	{
		if (index == location)
		{
			RHSequences.Sequences.erase(rhItera);
			return;
		}

		location++;
	}

	for (auto rlItera = RLSequences.Sequences.begin();
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
