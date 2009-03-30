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
#include <timer.hpp>
#include "network.hpp"
#include "graphics.hpp"
#include "input.hpp"
#include <sim.hpp>


using namespace std;
using namespace gfx;


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
    std::auto_ptr<Entity> spider = scene->createEntity("spider", "warbird.mesh");
    std::auto_ptr<Entity> warbird = scene->createEntity("warbird", "warbird.mesh");
    spider->setPosition(Ogre::Vector3(0.0f, 10.0f, 0.0f));
    warbird->setPosition(Ogre::Vector3(0.0f, -10.0f, 0.0f));
    camera->setPosition(Ogre::Vector3(-0.1f, -0.1f, 200.0f));
    camera->lookAt(Ogre::Vector3(0.0f, 0.0f, 0.0f));
    scene->setSkyPlane("Sky/OrbitEarth", Ogre::Vector3::UNIT_Z, -1000.0f);

    Ship ship(*scene, "username", "spider.mesh");
    Input input(gfx.getViewport().getRenderWindow());
    std::auto_ptr<LocalController> ctrl = input.createKeyboardListener<LocalController>();
    ctrl->setObject(&ship);

    NetworkInterface network;

    network.setServer("localhost");
    network.maintainServerConnection(true);

    Timer simTimer;
    sim::Object testObj(10.0f);

    while (clientRunning) {
#if 0
        testObj.ApplySpin(Quaternion(DEG2RAD(-30.0f), Vector3(0.0f, 0.0f, 1.0f)));
        testObj.ApplySpin(Quaternion(DEG2RAD(45.0f), Vector3(1.0f, 0.0f, 0.0f)));
        //testObj.ApplyRelativeForce(Vector3(0.0f, 10.0f, 0.0f));
        testObj.ApplyAbsoluteForce(Vector3(0.0f, 0.0f, 20.0f));
        float elapsed = simTimer.elapsed();
        elapsed /= 1000000.0f;
        testObj.integrate(elapsed);
        simTimer.reset();

        const Quaternion& rot = testObj.getRotation();
        spider->setOrientation(Ogre::Quaternion(rot.w, rot.x, rot.y, rot.z));
        const Vector3& pos = testObj.getPosition();
        spider->setPosition(Ogre::Vector3(pos.x, pos.y, pos.z));
#endif

        const Vector3& shipPos = ship.getApparentPosition();
        camera->setPosition(Ogre::Vector3(shipPos.x, shipPos.y, shipPos.z + 200.0f));

        input.capture();
        network.main();
        ship.update();
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

#ifdef WIN32
    int rval = 0;//chdir("mingw/share/client");
#else
    int rval = chdir("linux/share/client");
#endif
    assert(rval == 0);
    
    try {
        clientMain();
    } catch (std::exception& e) {
        Log::log->error(e.what());
    }
    
    Log::log->info("Client stops");
    
    return 0;
}

