#include "logging.hpp"
#include "autolock.hpp"
#include <sys/types.h>
#include <unistd.h>


void initialiseLogging()
{
    static Log::Console consoleLog;
    static Log::File fileLogAll("/tmp/space-game-server-all.log");

    char filename[256];
    snprintf(filename, sizeof(filename), "/tmp/space-game-server-%d.log", getpid());
    static Log::File fileLogSession(filename);

    static Log::Multi multiLog;
    multiLog.add(consoleLog);
    multiLog.add(fileLogAll);
    multiLog.add(fileLogSession);

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

