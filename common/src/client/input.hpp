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


namespace CEGUI { class System; }


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

        void setEnabled(bool enabled);
        sim::ControlState getControlState() const;

        virtual bool keyPressed(const OIS::KeyEvent& arg);
        virtual bool keyReleased(const OIS::KeyEvent& arg);

    private:
        bool _enabled;
        sim::ControlState _state;
};


class CEGUIInput : public OIS::KeyListener, public OIS::MouseListener {
    public:
        CEGUIInput(CEGUI::System& system);

        virtual bool keyPressed(const OIS::KeyEvent& arg);
        virtual bool keyReleased(const OIS::KeyEvent& arg);

        virtual bool mouseMoved(const OIS::MouseEvent &arg);
        virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
        virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

    private:
        CEGUI::System& _system;
};


class Input : public OIS::KeyListener, public OIS::MouseListener {
    public:
        Input(Ogre::RenderWindow* window);
        ~Input();

        void updateMouseClipping();

        template<typename T>
        void addKeyboardListener(T& listener);

        template<typename T>
        void addMouseListener(T& listener);

        void capture();

    private:
        virtual bool keyPressed(const OIS::KeyEvent& arg);
        virtual bool keyReleased(const OIS::KeyEvent& arg);

        virtual bool mouseMoved(const OIS::MouseEvent &arg);
        virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
        virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

        std::vector<OIS::KeyListener*> _keyListeners;
        std::vector<OIS::MouseListener*> _mouseListeners;

        Ogre::RenderWindow* _window;
        OIS::InputManager* _inputManager;
        OIS::Keyboard* _keyboard;
        OIS::Mouse* _mouse;
};


////////// Input //////////

template<typename T>
inline void Input::addKeyboardListener(T& listener)
{
    _keyListeners.push_back(&listener);
}

template<typename T>
inline void Input::addMouseListener(T& listener)
{
    _mouseListeners.push_back(&listener);
}


#endif  // INPUT_HPP

