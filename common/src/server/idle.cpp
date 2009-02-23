#include "idle.hpp"


////////// Timer //////////

Timer::Timer()
{
    reset();
}

void Timer::reset()
{
    gettimeofday(&_start, 0);
}

unsigned int Timer::elapsed()
{
    timeval now;
    gettimeofday(&now, 0);
    
    int deltaSecond = now.tv_sec - _start.tv_sec;
    int deltaMicrosecond = now.tv_usec - _start.tv_usec;
     
    return (1000000 * deltaSecond + deltaMicrosecond);
}


////////// Idle //////////

Idle::Idle(unsigned int cyclePeriod)
    : _cyclePeriod(cyclePeriod)
{
    
}

Idle::~Idle()
{
    
}

Job::RetType Idle::run()
{
    unsigned int elapsed = _timer->elapsed();
    
    if (elapsed < _cyclePeriod)
        usleep(_cyclePeriod - elapsed);
    
    _timer->reset();
    
    return Job::YIELD;
}

bool Idle::readOnly()
{
    return true;
}
