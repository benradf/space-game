#include <assert.h>
#include <time.h>
#include <iostream>
#include "log.hpp"


#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH


// NOTE: This implementation is not thread safe at the moment.


Log::Null nullLog;
Log::Base* Log::log = &nullLog;


std::string getTimeString()
{
    char buffer[64];
    time_t epochTime = time(0);
    strftime(buffer, sizeof(buffer), "%d/%m/%y %H:%M:%S", localtime(&epochTime));
    
    return std::string(buffer);
}


////////// Log::LogBase //////////

Log::Base::~Base()
{
    
}

void Log::Base::debug(std::string message)
{
    write(std::string("(DD) [") + getTimeString() + "] " + message);
}

void Log::Base::info(std::string message)
{
    write(std::string("(II) [") + getTimeString() + "] " + message);
}

void Log::Base::warn(std::string message)
{
    write(std::string("(WW) [") + getTimeString() + "] " + message);
}

void Log::Base::error(std::string message)
{
    write(std::string("(EE) [") + getTimeString() + "] " + message);
}


////////// Log::Null //////////

void Log::Null::write(std::string)
{
    
}


////////// Log::Console //////////

void Log::Console::write(std::string message)
{
    std::cout << message << std::endl;
}


////////// Log::File //////////

Log::File::File(const char* filename)
    : file(filename, std::ios::app)
{
    assert(file.is_open());
}

Log::File::~File()
{
    file.close();
}

void Log::File::write(std::string message)
{
    file << message << std::endl;
}


////////// Log::Multi //////////

void Log::Multi::add(Base& log)
{
    _logs.push_back(&log);
}

void Log::Multi::write(std::string message)
{
    foreach (Base* log, _logs)
        log->write(message);
}
