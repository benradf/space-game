/// \file input.cpp
/// \brief Handles user input.
/// \author Ben Radford 
/// \date 27th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "input.hpp"
#include <core/core.hpp>
#include <sstream>
#include <physics/object.hpp>
#include <CEGUI/CEGUI.h>


using namespace sim;


////////// RemoteController //////////

void RemoteController::setRotation(float rotation)
{
    assert(_object != 0);

    _object->setRotation(rotation);
}

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
    _enabled(true), _state(0)
{

}

void LocalController::setEnabled(bool enabled)
{
    _enabled = enabled;
}

sim::ControlState LocalController::getControlState() const
{
    return _state;
}

bool LocalController::keyPressed(const OIS::KeyEvent& arg)
{
    if (!_enabled) 
        return true;

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

    return true;
}


////////// CEGUIInput //////////

CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID) {
        case OIS::MB_Left:   return CEGUI::LeftButton;
        case OIS::MB_Right:  return CEGUI::RightButton;
        case OIS::MB_Middle: return CEGUI::MiddleButton;
        default:             return CEGUI::LeftButton;
    }
}

CEGUIInput::CEGUIInput(CEGUI::System& system) :
    _system(system)
{

}

bool CEGUIInput::keyPressed(const OIS::KeyEvent& arg)
{
    _system.injectChar(arg.text);
    return _system.injectKeyDown(arg.key);
}

bool CEGUIInput::keyReleased(const OIS::KeyEvent& arg)
{
    return _system.injectKeyUp(arg.key);
}

bool CEGUIInput::mouseMoved(const OIS::MouseEvent &arg)
{
    return _system.injectMousePosition(arg.state.X.abs, arg.state.Y.abs);
}

bool CEGUIInput::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    return _system.injectMouseButtonDown(convertButton(id));
}

bool CEGUIInput::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    return _system.injectMouseButtonUp(convertButton(id));
}


////////// Input //////////

Input::Input(Ogre::RenderWindow* window) :
    _window(window)
{
    size_t windowHandle = 0;
    window->getCustomAttribute("WINDOW", &windowHandle);

    OIS::ParamList params;
    std::ostringstream windowHandleStr;
    windowHandleStr << windowHandle;
    params.insert(std::make_pair(std::string("WINDOW"), windowHandleStr.str()));
    params.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
    params.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("true"))); 
    params.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
    params.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
    params.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
    params.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
    params.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));

    _inputManager = OIS::InputManager::createInputSystem(params);

    _mouse = static_cast<OIS::Mouse*>(
        _inputManager->createInputObject(OIS::OISMouse, true));
    _keyboard = static_cast<OIS::Keyboard*>(
        _inputManager->createInputObject(OIS::OISKeyboard, true));

    _keyboard->setEventCallback(this);
    _mouse->setEventCallback(this);

    updateMouseClipping();
}

Input::~Input()
{
    _inputManager->destroyInputObject(_mouse);
    _inputManager->destroyInputObject(_keyboard);
    OIS::InputManager::destroyInputSystem(_inputManager);
}

void Input::updateMouseClipping()
{
    _mouse->getMouseState().width = _window->getWidth();
    _mouse->getMouseState().height = _window->getHeight();
}

void Input::capture()
{
    _keyboard->capture();
    _mouse->capture();

    extern bool clientRunning;
    if (_keyboard->isKeyDown(OIS::KC_ESCAPE)) 
        clientRunning = false;
}

bool Input::keyPressed(const OIS::KeyEvent& arg)
{
    foreach (OIS::KeyListener* listener, _keyListeners) 
        listener->keyPressed(arg);
}

bool Input::keyReleased(const OIS::KeyEvent& arg)
{

    foreach (OIS::KeyListener* listener, _keyListeners) 
        listener->keyReleased(arg);
}

bool Input::mouseMoved(const OIS::MouseEvent &arg)
{

    foreach (OIS::MouseListener* listener, _mouseListeners) 
        listener->mouseMoved(arg);
}

bool Input::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{

    foreach (OIS::MouseListener* listener, _mouseListeners) 
        listener->mousePressed(arg, id);
}

bool Input::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    foreach (OIS::MouseListener* listener, _mouseListeners) 
        listener->mouseReleased(arg, id);
}

