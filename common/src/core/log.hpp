#ifndef LOG_HPP
#define LOG_HPP


#include <vector>
#include <fstream>


namespace Log {

extern class Base* log;

class Base {
    public:
        virtual ~Base();
        
        void debug(std::string message);
        void info(std::string message);
        void warn(std::string message);
        void error(std::string message);
        
        virtual void write(std::string message) = 0;
};

class Null : public Base {
    public:
        virtual void write(std::string message);
};

class Console : public Base {
    public:
        virtual void write(std::string message);
};

class File : public Base {
    public:
        File(const char* filename);
        ~File();
        
        virtual void write(std::string message);
        
    private:
        std::ofstream file;    
};

class Multi : public Base {
    public:
        void add(Base& log);
        
        virtual void write(std::string message);
        
    private:
        std::vector<Base*> _logs;
};

}

#endif  // LOG_HPP
