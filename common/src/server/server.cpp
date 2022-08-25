#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "concurrency.hpp"
#include <core/core.hpp>
#include "settings.hpp"
#include "scriptmodule.hpp"
#include "postoffice.hpp"
#include "network.hpp"
#include "player.hpp"
#include "idle.hpp"
#include "zone.hpp"
#include <math/prim.hpp>
#include "canvas.hpp"
#include <physics/kdtree.hpp>
#include "daemon.hpp"
#include "logging.hpp"

// temp testing of headers
#include <math/volumes.hpp>
#include "zone.hpp"

using namespace std;

static const int CYCLE_PERIOD = 100000;


class Server : public Daemon, public SignalHandler {
    public:
        Server();
        virtual ~Server();
        virtual int main();

    private:
        int safeMain();

        virtual void handle_SIGINT();
        virtual void handle_SIGTERM();

        bool _running;

};

Server::Server() :
    Daemon("/var/run/mmoserv.pid"), _running(true)
{

}

Server::~Server()
{

}

int Server::main()
{
    initialiseLogging();
    
    logInfo(LOGMSG_SERVER_START);
    net::initialise();

    installSignalHandler(SIGINT);
    installSignalHandler(SIGTERM);
    
    try {
        safeMain();
    } catch (std::exception& e) {
        logFatal(e.what());
    }

    net::cleanup();
    
    return 0;
}

int Server::safeMain()
{
    // Create standard jobs.
    auto jobIdle = std::make_unique<Idle>(CYCLE_PERIOD);
    auto jobPostOffice = std::make_unique<PostOffice>();
    auto jobNetwork = std::make_unique<NetworkInterface>(*jobPostOffice);
    auto jobLogin = std::make_unique<LoginManager>(*jobPostOffice);
    auto testZone = std::make_unique<Zone>(*jobPostOffice);

    // Add to pool.
    JobPool pool;
    pool.add(std::move(jobIdle));
    pool.add(std::move(jobPostOffice));
    pool.add(std::move(jobNetwork));
    pool.add(std::move(jobLogin));
    pool.add(std::move(testZone));

    // Create worker threads.
    std::vector<boost::shared_ptr<Worker> > workers;
    for (int i = 0; i < getSettings().threadMax(); i++) {
        workers.push_back(boost::shared_ptr<Worker>(new Worker(pool)));
        logInfo(LOGMSG_CREATE_THREAD);
    }

    do {
        pause();
    } while (_running);
}

void Server::handle_SIGINT()
{
    logInfo(LOGMSG_SERVER_STOP);

    _running = false;
}

void Server::handle_SIGTERM()
{
    logInfo(LOGMSG_SERVER_STOP);

    _running = false;
}

int main(int argc, char* argv[])
{
    Settings settings(argc, argv);
    setSettings(settings);

    try {
        Server server;
        server.daemonise("mmoserv", "/");
    } catch (const std::exception& e) {
        ofstream log("/var/log/mmoserv_init.log");
        log << e.what() << endl;
        log.close();
    }

    return 0;
}

