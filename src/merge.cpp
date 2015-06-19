#include "merge.h"

void Merge::MergePatternFiles()
{
	vector<string> files;
	GetAllFilesNamesWithinFolder(patternFolder, files);

	vector<PatternFile> patterns;
	ReadPatterns(files, patterns);
	ExportToPatternFile(patterns);
}

void Merge::GetAllFilesNamesWithinFolder(string folder, vector<string>& names)
{
	DIR *dir;
	struct dirent *ent;
	struct stat st;

	dir = opendir(folder.c_str());
	while ((ent = readdir(dir)) != NULL)
	{
		const string file_name = ent->d_name;
		const string full_file_name = folder + "/" + file_name;

		if (file_name[0] == '.')
			continue;

		if (stat(full_file_name.c_str(), &st) == -1)
			continue;

		const bool is_directory = (st.st_mode & S_IFDIR) != 0;

		if (is_directory)
			continue;

		names.push_back(full_file_name);
	}
	closedir(dir);
}

void Merge::ReadPatterns(vector<string> files, vector<PatternFile>& patterns)
{
	for (string file : files)
	{
		if (file.find("pattern_hebing") == string::npos)
		{
			continue;
		}

		ifstream ifs;

		ifs.open(file);

		if (!ifs)
		{
			cerr << "fatal error: failed to open pattern file " << file << endl;
			exit(1);
		}

		string row1;

		ifs >> row1;

		if (row1 != "pattern_id")
		{
			cerr << "fatal error: file " << file << " has error" << endl;
			exit(1);
		}

		string tmp;
		getline(ifs, tmp);

		string patternNumber;

		while (!ifs.eof())
		{
			string patternId;
			ifs >> patternId;

			string hName;
			ifs >> hName;

			string lName;
			ifs >> lName;

			string fName;
			ifs >> fName;

			string tmpPatternNumber;
			ifs >> tmpPatternNumber;

			getline(ifs, tmp);

			patternNumber += tmpPatternNumber + " ";
		}

		patterns.push_back(
		{ file, patternNumber });

		ifs.close();
	}
}

void Merge::ExportToPatternFile(vector<PatternFile>& patterns)
{
	string fileName = patternFolder + "/pattern_all";

	ofstream ofs;
	ofs.open(fileName, ios::out);

	if (!ofs)
	{
		cerr << "fatal error: failed to open file pattern_all to write" << endl;
		exit(1);
	}

	stringstream sstr;

	sstr << "file_name\t" << "patterns" << endl;

	for (PatternFile pattern : patterns)
	{
		sstr << pattern.fileName << '\t' << pattern.patterns << endl;
	}

	ofs << sstr.rdbuf();
	ofs.close();
}
