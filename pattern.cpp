#include "pattern.h"

void Pattern::DifferentiatePattern(GenomeSequenceInfo &FSequence,
		GenomeSequenceInfo &HSequence, GenomeSequenceInfo &LSequence)
{
	Differentiate(FSequence, HSequence, LSequence);
	ExportToPatternFile(patternsInfo);
}

void Pattern::Differentiate(GenomeSequenceInfo &FSequence,
		GenomeSequenceInfo &HSequence, GenomeSequenceInfo &LSequence)
{
	//pattern 6
	if ((FSequence.Sequences.size() != 0) && (HSequence.Sequences.size() != 0)
			&& (LSequence.Sequences.size() == 0))
	{
		//return 6;
		patternsInfo.push_back(
				MultiplePattern(HSequence.Sequences.begin()->sequenceName,
						"none", FSequence.Sequences.begin()->sequenceName, 6));
		return;
	}

	//pattern 7
	if ((FSequence.Sequences.size() != 0) && (LSequence.Sequences.size() != 0)
			&& (HSequence.Sequences.size() == 0))
	{
		//return 7;
		patternsInfo.push_back(
				MultiplePattern("none",
						LSequence.Sequences.begin()->sequenceName,
						FSequence.Sequences.begin()->sequenceName, 7));
		return;
	}

	for (auto hItera = HSequence.Sequences.begin();
			hItera != HSequence.Sequences.end(); hItera++)
	{
		string hStr = hItera->sequence;

		for (auto lItera = LSequence.Sequences.begin();
				lItera != LSequence.Sequences.end(); lItera++)
		{
			string lStr = lItera->sequence;

			for (auto fItera = FSequence.Sequences.begin();
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
					patternsInfo.push_back(
							MultiplePattern(hName, lName, fName, 9));
					return;
				}

				int mutationPointCount(0);
				string fSeqEncode;

				bool isInEnds(false);
				int mutationP10Count(0);
				bool isCandidateOfPattern10(false);

				char preEncode('\0');
				string::iterator preMutationLoc(fStr.begin());

				char curEncode;
				string::iterator curMutationLoc;

				auto hStrItera = hStr.begin();
				auto lStrItera = lStr.begin();

				for (auto fStrItera = fStr.begin(); fStrItera != fStr.end();
						fStrItera++, hStrItera++, lStrItera++)
				{
					if (*fStrItera != *hStrItera && *fStrItera != *lStrItera)
					{
						mutationPointCount++;
						continue;
					}

					if (((float) distance(fStr.begin(), fStrItera)
							<= (float) fStr.length() * 0.05)
							|| ((float) distance(fStrItera, fStr.end())
									<= (float) fStr.length() * 0.05))
					{
						isInEnds = true;
					}
					else
					{
						isInEnds = false;
						mutationP10Count = 0;
					}

					if (*fStrItera == *hStrItera && *fStrItera != *lStrItera)
					{
						curEncode = 'h';
						curMutationLoc = fStrItera;
					}

					if (*fStrItera == *lStrItera && *fStrItera != *hStrItera)
					{
						curEncode = 'l';
						curMutationLoc = fStrItera;
					}

					if (preEncode != curEncode)
					{
						if (isInEnds == true)
						{
							if (distance(preMutationLoc, curMutationLoc) == 1)
							{
								mutationP10Count++;
							}
							else
							{
								mutationP10Count = 0;
							}

							//mutation occoured once, so the count is 7
							if (mutationP10Count >= 7)
							{
								//might be pattern 10
								isCandidateOfPattern10 = true;
							}
						}

						fSeqEncode.push_back(curEncode);
					}

					preEncode = curEncode;
					preMutationLoc = curMutationLoc;
				}

				if (((float) mutationPointCount / fStr.length())
						<= param.mutationRate)
				{
					if (isCandidateOfPattern10 == true)
					{
						patternsInfo.push_back(
								MultiplePattern(hName, lName, fName, 10));
					}
					else
					{
						string reducedStr;
						ReduceSequence(fSeqEncode, reducedStr);
						int patternNumber = CheckPattern(reducedStr);

						patternsInfo.push_back(
								MultiplePattern(hName, lName, fName,
										patternNumber));
					}
				}
				else
				{
					//pattern 8
					patternsInfo.push_back(
							MultiplePattern(hName, lName, fName, 8));
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
		else if (reducedStr[0] == 'l')
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
		else if (reducedStr[0] == 'l' && reducedStr[1] == 'h')
		{
			return 2;
		}
	}
	else if (length == 3)
	{
		//pattern 4
		if (reducedStr[0] == 'h' && reducedStr[1] == 'l'
				&& reducedStr[2] == 'h')
		{
			return 4;
		}
		//pattern 3
		else if (reducedStr[0] == 'l' && reducedStr[1] == 'h'
				&& reducedStr[2] == 'l')
		{
			return 3;
		}
	}
	return 5;
}

void Pattern::ExportToPatternFile(vector<MultiplePattern> info)
{
	string fileName = outputLocation + "pattern_hebing_" + saveId;

	coutMutex.lock();
	cout << "saving pattern info " + saveId << endl;
	coutMutex.unlock();

	ofstream ofs;
	ofs.open(fileName, ios::out);

	if (!ofs)
	{
		cerr << "fatal error: failed to open file pattern_hebing_" << saveId
				<< " to write" << endl;
		exit(1);
	}

	stringstream sstr;

	sstr << "pattern_id\t" << "h_name\t" << "l_name\t" << "f_name\t"
			<< "pattern" << endl;

	for (auto itera = info.begin(); itera != info.end(); itera++)
	{
		sstr << itera - info.begin() + 1 << '\t' << itera->HName << '\t'
				<< itera->LName << '\t' << itera->FName << '\t'
				<< itera->patternNumber << endl;
	}

	ofs << sstr.rdbuf();
	ofs.close();
}
