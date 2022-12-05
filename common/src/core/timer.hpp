/// \file timer.hpp
/// \brief Portable timer implementation.
/// \author Ben Radford 
/// \date 26th March 2009
///
/// Copyright (c) 2009 Ben Radford.
///
/// Modifications (most recent first):


#ifndef TIMER_HPP
#define TIMER_HPP


#include <stdint.h>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif


class Timer {
    public:
        Timer();
        void reset();
        uint64_t elapsed();
        
    private:
#ifdef WIN32
        DWORD _startGTC;
        uint64_t _startQPC;
        uint64_t _freq;
        static const int QPC_TOLERANCE = 50;
#else
        timeval _start;
#endif
};


#endif  // TIMER_HPP

