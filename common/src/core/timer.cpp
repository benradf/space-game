/// \file timer.hpp
/// \brief Portable timer implementation.
/// \author Ben Radford 
/// \date 26th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///
/// Modifications (most recent first):


#include "timer.hpp"
#include <assert.h>
#include <math.h>


////////// Timer //////////

Timer::Timer()
{
    reset();

#ifdef WIN32
    QueryPerformanceFrequency((LARGE_INTEGER*)&_freq);
    assert(_freq != 0);
#endif
}

/// Reset elapsed time to zero.
void Timer::reset()
{
#ifdef WIN32
    _startGTC = GetTickCount();
    QueryPerformanceCounter((LARGE_INTEGER*)&_startQPC);
#else
    gettimeofday(&_start, 0);
#endif
}

/// \return Elapsed time in microseconds.
uint64_t Timer::elapsed()
{
#ifdef WIN32
    uint64_t nowQPC;
    QueryPerformanceCounter((LARGE_INTEGER*)&nowQPC);
    DWORD nowGTC = GetTickCount();

    uint64_t deltaQPC = nowQPC - _startQPC;
    uint64_t deltaMicrosecondQPC = (1000000 * deltaQPC) / _freq ;
    uint64_t deltaMillisecondGTC = nowGTC - _startGTC;

    uint64_t variance = abs(deltaMillisecondGTC - (deltaMicrosecondQPC / 1000));

    if (variance > QPC_TOLERANCE) 
        return (1000 * deltaMillisecondGTC);

    return deltaMicrosecondQPC;
#else
    timeval now;
    gettimeofday(&now, 0);
    
    uint64_t deltaSecond = now.tv_sec - _start.tv_sec;
    uint64_t deltaMicrosecond = now.tv_usec - _start.tv_usec;
     
    return (1000000 * deltaSecond + deltaMicrosecond);
#endif
}

