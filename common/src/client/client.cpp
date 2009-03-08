/// \file client.hpp
/// \brief Main client file.
/// \author Ben Radford 
/// \date 8nd March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <core.hpp>
#include "network.hpp"


using namespace std;


bool clientRunning = true;


void signalHandler(int signum)
{
    clientRunning = false;
}


void clientMain()
{
    signal(SIGINT, signalHandler);

    NetworkInterface network;

    network.setServer("localhost");
    network.maintainServerConnection(true);

    while (clientRunning) {
        network.main();
        usleep(100000);
    }
}

int main(int argc, char* argv[])
{
    // Initialise logging.
    Log::Console consoleLog;
    Log::File fileLog("client.log");
    Log::Multi multiLog;
    multiLog.add(consoleLog);
    multiLog.add(fileLog);
    Log::log = &multiLog;

    Log::log->info("Client starts");
    Log::log->info("Built " __DATE__ " " __TIME__);
    
    try {
        clientMain();
    } catch (std::exception& e) {
        Log::log->error(e.what());
    }
    
    Log::log->info("Client stops");
    
    return 0;
}

