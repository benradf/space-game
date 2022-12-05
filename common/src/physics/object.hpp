/// \file object.hpp
/// \brief Implements the various game objects.
/// \author Ben Radford 
/// \date 3rd April 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef OBJECT_HPP
#define OBJECT_HPP


#include "sim.hpp"
#include <stdint.h>
#include <core/timer.hpp>


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


struct MovableObject : public RigidBody {
    virtual ~MovableObject();
    virtual void update() = 0;
    virtual ObjectID getID() const = 0;
    virtual ControlState getControlState() const = 0;
    virtual void setControlState(ControlState control) = 0;
};


class Ship : public MovableObject {
    public:
        Ship(ObjectID id);
        virtual ~Ship();

        virtual void update();
        virtual ObjectID getID() const;
        virtual ControlState getControlState() const;
        virtual void setControlState(ControlState control);

        virtual void collisionWith(const Ship& other);
        virtual void collisionWith(const RigidBody& other);
        virtual void collisionDispatch(RigidBody& other) const;

        void setSystem(Physics& system);

    private:
        ControlState _control;
        Timer _timer;

        ObjectID _id;

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

