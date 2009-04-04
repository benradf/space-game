#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "concurrency.hpp"
#include <core.hpp>
#include "settings.hpp"
#include "scriptmodule.hpp"
#include "postoffice.hpp"
#include "network.hpp"
#include "player.hpp"
#include "idle.hpp"
#include "zone.hpp"

// temp testing of headers
#include "volumes.hpp"
#include "zone.hpp"

using namespace std;

static const int CYCLE_PERIOD = 100000;

void signalShutdown(int signum)
{
    Log::log->info("SIGINT caught: use CTRL+D to shutdown server cleanly");
}

void catchSignals(bool yes)
{
    if (yes) {
        signal(SIGINT, signalShutdown);
    } else {
        signal(SIGINT, SIG_DFL);
    }
}


void serverMain()
{
    // Catch signals.
    catchSignals(true);

    // Change working directory.
    if (chdir(getSettings().directory().c_str()) != 0)
        throw FileException("unable to change to specified working directory");

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
    
    // Main thread waits.
    while (!feof(stdin))
        fgetc(stdin);

    // Stop catching signals.
    catchSignals(false);
}

int main(int argc, char* argv[])
{
    // Initialise logging.
    Log::Console consoleLog;
    Log::File fileLog("server.log");
    Log::Multi multiLog;
    multiLog.add(consoleLog);
    multiLog.add(fileLog);
    Log::log = &multiLog;

    // Read settings.
    Settings serverSettings(argc, argv);
    setSettings(serverSettings);
    
    Log::log->info("Server starts");
    Log::log->info("Built " __DATE__ " " __TIME__);
    
    try {
        serverMain();
    } catch (std::exception& e) {
        Log::log->error(e.what());
    }
    
    Log::log->info("Server stops");
    
    return 0;
}
