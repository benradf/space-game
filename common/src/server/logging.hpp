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


class Logger;
class LogXMLTree;
class LogXMLText;


extern Logger* logger;


#define logTrace(e) logger->log(Logger::TRACE, e)
#define logDebug(e) logger->log(Logger::DEBUG, e)
#define logInfo(e) logger->log(Logger::INFO, e)
#define logWarn(e) logger->log(Logger::WARN, e)
#define logError(e) logger->log(Logger::ERROR, e)
#define logFatal(e) logger->log(Logger::FATAL, e)


struct Logger {
    enum Level { TRACE, DEBUG, INFO, WARN, ERROR, FATAL };
    virtual void log(Level level, const LogXMLTree& event) = 0;
};


class FileLogger : public Logger {
    public:
        FileLogger(const char* filename);
        virtual void log(Level level, const LogXMLTree& event);

    private:
        std::ofstream _log;
};


class LogXMLTree {
    public:
        LogXMLTree(const std::string& name);
        virtual ~LogXMLTree();

        virtual std::string getXML(int indent = 0) const;

    protected:
        void pushChild(LogXMLTree* child);
        void pushChild(std::auto_ptr<LogXMLTree> child);

    private:
        static const int INDENT_SPACES = 4;

        std::vector<LogXMLTree*> _children;
        std::string _name;
};


class LogXMLText : public LogXMLTree {
    public:
        LogXMLText(const std::string& name, const std::string& text);

    private:
        struct Text : public LogXMLTree {
            Text(const std::string& text);
            virtual std::string getXML(int indent = 0) const;
            std::string _text;
        };
};


struct LogTime : public LogXMLText {
    LogTime();
};


struct LogBacktraceFrame : public LogXMLTree {
    LogBacktraceFrame(char* symbol);
};


struct LogBacktrace : public LogXMLTree {
    LogBacktrace();
    static const int MAX_SIZE = 64;
};


struct LogContext : public LogXMLTree {
    LogContext(const std::string& name);
};


struct LogException : public LogContext {
    LogException(const std::exception& e);
};


struct LogNote : public LogContext {
    LogNote(const std::string& message);
};


#endif  // LOGGING_HPP

