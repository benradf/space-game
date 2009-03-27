/// \file input.cpp
/// \brief Handles user input.
/// \author Ben Radford 
/// \date 27th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "input.hpp"
#include <core.hpp>
#include <sstream>


////////// Ship //////////

Ship::Ship(gfx::Scene& scene, const char* name, const char* mesh) :
    _apparentPos(Vector3::ZERO), _apparentRot(Quaternion::IDENTITY), 
    _simObject(10.0f), _thrustOn(false), _boostOn(false),
    _rightOn(false), _leftOn(false)
{
    _gfxEntity = scene.createEntity(name, mesh);

    _stats[STAT_THRUST] = 150.0f;
    _stats[STAT_BOOST] = 500.0f;
    _stats[STAT_ROTSPEED] = DEG2RAD(120.0f);
    _stats[STAT_MAXSPEED] = 15.0f;
}

void Ship::update()
{
    if (_timer.elapsed() < 0.01f) 
        return;

    if (_thrustOn)
        _simObject.ApplyRelativeForce(Vector3::UNIT_Y * _stats[STAT_THRUST]);

    if (_thrustOn && _boostOn) 
        _simObject.ApplyRelativeForce(Vector3::UNIT_Y * _stats[STAT_BOOST]);

    if (_rightOn)
        _simObject.ApplySpin(Quaternion(-_stats[STAT_ROTSPEED], Vector3::UNIT_Z));

    if (_leftOn)
        _simObject.ApplySpin(Quaternion(_stats[STAT_ROTSPEED], Vector3::UNIT_Z));

    float elapsed = _timer.elapsed();
    _simObject.integrate(elapsed / 1000000.0f);
    _timer.reset();

    _apparentPos = _simObject.getPosition();
    _apparentRot = _simObject.getRotation();

    _gfxEntity->setPosition(Ogre::Vector3(
        _apparentPos.x, _apparentPos.y, _apparentPos.z));

    _gfxEntity->setOrientation(Ogre::Quaternion(
        _apparentRot.w, _apparentRot.x, _apparentRot.y, _apparentRot.z));
}

void Ship::setPosition(const Vector3& pos)
{
    _simObject.setPosition(pos);
}

void Ship::setVelocity(const Vector3& vel)
{
    _simObject.setVelocity(vel);
}

void Ship::setAcceleration(const Vector3& acc)
{
    _simObject.setAcceleration(acc);
}

void Ship::enableThrust(bool enable)
{
    _thrustOn = enable;
}

void Ship::enableBoost(bool enable)
{
    _boostOn = enable;
}

void Ship::enableRight(bool enable)
{
    _rightOn = enable;
}

void Ship::enableLeft(bool enable)
{
    _leftOn = enable;
}


////////// LocalController //////////

bool LocalController::keyPressed(const OIS::KeyEvent& arg)
{
    if (_object == 0) 
        return false;

    switch (arg.key) {
        case OIS::KC_UP:
            _object->enableThrust(true);
            break;
        case OIS::KC_LSHIFT:
            _object->enableBoost(true);
            break;
        case OIS::KC_LEFT:
            _object->enableLeft(true);
            break;
        case OIS::KC_RIGHT:
            _object->enableRight(true);
            break;
    }

    return true;
}

bool LocalController::keyReleased(const OIS::KeyEvent& arg)
{
    if (_object == 0) 
        return false;

    switch (arg.key) {
        case OIS::KC_UP:
            _object->enableThrust(false);
            break;
        case OIS::KC_LSHIFT:
            _object->enableBoost(false);
            break;
        case OIS::KC_LEFT:
            _object->enableLeft(false);
            break;
        case OIS::KC_RIGHT:
            _object->enableRight(false);
            break;
    }

    return true;
}

void LocalController::setObject(GuidedMovableObject* object)
{
    _object = object;
}


////////// Input //////////

Input::Input(Ogre::RenderWindow* window)
{
    size_t windowHandle = 0;
    window->getCustomAttribute("WINDOW", &windowHandle);

    OIS::ParamList params;
    std::ostringstream windowHandleStr;
    windowHandleStr << windowHandle;
    params.insert(std::make_pair(std::string("WINDOW"), windowHandleStr.str()));
    params.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
    params.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false"))); 
    params.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));

    _inputManager = OIS::InputManager::createInputSystem(params);

    _mouse = static_cast<OIS::Mouse*>(
        _inputManager->createInputObject(OIS::OISMouse, true));
    _keyboard = static_cast<OIS::Keyboard*>(
        _inputManager->createInputObject(OIS::OISKeyboard, true));
}

Input::~Input()
{
    _inputManager->destroyInputObject(_mouse);
    _inputManager->destroyInputObject(_keyboard);
    OIS::InputManager::destroyInputSystem(_inputManager);
}

void Input::capture()
{
    _keyboard->capture();
    _mouse->capture();

    //if (_keyboard->isKeyDown(OIS::KC_ESCAPE)) 
    //    throw Exception("quitting");
}

