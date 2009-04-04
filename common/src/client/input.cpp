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
#include <object.hpp>


using namespace sim;


////////// RemoteController //////////

void RemoteController::setPosition(const Vector3& pos)
{
    assert(_object != 0);

    _object->setPosition(pos);
}

void RemoteController::setVelocity(const Vector3& vel)
{
    assert(_object != 0);

    _object->setVelocity(vel);
}

void RemoteController::setRotation(const Quaternion& rot)
{
    assert(_object != 0);

    _object->setRotation(rot);
}

void RemoteController::setState(uint32_t flags)
{
    assert(_object != 0);

    _object->setControlState(flags);
}

void RemoteController::setObject(sim::MovableObject* object)
{
    _object = object;
}


////////// LocalController //////////

LocalController::LocalController() :
    _object(0), _state(0)
{

}

bool LocalController::keyPressed(const OIS::KeyEvent& arg)
{
    switch (arg.key) {
        case OIS::KC_UP:
            _state = controlSet(CTRL_THRUST, true, _state);
            break;
        case OIS::KC_LSHIFT:
            _state = controlSet(CTRL_BOOST, true, _state);
            break;
        case OIS::KC_LEFT:
            _state = controlSet(CTRL_LEFT, true, _state);
            break;
        case OIS::KC_RIGHT:
            _state = controlSet(CTRL_RIGHT, true, _state);
            break;
    }

    if (_object != 0) 
        _object->setControlState(_state);

    return true;
}

bool LocalController::keyReleased(const OIS::KeyEvent& arg)
{
    switch (arg.key) {
        case OIS::KC_UP:
            _state = controlSet(CTRL_THRUST, false, _state);
            break;
        case OIS::KC_LSHIFT:
            _state = controlSet(CTRL_BOOST, false, _state);
            break;
        case OIS::KC_LEFT:
            _state = controlSet(CTRL_LEFT, false, _state);
            break;
        case OIS::KC_RIGHT:
            _state = controlSet(CTRL_RIGHT, false, _state);
            break;
    }

    if (_object != 0) 
        _object->setControlState(_state);

    return true;
}

void LocalController::setObject(sim::MovableObject* object)
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

