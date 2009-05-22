#include "logging.hpp"
#include <core/core.hpp>
#include <execinfo.h>


static FileLogger nullLogger("/dev/null");
Logger* logger = &nullLogger;


////////// FileLogger //////////

FileLogger::FileLogger(const char* filename) :
    _log(filename)
{

}

void FileLogger::log(Level level, const LogXMLTree& event)
{
    _log << event.getXML() << std::endl;
}


////////// LogXMLTree //////////

LogXMLTree::LogXMLTree(const std::string& name) :
    _name(name)
{

}

LogXMLTree::~LogXMLTree()
{
    foreach (LogXMLTree* child, _children) 
        std::auto_ptr<LogXMLTree> p(child);
}

std::string LogXMLTree::getXML(int indent) const
{
    std::string xml;

    std::string indentStr;
    for (int i = 0; i < indent; i++) 
        indentStr += " ";

    xml += indentStr + "<" + _name + ">\n";

    foreach (const LogXMLTree* child, _children) 
        xml += child->getXML(indent + INDENT_SPACES);

    xml += indentStr + "</" + _name + ">\n";

    return xml;
}

void LogXMLTree::pushChild(LogXMLTree* child)
{
    pushChild(std::auto_ptr<LogXMLTree>(child));
}

void LogXMLTree::pushChild(std::auto_ptr<LogXMLTree> child)
{
    _children.push_back(child.get());
    child.release();
}


////////// LogXMLText //////////

LogXMLText::LogXMLText(const std::string& name, const std::string& text) :
    LogXMLTree(name)
{
    pushChild(new Text(text));
}


////////// LogXMLText::Text //////////

LogXMLText::Text::Text(const std::string& text) :
    LogXMLTree(""), _text(text)
{

}

std::string LogXMLText::Text::getXML(int indent) const
{
    std::string indentStr;
    for (int i = 0; i < indent; i++) 
        indentStr += " ";

    return indentStr + _text + "\n";
}


////////// LogTime //////////

static std::string getTimeString()
{
    char buffer[64];
    time_t epochTime = time(0);
    strftime(buffer, sizeof(buffer), "%d/%m/%y %H:%M:%S", localtime(&epochTime));
    
    return std::string(buffer);
}

LogTime::LogTime() :
    LogXMLText("time", getTimeString())
{

}


////////// LogBacktraceFrame //////////

LogBacktraceFrame::LogBacktraceFrame(char* symbol) :
    LogXMLTree("frame")
{
    char null = 0;

    char* addr = strchr(symbol, ' ');
    if (addr != 0) {
        *addr++ = 0;
    } else {
        addr = &null;
    }

    char* offset = strchr(symbol, '+');
    if (offset != 0) {
        *offset++ = 0;
        *strchr(offset, ')') = 0;
    } else {
        offset = &null;
    }

    char* function = strchr(symbol, '(');
    if (function != 0) {
        *function++ = 0;
    } else {
        function = &null;
    }

    char* object = symbol;

    pushChild(new LogXMLText("object", object));
    pushChild(new LogXMLText("function", function));
    pushChild(new LogXMLText("offset", offset));
    pushChild(new LogXMLText("return", addr));
}


////////// LogBacktrace //////////

LogBacktrace::LogBacktrace() :
    LogXMLTree("backtrace")
{
    void** buffer = new void*[MAX_SIZE];
    int size = backtrace(buffer, MAX_SIZE);
    char** symbols = backtrace_symbols(buffer, size);

    try {
        for (int i = 0; i < size; i++)
            pushChild(new LogBacktraceFrame(symbols[i]));

    } catch (...) {
        free(symbols);
    }

    if (size == MAX_SIZE) 
        pushChild(new LogXMLText("function", "..."));

    free(symbols);
}


////////// LogContext //////////

LogContext::LogContext(const std::string& name) :
    LogXMLTree(name)
{
    pushChild(new LogTime);
}


////////// LogException //////////

LogException::LogException(const std::exception& e) :
    LogContext("exception")
{
    pushChild(new LogXMLText("description", e.what()));
}


////////// LogStatic //////////

LogNote::LogNote(const std::string& message) :
    LogContext("notification")
{
    pushChild(new LogXMLText("message", message));
}

