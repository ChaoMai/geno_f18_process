#include "utilities.h"

TimeUtili::TimeUtili()
{
    timeBegin = time(NULL);
    timeLast = time(NULL);
}

time_t TimeUtili::AllTime()
{
    return time(NULL) - timeBegin;
}

char* TimeUtili::CurrTime()
{
    time_t t = time(NULL);
    return ctime(&t);
}
