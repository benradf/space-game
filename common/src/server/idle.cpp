#include "idle.hpp"
#include <unistd.h>


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
