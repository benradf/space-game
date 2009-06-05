#ifndef LOG_HPP
#define LOG_HPP


#include <vector>
#include <fstream>


namespace Log {

extern class Base* log;

class Base {
    public:
        virtual ~Base();

        void trace(const std::string& message);
        void debug(const std::string& message);
        void info(const std::string& message);
        void warn(const std::string& message);
        void error(const std::string& message);
        void fatal(const std::string& message);

        virtual void write(const std::string& message) = 0;
};

class Null : public Base {
    public:
        virtual void write(const std::string& message);
};

class Console : public Base {
    public:
        virtual void write(const std::string& message);
};

class File : public Base {
    public:
        File(const char* filename);
        ~File();
        
        virtual void write(const std::string& message);
        
    private:
        std::ofstream file;    
};

class Multi : public Base {
    public:
        void add(Base& log);
        
        virtual void write(const std::string& message);
        
    private:
        std::vector<Base*> _logs;
};

}

#endif  // LOG_HPP
