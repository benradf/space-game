/// \file idle.hpp
/// \brief Provides job to run when server needs to idle.
/// \author Ben Radford 
/// \date 10th November 2007
///
/// Copyright (c) 2007 Ben Radford. All rights reserved.
///
/// Modifications (most recent first):


#ifndef IDLE_HPP
#define IDLE_HPP


#include <sys/time.h>
#include "concurrency.hpp"
#include "localstore.hpp"


class Timer {
    public:
        Timer();
        void reset();
        unsigned int elapsed();
        
    private:
        timeval _start;
};


class Idle : public Job {
    public:
        Idle(unsigned int cyclePeriod);
        virtual ~Idle();
        
        virtual Job::RetType run();
        
    private:
        virtual bool readOnly();
        
        LocalStore<Timer> _timer;
        unsigned int _cyclePeriod;
};


#endif  // IDLE_HPP
