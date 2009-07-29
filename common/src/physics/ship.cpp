/// \file ship.cpp
/// \brief Implementation of space ships.
/// \author Ben Radford 
/// \date 28th July 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "ship.hpp"


////////// Ship //////////

physics::Ship::Ship(ShipID id) :
    _control(0), _id(id)
{
    setMass(10.0f);
    setRadius(8.0f);

    _stats[STAT_THRUST] = 150.0f;
    _stats[STAT_BOOST] = 500.0f;
    _stats[STAT_ROTSPEED] = DEG2RAD(120.0f);
    _stats[STAT_MAXSPEED] = 15.0f;
}

physics::Ship::~Ship()
{

}

void physics::Ship::updateState()
{
    if (_timer.elapsed() < 0.01f) 
        return;

    clearForce();
    clearSpin();

    if (controlIsOn(CTRL_THRUST, _control)) 
        applyRelativeForce(Vector3::UNIT_Y * _stats[STAT_THRUST]);

    if (controlIsOn(CTRL_BOOST, _control) && controlIsOn(CTRL_THRUST, _control)) 
        applyRelativeForce(Vector3::UNIT_Y * _stats[STAT_BOOST]);

    if (controlIsOn(CTRL_LEFT, _control) && !controlIsOn(CTRL_RIGHT, _control)) 
        applySpin(_stats[STAT_ROTSPEED]);

    if (controlIsOn(CTRL_RIGHT, _control) && !controlIsOn(CTRL_LEFT, _control)) 
        applySpin(-_stats[STAT_ROTSPEED]);
}

void physics::Ship::setSystem(Physics& system)
{
    system.registerBody(*this);
}

physics::ShipID physics::Ship::getID() const
{
    return _id;
}

physics::ControlState physics::Ship::getControlState() const
{
    return _control;
}

void physics::Ship::collision(Ship& ship)
{

}

void physics::Ship::collision(Missile& missile)
{

}

void physics::Ship::collision(const Vector3& normal)
{

}

