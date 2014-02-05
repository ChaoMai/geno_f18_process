#include "f18init.h"

using namespace std;

void mGetOptions(int, char**, string &, string &, string &, string &);

int main(int argc, char* argv[])
{
    string filesLocation;
    string homologousAnnotationFile;
    string outputLocation;
    string prankExeLocation;

    try
    {
        TimeUtili time;
        cout << "Start at: " << time.CurrTime() << endl;
        mGetOptions(argc, argv, filesLocation, homologousAnnotationFile, outputLocation, prankExeLocation);

        Sequence sequence(filesLocation, homologousAnnotationFile, outputLocation, prankExeLocation);

        sequence.LoadHomologousAnnotationFile();
        cout << "load Homologous Annotation File. "
            << time.AllTime()
            << " secs passed."
            << endl;

        sequence.ProcessData();
        cout << "sequence Data is Processed. "
            << time.AllTime()
            << " secs passed."
            << endl;
    }
    catch (exception e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}

void mGetOptions(int argc, char* argv[],
    string &filesLocation, string &homologousAnnotationFile, string &outputLocation,
    string &prankExeLocation) {
    // options
    int i;
    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-') return;
        switch (argv[i][1])
        {
        case 'f': filesLocation = argv[++i]; break;
        case 'h': homologousAnnotationFile = argv[++i]; break;
        case 'o': outputLocation = argv[++i]; break;
        case 'p': prankExeLocation = argv[++i]; break;
        }
    }
}
