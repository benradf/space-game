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
#include "sim.hpp"


using namespace std;
using namespace gfx;


bool clientRunning = true;


void signalHandler(int signum)
{
    clientRunning = false;
}

#define PI 3.141592654

static const float degPerRad = 180.0f / PI;
static const float radPerDeg = PI / 180.f;

#define DEG2RAD(x) (x * radPerDeg)
#define RAD2DEG(x) (x * degPerRad)

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

    Timer simTimer;
    sim::Object testObj(10.0f);

    while (clientRunning) {
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

        //camera->setPosition(Ogre::Vector3(pos[0], pos[1], pos[2] + 200.0f));

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

