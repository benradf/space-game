/// \file input.hpp
/// \brief Handles user input.
/// \author Ben Radford 
/// \date 27th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef INPUT_HPP
#define INPUT_HPP


#include <OIS/OIS.h>
#include <math/vecmath.hpp>
#include <tr1/unordered_map>
#include "graphics.hpp"
#include <physics/object.hpp>


class RemoteController {
    public:
        RemoteController();

        void setRotation(float rotation);
        void setPosition(const Vector3& pos);
        void setVelocity(const Vector3& vel);
        void setState(uint32_t flags);

        void setObject(sim::MovableObject* object);

    private:
        sim::MovableObject* _object;
};

class LocalController : public OIS::KeyListener {
    public:
        LocalController();

        sim::ControlState getControlState() const;

        virtual bool keyPressed(const OIS::KeyEvent& arg);
        virtual bool keyReleased(const OIS::KeyEvent& arg);

    private:
        sim::ControlState _state;
};


class Input {
    public:
        Input(Ogre::RenderWindow* window);
        ~Input();

        template<typename T>
        std::auto_ptr<T> createKeyboardListener();

        template<typename T>
        std::auto_ptr<T> createMouseListener();

        template<typename T>
        std::auto_ptr<T> createInputListener();

        void capture();

    private:
        OIS::InputManager* _inputManager;
        OIS::Keyboard* _keyboard;
        OIS::Mouse* _mouse;
};


////////// Input //////////

template<typename T>
inline std::auto_ptr<T> Input::createKeyboardListener()
{
    std::auto_ptr<T> p(new T);
    _keyboard->setEventCallback(p.get());

    return p;
}

template<typename T>
inline std::auto_ptr<T> Input::createMouseListener()
{
    std::auto_ptr<T> p(new T);
    _mouse->setEventCallback(p.get());

    return p;
}

template<typename T>
inline std::auto_ptr<T> Input::createInputListener()
{
    std::auto_ptr<T> p(new T);
    _keyboard->setEventCallback(p.get());
    _mouse->setEventCallback(p.get());

    return p;
}


#endif  // INPUT_HPP

