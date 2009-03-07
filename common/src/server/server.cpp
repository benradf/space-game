#include <errno.h>
#include <unistd.h>
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
#include "idle.hpp"

using namespace std;

int abc;


void serverMain()
{
    JobPool pool;
    
    // Change working directory.
    if (chdir(getSettings().directory().c_str()) != 0)
        throw FileException("unable to change to specified working directory");

    // Create standard jobs.
    std::auto_ptr<Idle> jobIdle(new Idle(100000));
    std::auto_ptr<PostOffice> jobPostOffice(new PostOffice);
    std::auto_ptr<NetworkInterface> jobNetwork(new NetworkInterface(*jobPostOffice));

    // Add to pool.
    pool.add(Job::Ptr(jobIdle));
    pool.add(Job::Ptr(jobPostOffice));
    pool.add(Job::Ptr(jobNetwork));

#if 0
    VMSet vmSet;
    Job::Ptr vm1(new VirtualMachine(vmSet, 1000));
    //Job::Ptr vm2(new VirtualMachine(vmSet, 2000));
    pool.add(vm1);
    //pool.add(vm2);
#endif
    
    // Create worker threads.
    std::vector<boost::shared_ptr<Worker> > workers;
    for (int i = 0; i < getSettings().threadMax(); i++) {
        workers.push_back(boost::shared_ptr<Worker>(new Worker(pool)));
        Log::log->info("Worker thread created");
    }
    
    int end;
    cin >> end;
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
