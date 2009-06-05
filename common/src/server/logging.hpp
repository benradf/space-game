/// \file logging.hpp
/// \brief Advanced logging features.
/// \author Ben Radford 
/// \date 19th May 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef LOGGING_HPP
#define LOGGING_HPP


#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <core/log.hpp>
#include "logmsg.hpp"
#include "lock.hpp"


#ifdef NDEBUG
#define logTrace(e)
#define logDebug(e)
#define logInfo(e) Log::log->info(e)
#define logWarn(e) Log::log->warn(e)
#define logError(e) Log::log->error(e)
#define logFatal(e) Log::log->fatal(e)
#else
#define logTrace(e) Log::log->trace(e)
#define logDebug(e) Log::log->debug(e)
#define logInfo(e) Log::log->info(e)
#define logWarn(e) Log::log->warn(e)
#define logError(e) Log::log->error(e)
#define logFatal(e) Log::log->fatal(e)
#endif  // NDEBUG


void initialiseLogging();


class SafeLog : public Log::Base {
    public:
        SafeLog(Log::Base& log);
        virtual void write(const std::string& message);

    private:
        Lock<Log::Base> _lock;
};


class LogEvent {
    public:
        virtual const std::string& what() = 0;
        operator const std::string&();
};


class LogMsg : public LogEvent {
    public:
        LogMsg(const std::string& msg);
        virtual const std::string& what();

    private:
        std::string _msg;
};


#endif  // LOGGING_HPP

