/// \file daemon.hpp
/// \brief Handles daemon spawning.
/// \author Ben Radford 
/// \date 18th May 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef DAEMON_HPP
#define DAEMON_HPP


class Daemon {
    public:
        Daemon();
        virtual ~Daemon();
        virtual void main() = 0;

        void secure(const char* user);
        void daemonise(const char* pidFile);

        void installSignalHandler(int sig);

        virtual void handle_SIGINT();
        virtual void handle_SIGQUIT();
        virtual void handle_SIGILL();
        virtual void handle_SIGABRT();
        virtual void handle_SIGFPE();
        virtual void handle_SIGSEGV();
        virtual void handle_SIGPIPE();
        virtual void handle_SIGALRM();
        virtual void handle_SIGTERM();
        virtual void handle_SIGUSR1();
        virtual void handle_SIGUSR2();
        virtual void handle_SIGCHLD();
        virtual void handle_SIGCONT();
        virtual void handle_SIGTSTP();
        virtual void handle_SIGTTIN();
        virtual void handle_SIGTTOU();
        virtual void handle_SIGBUS();
        virtual void handle_SIGPOLL();
        virtual void handle_SIGPROF();
        virtual void handle_SIGSYS();
        virtual void handle_SIGTRAP();
        virtual void handle_SIGURG();
        virtual void handle_SIGVTALRM();
        virtual void handle_SIGXCPU();
        virtual void handle_SIGXFSZ();
        virtual void handle_SIGSTKFLT();
        virtual void handle_SIGPWR();
        virtual void handle_SIGWINCH();

    private:
        void forkOff();
        void closeFiles();
        void exclude(const char* pidFile);

        static void signalHandler(int sig);
        static Daemon* _this;
};


class Watchdog {
    public:

    private:

};


#endif  // DAEMON_HPP

