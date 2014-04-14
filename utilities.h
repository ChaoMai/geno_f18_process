#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <cstdlib>
#include <ctime>

class TimeUtili
{
public:
	TimeUtili();
	time_t AllTime();
	char* CurrTime();

private:
	time_t timeBegin;
	time_t timeLast;
};

#endif //_UTILITIES_H_
