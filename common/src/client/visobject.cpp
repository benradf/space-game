/// \file visobject.cpp
/// \brief Unites object graphics and physics.
/// \author Ben Radford 
/// \date 4th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "visobject.hpp"


using namespace sim;


////////// VisibleObject //////////

VisibleObject::VisibleObject(
    std::auto_ptr<gfx::Entity> entity, 
    std::auto_ptr<sim::MovableObject> object) :
    _objectSim(object), _objectGfx(entity),
    _shouldInterpolate(false), _timeToCorrect(0),
    _position(Vector3::ZERO), _rotation(Quaternion::IDENTITY)
{

}

#include <iostream>
using namespace std;
void VisibleObject::update()
{
    _objectSim->update();

    int elapsed = _timer.elapsed();
    if (elapsed > _timeToCorrect) 
        _shouldInterpolate = false;

    if (_shouldInterpolate) {
        float s = float(elapsed) / float(_timeToCorrect);

        _position *= 1.0f - s;
        _position += s * _objectSim->getPosition();

        //TODO: Interpolate rotation.
        _rotation = _objectSim->getRotation();
    } else {
        _position = _objectSim->getPosition();
        _rotation = _objectSim->getRotation();
    }

    _objectGfx->setPosition(Ogre::Vector3(_position.x, _position.y, _position.z));
    _objectGfx->setOrientation(Ogre::Quaternion(_rotation.w, _rotation.x, _rotation.y, _rotation.z));
}

const Vector3& VisibleObject::getApparentPosition() const
{
    return _position;
}

const Quaternion& VisibleObject::getApparentRotation() const
{
    return _rotation;
}

const Vector3& VisibleObject::getPosition() const
{
    return _objectSim->getPosition();
}

const Vector3& VisibleObject::getVelocity() const
{
    return _objectSim->getVelocity();
}

const Quaternion& VisibleObject::getRotation() const
{
    return _objectSim->getRotation();
}

sim::ControlState VisibleObject::getControlState() const
{
    return _objectSim->getControlState();
}

void VisibleObject::setPosition(const Vector3& pos)
{
    _objectSim->setPosition(pos);
    beginInterpolation();
}

void VisibleObject::setVelocity(const Vector3& vel)
{
    _objectSim->setVelocity(vel);
    beginInterpolation();
}

void VisibleObject::setRotation(const Quaternion& rot)
{
    _objectSim->setRotation(rot);
    beginInterpolation();
}

void VisibleObject::setControlState(ControlState control)
{
    _objectSim->setControlState(control);
}

void VisibleObject::beginInterpolation()
{
    _shouldInterpolate = false;
    _timeToCorrect = INTERP_TIME;
    _timer.reset();
}

