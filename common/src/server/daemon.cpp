/// \file daemon.cpp
/// \brief Handles daemon spawning.
/// \author Ben Radford 
/// \date 18th May 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
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


////////// Daemon //////////

Daemon* Daemon::_this = 0;

Daemon::Daemon()
{
    _this = this;
}

Daemon::~Daemon()
{
    _this = 0;
}

void Daemon::secure(const char* user)
{
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

void Daemon::daemonise(const char* pidFile)
{
    forkOff();
    setsid();
    forkOff();
    closeFiles();
    umask(0022);
    chdir("/");
    exclude(pidFile);
    main();
}

void Daemon::installSignalHandler(int sig)
{
    signal(sig, signalHandler);
}

void Daemon::handle_SIGINT()
{

}

void Daemon::handle_SIGQUIT()
{

}

void Daemon::handle_SIGILL()
{

}

void Daemon::handle_SIGABRT()
{

}

void Daemon::handle_SIGFPE()
{

}

void Daemon::handle_SIGSEGV()
{

}

void Daemon::handle_SIGPIPE()
{

}

void Daemon::handle_SIGALRM()
{

}

void Daemon::handle_SIGTERM()
{

}

void Daemon::handle_SIGUSR1()
{

}

void Daemon::handle_SIGUSR2()
{

}

void Daemon::handle_SIGCHLD()
{

}

void Daemon::handle_SIGCONT()
{

}

void Daemon::handle_SIGTSTP()
{

}

void Daemon::handle_SIGTTIN()
{

}

void Daemon::handle_SIGTTOU()
{

}

void Daemon::handle_SIGBUS()
{

}

void Daemon::handle_SIGPOLL()
{

}

void Daemon::handle_SIGPROF()
{

}

void Daemon::handle_SIGSYS()
{

}

void Daemon::handle_SIGTRAP()
{

}

void Daemon::handle_SIGURG()
{

}

void Daemon::handle_SIGVTALRM()
{

}

void Daemon::handle_SIGXCPU()
{

}

void Daemon::handle_SIGXFSZ()
{

}

void Daemon::handle_SIGSTKFLT()
{

}

void Daemon::handle_SIGPWR()
{

}

void Daemon::handle_SIGWINCH()
{

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

void Daemon::exclude(const char* pidFile)
{
    int fd = open(pidFile, O_WRONLY|O_CREAT, 0640);
    
    if (fd == -1) 
        throw ErrNoException("failed to open pid file");

    if (lockf(fd, F_TLOCK, 0) != 0) 
        throw ErrNoException("failed to lock pid file");

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d\n", getpid());
    write(fd, buffer, strlen(buffer));
}

void Daemon::signalHandler(int sig)
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

