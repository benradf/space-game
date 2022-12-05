/// \file daemon.hpp
/// \brief Handles daemon spawning.
/// \author Ben Radford 
/// \date 18th May 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef DAEMON_HPP
#define DAEMON_HPP


class SignalHandler {
    public:
        SignalHandler();
        virtual ~SignalHandler();

    protected:
        void installSignalHandler(int sig);

    private:
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

        static void signalHandler(int sig);
        static SignalHandler* _this;
};


class Daemon {
    public:
        Daemon(const char* pidFile);

        virtual ~Daemon();
        virtual int main() = 0;

        void daemonise(const char* user, const char* dir);

    private:
        void secure(const char* user);

        void forkOff();
        void closeFiles();
        void writePidFile();

        const char* _pidFile;
        int _pidFileDescriptor;
};


class Watchdog {
    public:

    private:

};


#endif  // DAEMON_HPP

