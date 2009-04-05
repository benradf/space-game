/// \file object.hpp
/// \brief Implements the various game objects.
/// \author Ben Radford 
/// \date 3rd April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef OBJECT_HPP
#define OBJECT_HPP


#include <sim.hpp>
#include <stdint.h>
#include <timer.hpp>


namespace sim {


typedef uint32_t ObjectID;
typedef uint32_t ControlState;


enum STAT {
    STAT_THRUST,
    STAT_BOOST,
    STAT_ROTSPEED,
    STAT_MAXSPEED,
    STAT_COUNT
};


enum Control {
    CTRL_THRUST = 0x00000001,
    CTRL_BOOST  = 0x00000002,
    CTRL_LEFT   = 0x00000004,
    CTRL_RIGHT  = 0x00000008,
    CTRL_CANNON = 0x00000010,
};


struct MovableObject {
    virtual ~MovableObject();
    virtual void update() = 0;
    virtual const Vector3& getPosition() const = 0;
    virtual const Vector3& getVelocity() const = 0;
    virtual const Vector3& getAcceleration() const = 0;
    virtual const Quaternion& getRotation() const = 0;
    virtual ControlState getControlState() const = 0;
    virtual void setPosition(const Vector3& pos) = 0;
    virtual void setVelocity(const Vector3& vel) = 0;
    virtual void setRotation(const Quaternion& rot) = 0;
    virtual void setControlState(ControlState control) = 0;
};


class Ship : public MovableObject {
    public:
        Ship();
        virtual ~Ship();

        virtual void update();

        virtual const Vector3& getPosition() const;
        virtual const Vector3& getVelocity() const;
        virtual const Vector3& getAcceleration() const;
        virtual const Quaternion& getRotation() const;
        virtual ControlState getControlState() const;

        virtual void setPosition(const Vector3& pos);
        virtual void setVelocity(const Vector3& vel);
        virtual void setRotation(const Quaternion& rot);
        virtual void setControlState(ControlState control);

    private:
        sim::Object _simObject;
        ControlState _control;
        Timer _timer;

        float _stats[STAT_COUNT];
};


inline bool controlIsOn(Control control, ControlState state)
{
    return ((state & control) != 0);
}

inline ControlState controlSet(Control control, bool enable, ControlState state)
{
    return (enable ? state | control : state & ~control);
}


}  // namespace sim


#endif  // OBJECT_HPP

