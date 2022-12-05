/// \file daemon.cpp
/// \brief Handles daemon spawning.
/// \author Ben Radford 
/// \date 18th May 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#include "daemon.hpp"

#include <iostream>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <pwd.h>

#include <core/exception.hpp>
#include "logging.hpp"


////////// SignalHandler //////////

SignalHandler* SignalHandler::_this = 0;

SignalHandler::SignalHandler()
{
    _this = this;
}

SignalHandler::~SignalHandler()
{
    _this = 0;
}

void SignalHandler::installSignalHandler(int sig)
{
    signal(sig, signalHandler);
}

void SignalHandler::handle_SIGINT()
{

}

void SignalHandler::handle_SIGQUIT()
{

}

void SignalHandler::handle_SIGILL()
{

}

void SignalHandler::handle_SIGABRT()
{

}

void SignalHandler::handle_SIGFPE()
{

}

void SignalHandler::handle_SIGSEGV()
{

}

void SignalHandler::handle_SIGPIPE()
{

}

void SignalHandler::handle_SIGALRM()
{

}

void SignalHandler::handle_SIGTERM()
{

}

void SignalHandler::handle_SIGUSR1()
{

}

void SignalHandler::handle_SIGUSR2()
{

}

void SignalHandler::handle_SIGCHLD()
{

}

void SignalHandler::handle_SIGCONT()
{

}

void SignalHandler::handle_SIGTSTP()
{

}

void SignalHandler::handle_SIGTTIN()
{

}

void SignalHandler::handle_SIGTTOU()
{

}

void SignalHandler::handle_SIGBUS()
{

}

void SignalHandler::handle_SIGPOLL()
{

}

void SignalHandler::handle_SIGPROF()
{

}

void SignalHandler::handle_SIGSYS()
{

}

void SignalHandler::handle_SIGTRAP()
{

}

void SignalHandler::handle_SIGURG()
{

}

void SignalHandler::handle_SIGVTALRM()
{

}

void SignalHandler::handle_SIGXCPU()
{

}

void SignalHandler::handle_SIGXFSZ()
{

}

void SignalHandler::handle_SIGSTKFLT()
{

}

void SignalHandler::handle_SIGPWR()
{

}

void SignalHandler::handle_SIGWINCH()
{

}

void SignalHandler::signalHandler(int sig)
{
    if (_this == 0) 
        return;

    switch (sig) {
        case SIGINT: _this->handle_SIGINT(); break;
        case SIGQUIT: _this->handle_SIGQUIT(); break;
        case SIGILL: _this->handle_SIGILL(); break;
        case SIGABRT: _this->handle_SIGABRT(); break;
        case SIGFPE: _this->handle_SIGFPE(); break;
        case SIGSEGV: _this->handle_SIGSEGV(); break;
        case SIGPIPE: _this->handle_SIGPIPE(); break;
        case SIGALRM: _this->handle_SIGALRM(); break;
        case SIGTERM: _this->handle_SIGTERM(); break;
        case SIGUSR1: _this->handle_SIGUSR1(); break;
        case SIGUSR2: _this->handle_SIGUSR2(); break;
        case SIGCHLD: _this->handle_SIGCHLD(); break;
        case SIGCONT: _this->handle_SIGCONT(); break;
        case SIGTSTP: _this->handle_SIGTSTP(); break;
        case SIGTTIN: _this->handle_SIGTTIN(); break;
        case SIGTTOU: _this->handle_SIGTTOU(); break;
        case SIGBUS: _this->handle_SIGBUS(); break;
        case SIGPOLL: _this->handle_SIGPOLL(); break;
        case SIGPROF: _this->handle_SIGPROF(); break;
        case SIGSYS: _this->handle_SIGSYS(); break;
        case SIGTRAP: _this->handle_SIGTRAP(); break;
        case SIGURG: _this->handle_SIGURG(); break;
        case SIGVTALRM: _this->handle_SIGVTALRM(); break;
        case SIGXCPU: _this->handle_SIGXCPU(); break;
        case SIGXFSZ: _this->handle_SIGXFSZ(); break;
        case SIGSTKFLT: _this->handle_SIGSTKFLT(); break;
        case SIGPWR: _this->handle_SIGPWR(); break;
        case SIGWINCH: _this->handle_SIGWINCH(); break;
    }
}


////////// Daemon //////////

Daemon::Daemon(const char* pidFile) :
    _pidFile(pidFile)
{

}

Daemon::~Daemon()
{

}

void Daemon::daemonise(const char* user, const char* dir)
{
    secure(user);
    forkOff();
    setsid();
    forkOff();
    chdir(dir);
    umask(0022);
    writePidFile();
    main();
}

void Daemon::secure(const char* user)
{
    closeFiles();

    passwd* entry = getpwnam(user);

    if (entry == 0) 
        throw Exception("user not found");

    if (chdir(entry->pw_dir) != 0) 
        throw ErrNoException("chdir failed");

    if (chroot(entry->pw_dir) != 0) 
        throw ErrNoException("chroot failed");

    if (setgid(entry->pw_gid) != 0) 
        throw ErrNoException("setgid failed");

    if (setuid(entry->pw_uid) != 0)
        throw ErrNoException("setuid failed");
}

void Daemon::forkOff()
{
    pid_t pid = fork();

    if (pid == -1) 
        throw ErrNoException("fork failed");

    if (pid == 0) 
        return;

    _exit(0);
}

void Daemon::closeFiles()
{
    for (int i = 0; i < getdtablesize(); i++)
        close(i);

    int fd = open("/dev/null", O_RDONLY);
    dup2(fd, 0);
    close(fd);

    fd = open("/dev/null", O_WRONLY);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
}

void Daemon::writePidFile()
{
    int fd = open(_pidFile, O_WRONLY|O_CREAT, 0640);
    
    if (fd == -1) 
        throw ErrNoException("failed to open pid file");

    if (lockf(fd, F_TLOCK, 0) != 0) 
        throw ErrNoException("failed to lock pid file");

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d\n", getpid());
    write(fd, buffer, strlen(buffer));

    _pidFileDescriptor = fd;
}

