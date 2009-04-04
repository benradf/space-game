/// \file object.cpp
/// \brief Implements the various game objects.
/// \author Ben Radford 
/// \date 3rd April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "object.hpp"
#include <core.hpp>



////////// MovableObject //////////

sim::MovableObject::~MovableObject()
{

}


////////// Ship //////////

sim::Ship::Ship() :
    _simObject(100.0f), _control(0)
{
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

    if (controlIsOn(CTRL_THRUST, _control)) 
        _simObject.ApplyRelativeForce(Vector3::UNIT_Y * _stats[STAT_THRUST]);

    if (controlIsOn(CTRL_BOOST, _control) && controlIsOn(CTRL_THRUST, _control)) 
        _simObject.ApplyRelativeForce(Vector3::UNIT_Y * _stats[STAT_BOOST]);

    if (controlIsOn(CTRL_LEFT, _control) && !controlIsOn(CTRL_RIGHT, _control)) 
        _simObject.ApplySpin(Quaternion(_stats[STAT_ROTSPEED], Vector3::UNIT_Z));

    if (controlIsOn(CTRL_RIGHT, _control) && !controlIsOn(CTRL_LEFT, _control)) 
        _simObject.ApplySpin(Quaternion(-_stats[STAT_ROTSPEED], Vector3::UNIT_Z));

    float elapsed = _timer.elapsed();
    _simObject.integrate(elapsed / 1000000);
    _timer.reset();
}

const Vector3& sim::Ship::getPosition()
{
    return _simObject.getPosition();
}

const Vector3& sim::Ship::getVelocity()
{
    return _simObject.getVelocity();
}

const Vector3& sim::Ship::getAcceleration()
{
    return _simObject.getAcceleration();
}

const Quaternion& sim::Ship::getRotation()
{
    return _simObject.getRotation();
}

void sim::Ship::setPosition(const Vector3& pos)
{
    _simObject.setPosition(pos);
}

void sim::Ship::setVelocity(const Vector3& vel)
{
    _simObject.setVelocity(vel);
}

void sim::Ship::setRotation(const Quaternion& rot)
{
    _simObject.setRotation(rot);
}

void sim::Ship::setControlState(ControlState control)
{
    _control = control;
}

