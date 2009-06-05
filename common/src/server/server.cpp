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
    
    logInfo("server started");
    net::initialise();

    installSignalHandler(SIGINT);
    
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
    std::auto_ptr<Idle> jobIdle(new Idle(CYCLE_PERIOD));
    std::auto_ptr<PostOffice> jobPostOffice(new PostOffice);
    std::auto_ptr<NetworkInterface> jobNetwork(new NetworkInterface(*jobPostOffice));
    std::auto_ptr<LoginManager> jobLogin(new LoginManager(*jobPostOffice));
    std::auto_ptr<Zone> testZone(new Zone(*jobPostOffice));

    // Add to pool.
    JobPool pool;
    pool.add(Job::Ptr(jobIdle));
    pool.add(Job::Ptr(jobPostOffice));
    pool.add(Job::Ptr(jobNetwork));
    pool.add(Job::Ptr(jobLogin));
    pool.add(Job::Ptr(testZone));

    // Create worker threads.
    std::vector<boost::shared_ptr<Worker> > workers;
    for (int i = 0; i < getSettings().threadMax(); i++) {
        workers.push_back(boost::shared_ptr<Worker>(new Worker(pool)));
        Log::log->info("Worker thread created");
    }

    do {
        pause();
    } while (_running);
}

void Server::handle_SIGINT()
{
    logInfo("SIGINT caught: stopping server");

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
        ofstream log("/var/log/mmoserv.log");
        log << e.what() << endl;
        log.close();
    }

    return 0;
}

