#include "merge.h"

void Merge::MergePatternFiles()
{
    vector<string> files;
    GetAllFilesNamesWithinFolder(patternFolder, files);

    vector<PatternFile> patterns;
    ReadPatterns(patternFolder, files, patterns);
    ExportToPatternFile(patterns);
}

void Merge::GetAllFilesNamesWithinFolder(string folder, vector<string>& names)
{
    folder.append("\\*.*");

    wstring wstr = wstring(folder.begin(), folder.end());
    const wchar_t* szFolder = wstr.c_str();

    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(szFolder, &fd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                char ch[260];
                char DefChar = ' ';
                WideCharToMultiByte(CP_ACP, 0, fd.cFileName, -1, ch, 260, &DefChar, NULL);
                string str(ch);

                names.push_back(str);
            }
        } while (::FindNextFile(hFind, &fd));
        FindClose(hFind);
    }
}

void Merge::ReadPatterns(string folder, vector<string> files, vector<PatternFile>& patterns)
{
    for (string file : files)
    {
        if (file.find("pattern") == string::npos)
        {
            continue;
        }

        ifstream ifs;

        ifs.open(folder + "\\" + file);

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

        string row2;
        ifs >> row2;

        string tmp;
        getline(ifs, tmp);

        if (row2 == "pattern")
        {
            string patternId;
            ifs >> patternId;

            string patternNumber;
            ifs >> patternNumber;

            patterns.push_back({ file, patternNumber });
        }
        else
        {
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

            patterns.push_back({ file, patternNumber });
        }

        ifs.close();
    }
}

void Merge::ExportToPatternFile(vector<PatternFile>& patterns)
{
    string fileName = patternFolder + "\\pattern_hebing_all";

    ofstream ofs;
    ofs.open(fileName, ios::out);

    if (!ofs)
    {
        cerr << "fatal error: failed to open file pattern_hebing_all to write" << endl;
        exit(1);
    }

    stringstream sstr;

    sstr << "file_name\t" << "patterns" << endl;

    for (PatternFile pattern : patterns)
    {
        sstr << pattern.fileName << '\t'
            << pattern.patterns << endl;
    }

    ofs << sstr.rdbuf();
    ofs.close();
}
