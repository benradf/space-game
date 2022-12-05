/// \file client.hpp
/// \brief Main client file.
/// \author Ben Radford 
/// \date 8nd March 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <core/core.hpp>
#include <core/timer.hpp>
#include "network.hpp"
#include "graphics.hpp"
#include "input.hpp"
#include <physics/sim.hpp>
#include <stdio.h>
#include <physics/object.hpp>
#include <string>
#include <algorithm>
#include "hud.hpp"
#include "login.hpp"


using namespace std;
using namespace gfx;


bool clientRunning = true;
std::string serverHostname("localhost");


void signalHandler(int signum)
{
    clientRunning = false;
}

Scene* theScene = 0;

Physics* physics = 0;

gfx::HUD* hudPtr = 0;

gfx::Camera* camPtr = 0;

std::unique_ptr<VisibleObject> createVisibleObject(sim::ObjectID objectID)
{
    assert(theScene != 0);
    assert(physics != 0);
    assert(hudPtr != 0);
    assert(camPtr != 0);

    char nameBuffer[256];
    snprintf(nameBuffer, sizeof(nameBuffer), "object_%d", objectID);
    sim::Ship* ship = new sim::Ship(objectID);
    ship->setSystem(*physics);
    auto object = std::unique_ptr<sim::MovableObject>(ship);
    auto entity = theScene->createEntity(nameBuffer, "spider.mesh");
    MovableParticleSystem* exhaust = entity->attachParticleSystem(
        "Effects/EngineExhaust", Ogre::Vector3(0.0f, -7.0f, 0.0f));
    auto overlay = theScene->createObjectOverlay(nameBuffer);
    overlay->attachCamera(*camPtr);
    overlay->setText("username");
    overlay->setVisible(true);
    overlay->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
    entity->attachObjectOverlay(std::move(overlay));

    return std::make_unique<VisibleObject>(std::move(entity), std::move(object), std::move(exhaust));
}

//Vector3 cameraPos = Vector3::ZERO;
//LocalController* localController = 0;

void loadCollisionGeom(const char* filename);

void clientMain()
{
    signal(SIGINT, signalHandler);

    vol::AABB worldBounds(
        Vector3(-2000.0f, -2000.0f, 0.0f), 
        Vector3(2000.0f, 2000.0f, 0.0f));

    physics = new Physics(worldBounds, "common/data/maps/base03.dat");

    GFXManager gfx;
    auto scene = gfx.createScene();
    auto camera = scene->createCamera("cam1");
    gfx.getViewport().attachCamera(*camera);
    camPtr = camera.get();

    //std::auto_ptr<Entity> spider = scene->createEntity("spider", "warbird.mesh");
    //std::auto_ptr<Entity> warbird = scene->createEntity("warbird", "warbird.mesh");
    //spider->setPosition(Ogre::Vector3(0.0f, 10.0f, 0.0f));
    //warbird->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.0f));
    camera->setPosition(Ogre::Vector3(-0.1f, -0.1f, 200.0f));
    camera->lookAt(Ogre::Vector3(0.0f, 0.0f, 0.0f));
    //scene->setSkyPlane("Sky/Orion", Ogre::Vector3::UNIT_Z, -500.0f);

    //Ship ship(*scene, "username", "spider.mesh");
    //Input input(gfx.getViewport().getRenderWindow());
    Input& input = gfx.getViewport().getInput();
    auto ctrl = std::make_unique<LocalController>();
    input.addKeyboardListener(*ctrl);
    //localController = ctrl.get();
    //ctrl->setObject(&ship);
    theScene = scene.get();

    auto gui = gfx.getViewport().createGUI();
    auto login = gui->createLogin();

#if 0
    while (clientRunning) {
        input.capture();
        gui->render();
        gfx.getViewport().update();
    }
#endif

    scene->addBackdrop("Backdrop/StarFieldBackdrop", 0.0f, -1000.0f, 1500.0f);
    scene->addBackdrop("Backdrop/StarFieldOverlay1", 0.05f, -900.0f, 1350.0f);
    scene->addBackdrop("Backdrop/StarFieldOverlay2", 0.15f, -800.0f, 1200.0f);
    scene->addBackdrop("Backdrop/StarFieldOverlay3", 0.25f, -850.0f, 1100.0f);

    Ogre::Vector3 cameraPos = Ogre::Vector3::ZERO;

    auto map = scene->createEntity("map", "base03.mesh");
    map->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.0f));
    //loadCollisionGeom("maps/base03.dat");

    NetworkInterface network(*login);

    network.maintainServerConnection(true);
    auto hud = gui->createHUD(network, *ctrl);
    input.addKeyboardListener(*hud);
    hudPtr = hud.get();


    Timer simTimer;

    Timer fpsTimer;
    uint64_t frameCount = 0;

    uint64_t targetFPS = 120;
    Timer frameTimer;
    uint64_t framePeriod = 1000000 / targetFPS;


    while (clientRunning) {
        frameTimer.reset();

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

        //const Vector3& shipPos = ship.getApparentPosition();
        //camera->setPosition(Ogre::Vector3(shipPos.x, shipPos.y, shipPos.z + 200.0f));
        //camera->setPosition(Ogre::Vector3(cameraPos.x, cameraPos.y, cameraPos.z + 200.0f));

        if (network.hasServer() && network.getServer().hasAttachedObject()) {
            const Vector3& pos = network.getServer().getAttachedObjectPosition();
            cameraPos.x = pos.x;
            cameraPos.y = pos.y;

            const Vector3& vel = network.getServer().getAttachedObjectVelocity();
            const auto zoom = std::min(magnitude(vel), 700.0f);
            cameraPos.z = pos.z + 300.0f + zoom;

            camera->setPosition(cameraPos);
        }

        input.capture();

        if (network.hasServer())
            network.getServer().setControlState(ctrl->getControlState());

        network.main();
        //ship.update();

        physics->accumulateAndIntegrate();

        scene->updateBackdropPositions(cameraPos);
        gfx.render();
        hud->update();
        gui->render();
        //gfx.getViewport().update();
        frameCount++;

        Ogre::WindowEventUtilities::messagePump();

        if (fpsTimer.elapsed() > 5000000) {
            //int fps = frameCount / 5;
            //if (fps < targetFPS - 1) 
            //    printf("WARNING: Failed to meet target of %dfps\n", int(targetFPS));
            //printf("fps = %d\n", fps);
            frameCount = 0;
            fpsTimer.reset();
        }


        if (frameTimer.elapsed() < framePeriod) 
            usleep(framePeriod - frameTimer.elapsed());
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
    (void)rval;
#endif
    //assert(rval == 0);

    if (argc > 1) 
        serverHostname = argv[1];

    net::initialise();
    
    try {
        clientMain();
    } catch (std::exception& e) {
        Log::log->error(e.what());
    }

    net::cleanup();
    
    Log::log->info("Client stops");
    
    return 0;
}

