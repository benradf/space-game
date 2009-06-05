#include "logging.hpp"
#include "autolock.hpp"
#include <sys/types.h>
#include <unistd.h>


void initialiseLogging()
{
    static Log::Console consoleLog;

    char filename[64];
    snprintf(filename, sizeof(filename), "/var/log/mmoserv_%d.log", getpid());
    static Log::File fileLog(filename);

    static Log::Multi multiLog;
    multiLog.add(consoleLog);
    multiLog.add(fileLog);

    static SafeLog safeLog(multiLog);
    Log::log = &safeLog;
}


////////// LogEvent //////////

LogEvent::operator const std::string&()
{
    return what();
}


////////// SafeLog //////////

SafeLog::SafeLog(Log::Base& log) :
    _lock(log)
{

}

void SafeLog::write(const std::string& message)
{
    AutoWriteLock<Log::Base>(_lock)->write(message);
}


////////// LogMsg //////////

LogMsg::LogMsg(const std::string& msg) :
    _msg(msg)
{

}

const std::string& LogMsg::what()
{
    return _msg;
}

