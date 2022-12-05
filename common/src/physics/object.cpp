/// \file object.cpp
/// \brief Implements the various game objects.
/// \author Ben Radford 
/// \date 3rd April 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#include "object.hpp"
#include <core/core.hpp>



////////// MovableObject //////////

sim::MovableObject::~MovableObject()
{

}


////////// Ship //////////

sim::Ship::Ship(ObjectID id) :
    _control(0), _id(id)
{
    setMass(10.0f);
    setRadius(8.0f);

    _stats[STAT_THRUST] = 150.0f;
    _stats[STAT_BOOST] = 500.0f;
    _stats[STAT_ROTSPEED] = DEG2RAD(120.0f);
    _stats[STAT_MAXSPEED] = 15.0f;
}

sim::Ship::~Ship()
{

}

void sim::Ship::update()
{
    if (_timer.elapsed() < 0.01f) 
        return;

    ClearForce();
    ClearSpin();

    if (controlIsOn(CTRL_THRUST, _control)) 
        ApplyRelativeForce(Vector3::UNIT_Y * _stats[STAT_THRUST]);

    if (controlIsOn(CTRL_BOOST, _control) && controlIsOn(CTRL_THRUST, _control)) 
        ApplyRelativeForce(Vector3::UNIT_Y * _stats[STAT_BOOST]);

    if (controlIsOn(CTRL_LEFT, _control) && !controlIsOn(CTRL_RIGHT, _control)) 
        ApplySpin(_stats[STAT_ROTSPEED]);

    if (controlIsOn(CTRL_RIGHT, _control) && !controlIsOn(CTRL_LEFT, _control)) 
        ApplySpin(-_stats[STAT_ROTSPEED]);
}

void sim::Ship::setSystem(Physics& system)
{
    system.registerBody(*this);
}

sim::ObjectID sim::Ship::getID() const
{
    return _id;
}

sim::ControlState sim::Ship::getControlState() const
{
    return _control;
}

void sim::Ship::setControlState(ControlState control)
{
    _control = control;
}

void sim::Ship::collisionWith(const Ship& other)
{

}

void sim::Ship::collisionWith(const RigidBody& other)
{

}

void sim::Ship::collisionDispatch(RigidBody& other) const
{
    other.collisionWith(*this);
}

