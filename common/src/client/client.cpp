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
#include "graphics.hpp"


using namespace std;


bool clientRunning = true;


void signalHandler(int signum)
{
    clientRunning = false;
}


void clientMain()
{
    signal(SIGINT, signalHandler);

    GFXManager gfx;
    std::auto_ptr<Scene> scene = gfx.createScene();
    std::auto_ptr<Camera> camera = scene->createCamera("cam1");
    gfx.getViewport().attachCamera(*camera);
    std::auto_ptr<Entity> spider = scene->createEntity("spider", "spider.mesh");
    std::auto_ptr<Entity> warbird = scene->createEntity("warbird", "warbird.mesh");
    spider->setPosition(Ogre::Vector3(0.0f, 10.0f, 0.0f));
    warbird->setPosition(Ogre::Vector3(0.0f, -10.0f, 0.0f));
    camera->setPosition(Ogre::Vector3(-0.1f, -0.1f, 200.0f));
    camera->lookAt(Ogre::Vector3(0.0f, 0.0f, 0.0f));
    scene->setSkyPlane("Sky/OrbitEarth", Ogre::Vector3::UNIT_Z, -1000.0f);

    NetworkInterface network;

    network.setServer("localhost");
    network.maintainServerConnection(true);

    while (clientRunning) {
        network.main();
        gfx.render();
        gfx.getViewport().update();

        usleep(10000);
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

    int rval = chdir("linux/share/client");
    assert(rval == 0);
    
    try {
        clientMain();
    } catch (std::exception& e) {
        Log::log->error(e.what());
    }
    
    Log::log->info("Client stops");
    
    return 0;
}

