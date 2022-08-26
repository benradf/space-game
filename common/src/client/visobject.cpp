/// \file visobject.cpp
/// \brief Unites object graphics and physics.
/// \author Ben Radford 
/// \date 4th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "visobject.hpp"
#include <math/interp.hpp>
#include <limits>


using namespace sim;


////////// VisibleObject //////////

VisibleObject::VisibleObject(
        std::unique_ptr<gfx::Entity> entity, 
        std::unique_ptr<sim::MovableObject> object,
        gfx::MovableParticleSystem* exhaust) :
    _objectSim(std::move(object)),
    _objectGfx(std::move(entity)),
    _exhaust(exhaust), 
    _predictedPos(Vector3::ZERO),
    _apparentPos(Vector3::ZERO), 
    _shouldInterpPos(false),
    _timeToInterpPos(0),
    _predictedRot(Quaternion::IDENTITY), 
    _apparentRot(Quaternion::IDENTITY),
    _shouldInterpRot(false),
    _timeToInterpRot(0),
    _roll(Quaternion::IDENTITY),
    _rollFrom(0.0f),
    _rollTo(0.0f)
{
    assert(_objectSim.get() != 0);
    assert(_objectGfx.get() != 0);
    assert(_exhaust != 0);
}

void VisibleObject::update()
{
    _objectSim->update();
    updateBankedTurnRoll();
    updateApparentRotation();
    updateApparentPosition();

    ControlState state = _objectSim->getControlState();
    const Vector3& position = getApparentPosition();
    const Vector3& velocity = _objectSim->getVelocity();
    const Vector3& acceleration = _objectSim->getAcceleration();
    const Quaternion& rotation = getApparentRotation();

    _objectGfx->setPosition(Ogre::Vector3(position.x, position.y, position.z));
    _objectGfx->setOrientation(Ogre::Quaternion(rotation.w, rotation.x, rotation.y, rotation.z));
    _objectGfx->updateParticleSystems(Ogre::Vector3(velocity.x, velocity.y, velocity.z));
    _objectGfx->updateObjectOverlay();

    _exhaust->setEnabled(controlIsOn(CTRL_THRUST, state));
}

const Vector3& VisibleObject::getApparentPosition() const
{
    return _apparentPos;
}

const Quaternion& VisibleObject::getApparentRotation() const
{
    return _apparentRot;
}

float VisibleObject::getRotation() const
{
    return _objectSim->getRotation();
}

const Vector3& VisibleObject::getPosition() const
{
    return _objectSim->getPosition();
}

const Vector3& VisibleObject::getVelocity() const
{
    return _objectSim->getVelocity();
}

sim::ControlState VisibleObject::getControlState() const
{
    return _objectSim->getControlState();
}

void VisibleObject::setPosition(const Vector3& pos)
{
    if (magnitudeSq(_objectSim->getPosition() - pos) > 1.0f)
        beginInterpPosition();

    _objectSim->setPosition(pos);

}

void VisibleObject::setVelocity(const Vector3& vel)
{
    _objectSim->setVelocity(vel);
}

void VisibleObject::setRotation(float rotation)
{
    _objectSim->setRotation(rotation);

    beginInterpRotation();
}

void VisibleObject::setControlState(ControlState control)
{
    _objectSim->setControlState(control);
}

const std::string& VisibleObject::getName() const
{
    return _name;
}

void VisibleObject::setName(const std::string& name)
{
    _name = name;

    if (_objectGfx->hasObjectOverlay()) 
        _objectGfx->getObjectOverlay().setText(name.c_str());
}

void VisibleObject::beginInterpPosition()
{
    _predictedPos = _apparentPos;
    _shouldInterpPos = true;
    _timeToInterpPos = INTERP_TIME;
    _interpPosTimer.reset();
}

void VisibleObject::beginInterpRotation()
{
    _predictedRot = _apparentRot;
    _shouldInterpRot = true;
    _timeToInterpRot = INTERP_TIME;
    _interpRotTimer.reset();
}

void VisibleObject::updateApparentPosition()
{
    _apparentPos = _objectSim->getPosition();

    if (!_shouldInterpPos) 
        return;

    uint64_t elapsed = _interpPosTimer.elapsed();
    if (elapsed > _timeToInterpPos) {
        _shouldInterpPos = false;
        return;
    }

    float t = float(elapsed) / float(_timeToInterpPos);
    _apparentPos = lerp(_predictedPos, _apparentPos, t);
}

void VisibleObject::updateApparentRotation()
{
    _apparentRot = _objectSim->getRotationQuat() * _roll;

    if (!_shouldInterpRot) 
        return;

    uint64_t elapsed = _interpRotTimer.elapsed();
    if (elapsed > _timeToInterpRot) {
        _shouldInterpRot = false;
        return;
    }

    float t = float(elapsed) / float(_timeToInterpRot);
    _apparentRot = slerpMin(_predictedRot, _apparentRot, t);
}

void VisibleObject::updateBankedTurnRoll()
{
    Vector3 right = getApparentRotation() * Vector3::UNIT_X;

    Vector3 velocity = _objectSim->getVelocity();
    float velMagSq = magnitudeSq(velocity);

    if (velMagSq > std::numeric_limits<float>::epsilon()) 
        velocity.normalise();

    float dp = dotProduct(velocity, right);

    if ((fabs(dp) < 0.5) || !controlIsOn(CTRL_THRUST, _objectSim->getControlState()))
        dp = 0.0f;

    _rollTo = dp * std::min(magnitude(_objectSim->getVelocity()), 100.0f) / 100.0f;
    _rollFrom = math::lerp(_rollFrom, _rollTo, 0.0125f);
    
    _roll = Quaternion(-0.5f * M_PI * _rollFrom, Vector3::UNIT_Y);
}

