#ifndef _PARAM_H_
#define _PARAM_H_

#include "types.h"

class Param
{
public:
	string filesLocation;
	string homologousAnnotationFile;
	string outputLocation;
	string prankExeLocation;
	int threadNum;
	int prankIterate;
	float mutationRate;

	Param();
};

#endif //_PARAM_H_
