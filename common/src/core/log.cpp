#include <assert.h>
#include <time.h>
#include <iostream>
#include "log.hpp"


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

void Log::Base::trace(const std::string& message)
{
    write(std::string("(TT) [") + getTimeString() + "] " + message);
}

void Log::Base::debug(const std::string& message)
{
    write(std::string("(DD) [") + getTimeString() + "] " + message);
}

void Log::Base::info(const std::string& message)
{
    write(std::string("(II) [") + getTimeString() + "] " + message);
}

void Log::Base::warn(const std::string& message)
{
    write(std::string("(WW) [") + getTimeString() + "] " + message);
}

void Log::Base::error(const std::string& message)
{
    write(std::string("(EE) [") + getTimeString() + "] " + message);
}

void Log::Base::fatal(const std::string& message)
{
    write(std::string("(FF) [") + getTimeString() + "] " + message);
}


////////// Log::Null //////////

void Log::Null::write(const std::string&)
{
    
}


////////// Log::Console //////////

void Log::Console::write(const std::string& message)
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

void Log::File::write(const std::string& message)
{
    file << message << std::endl;
}


////////// Log::Multi //////////

void Log::Multi::add(Base& log)
{
    _logs.push_back(&log);
}

void Log::Multi::write(const std::string& message)
{
    for (auto log : _logs)
        log->write(message);
}
