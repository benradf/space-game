/// \file sim.cpp
/// \brief Physics simulation.
/// \author Ben Radford 
/// \date 25th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "sim.hpp"
#include "collision.hpp"

#include <unistd.h>
#include <core/timer.hpp>
#include <core/core.hpp>


CollisionGeometry collision("collision.dat");


////////// sim::Object //////////

sim::Object::Object(float mass) :
    _pos(Vector3::ZERO), _vel(Vector3::ZERO), _acc(Vector3::ZERO),
    _rot(Quaternion::IDENTITY), _spin(Quaternion::IDENTITY), 
    _forceApplied(false), _spinApplied(false),
    _mass(mass), _massInverse(1.0f / mass)
{

}

sim::Object::~Object()
{

}

void sim::Object::integrate(float dt)
{
    integrateLinearMotion(dt);
    integrateRotationalMotion(dt);
}

void sim::Object::ApplyAbsoluteForce(const Vector3& force)
{
    _acc += _massInverse * force;
    _forceApplied = true;
}

void sim::Object::ApplyRelativeForce(const Vector3& force)
{
    ApplyAbsoluteForce(_rot * force);
}

void sim::Object::ApplySpin(const Quaternion& spin)
{
    _spin *= spin;
    _spinApplied = true;
}

const Vector3& sim::Object::getPosition() const
{
    return _pos;
}

const Vector3& sim::Object::getVelocity() const
{
    return _vel;
}

const Vector3& sim::Object::getAcceleration() const
{
    return _acc;
}

const Quaternion& sim::Object::getRotation() const
{
    return _rot;
}

const Quaternion& sim::Object::getSpin() const
{
    return _spin;
}

void sim::Object::setPosition(const Vector3& pos)
{
    _pos = pos;
}

void sim::Object::setVelocity(const Vector3& vel)
{
    _vel = vel;
}

void sim::Object::setAcceleration(const Vector3& acc)
{
    _acc = acc;
    _forceApplied = true;
}

void sim::Object::setRotation(const Quaternion& rot)
{
    _rot = rot;
}

void sim::Object::setSpin(const Quaternion& spin)
{
    _spin = spin;
    _spinApplied = true;
}

void sim::Object::ClearForce()
{
    _acc = Vector3::ZERO;
    _forceApplied = false;
}

void sim::Object::ClearSpin()
{
    _spin = Quaternion::IDENTITY;
    _spinApplied = false;
}

void sim::Object::integrateLinearMotion(float dt)
{
    if (_forceApplied) 
        _vel += _acc * dt;

    Vector3 normal = Vector3::ZERO;
    Vector3 newPos = _pos + _vel * dt;
    if (!collision.checkCollision(vol::Sphere(newPos, 8.0f), normal)) {
        _pos = newPos;
    } else {
        _vel = Vector3::ZERO;
    }

    ClearForce();
}

void sim::Object::integrateRotationalMotion(float dt)
{
    if (!_spinApplied) 
        return;

    Vector3 turnAxis(_spin.x, _spin.y, _spin.z);
    if (magnitudeSq(turnAxis) < 0.001f) 
        return;

    float turnAngle = 2.0f * acos(_spin.w) * dt;
    _rot *= Quaternion(turnAngle, turnAxis.normalise());

    ClearSpin();
}

