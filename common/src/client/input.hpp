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
#include <vecmath.hpp>
#include <tr1/unordered_map>
#include "graphics.hpp"
#include "sim.hpp"
#include <timer.hpp>


enum STAT {
    STAT_THRUST,
    STAT_BOOST,
    STAT_ROTSPEED,
    STAT_MAXSPEED,
    STAT_COUNT
};

enum TURN {
    TURN_CW,
    TURN_ACW,
    TURN_NONE
};


typedef uint32_t ObjectID;


struct MovableObject {
    virtual void setPosition(const Vector3& pos) = 0;
    virtual void setVelocity(const Vector3& vel) = 0;
    virtual void setAcceleration(const Vector3& acc) = 0;
};

struct GuidedMovableObject : public MovableObject {
    virtual void enableThrust(bool enable = true) = 0;
    virtual void enableBoost(bool enable = true) = 0;
    virtual void enableRight(bool enable = true) = 0;
    virtual void enableLeft(bool enable = true) = 0;
};


struct ObjectManager {
    public:
        MovableObject* getMOB(ObjectID id);
        GuidedMovableObject* getGMOB(ObjectID id);



    private:
        std::tr1::unordered_map<ObjectID, MovableObject*> _mobs;
        std::tr1::unordered_map<ObjectID, GuidedMovableObject*> _gmobs;

};


class Ship : public GuidedMovableObject {
    public:
        Ship(gfx::Scene& scene, const char* name, const char* mesh);

        void update();

        virtual void setPosition(const Vector3& pos);
        virtual void setVelocity(const Vector3& vel);
        virtual void setAcceleration(const Vector3& acc);

        virtual void enableThrust(bool enable = true);
        virtual void enableBoost(bool enable = true);
        virtual void enableRight(bool enable = true);
        virtual void enableLeft(bool enable = true);

        const Vector3& getApparentPosition() const {
            return _apparentPos;
        }

    private:
        Ship(const Ship&);
        Ship& operator=(const Ship&);

        std::auto_ptr<gfx::Entity> _gfxEntity;

        sim::Object _simObject;
        Vector3 _apparentPos;
        Quaternion _apparentRot;

        bool _thrustOn;
        bool _boostOn;
        bool _rightOn;
        bool _leftOn;

        Timer _timer;

        float _stats[STAT_COUNT];
};

class LocalController : public OIS::KeyListener {
    public:
        virtual bool keyPressed(const OIS::KeyEvent& arg);
        virtual bool keyReleased(const OIS::KeyEvent& arg);

        void setObject(GuidedMovableObject* object);

    private:
        GuidedMovableObject* _object;
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

