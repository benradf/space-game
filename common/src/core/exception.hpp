#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP


#include <string>
#include <exception>
#include <string.h>
#include <errno.h>


#define STRINGIZE(S) REALLY_STRINGIZE(S)
#define REALLY_STRINGIZE(S) #S

#define errorLocation() (__FILE__ ":" STRINGIZE(__LINE__))


class Exception : public std::exception {
    public:
        Exception();
        Exception(const std::string& description);
        virtual ~Exception() throw ();
        virtual const char* what() const throw ();
        void annotate(const std::string& description);
        
    private:
        int levels;
        std::string _what;
};


class ErrNoException : public Exception {
    public:
        ErrNoException();
        ErrNoException(const char* context);
        virtual ~ErrNoException() throw ();
};


template<typename T>
class DerivedException : public Exception {
    public:
        DerivedException();
        DerivedException(const std::string& description);
        virtual ~DerivedException() throw ();
};


#undef NetworkException
class NetworkExceptionTag {};
typedef DerivedException<NetworkExceptionTag> NetworkException;


#undef ScriptException
class ScriptExceptionTag {};
typedef DerivedException<ScriptExceptionTag> ScriptException;


#undef FileException
class FileExceptionTag {};
typedef DerivedException<FileExceptionTag> FileException;


#undef InputException
class InputExceptionTag {};
typedef DerivedException<InputExceptionTag> InputException;


#undef MemoryException
class MemoryExceptionTag {};
typedef DerivedException<MemoryExceptionTag> MemoryException;


////////// Exception //////////

inline Exception::Exception()
    : _what("An exception was thrown:")
{
    
}

inline Exception::Exception(const std::string& description)
    : _what("An exception was thrown:")
{
    annotate(description);
}

inline Exception::~Exception() throw ()
{
    
}

inline const char* Exception::what() const throw ()
{
    return _what.c_str();
}

inline void Exception::annotate(const std::string& description)
{
    _what += "\n";
    _what += description;
}


////////// ErrNoException //////////

inline ErrNoException::ErrNoException()
{
    char buffer[64];

    annotate(strerror_r(errno, buffer, sizeof(buffer)));
}

inline ErrNoException::ErrNoException(const char* context)
{
    char buffer[64];
    char description[256];

    snprintf(description, sizeof(description), "%s: %s", 
        context, strerror_r(errno, buffer, sizeof(buffer)));

    annotate(description);
}

inline ErrNoException::~ErrNoException() throw ()
{

}


////////// DerivedException //////////

template<typename T>
inline DerivedException<T>::DerivedException()
{
    
}

template<typename T>
inline DerivedException<T>::DerivedException(const std::string& description)
    : Exception(description)
{
    
}

template<typename T>
inline DerivedException<T>::~DerivedException() throw ()
{
    
}


#endif  // EXCEPTION_HPP
