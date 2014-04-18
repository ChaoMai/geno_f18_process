#include "finit.h"

using namespace std;

void mGetOptions(int, char**, Param&);

Param param;

int main(int argc, char* argv[])
{
	try
	{
		TimeUtili time;
		cout << "Start at: " << time.CurrTime() << endl;
		mGetOptions(argc, argv, param);

		Sequence sequence(param);

		sequence.LoadHomologousAnnotationFile();
		cout << "load Homologous Annotation File. " << time.AllTime()
				<< " secs passed." << endl;

		sequence.ProcessData();
		cout << "sequence Data is Processed. " << time.AllTime()
				<< " secs passed." << endl;

		Merge merge(param.outputLocation);
		cout << "merging pattern files. " << endl;
		merge.MergePatternFiles();

		cout << "All done. " << time.AllTime() << " secs passed." << endl;
	} catch (exception& e)
	{
		cerr << e.what() << endl;
	}

	return 0;
}

void mGetOptions(int argc, char* argv[], Param& param)
{
	// options
	int i;
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] != '-')
			return;
		switch (argv[i][1])
		{
		case 'f':
			param.filesLocation = argv[++i];
			break;
		case 'h':
			param.homologousAnnotationFile = argv[++i];
			break;
		case 'o':
			param.outputLocation = argv[++i];
			break;
		case 'n':
			param.mafftNum = atoi(argv[++i]);
			break;
		case 't':
			param.mafftThread = atoi(argv[++i]);
			break;
		case 'i':
			param.mafftIterate = atoi(argv[++i]);
			break;
		case 'm':
			param.mutationRate = atof(argv[++i]);
			break;
		}
	}
}
