#ifndef _PARAM_H_
#define _PARAM_H_

#include "types.h"

class Param
{
public:
	string filesLocation;
	string homologousAnnotationFile;
	string outputLocation;
	int mafftNum;
	int mafftThread;
	int mafftIterate;
	double mutationRate;

	Param();
};

#endif //_PARAM_H_
