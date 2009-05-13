/// \file sim.cpp
/// \brief Physics simulation.
/// \author Ben Radford 
/// \date 25th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "sim.hpp"
#include "collision.hpp"
#include "physunits.hpp"

#include <unistd.h>
#include <core/timer.hpp>
#include <core/core.hpp>
#include <math/interp.hpp>


using math::lerp;


////////// RigidBody //////////

RigidBody::RigidBody() :
    _system(0)
{

}

RigidBody::~RigidBody()
{
    if (_system != 0) 
        _system->deregisterBody(*this);
}

void RigidBody::integrate(float dt)
{
    advanceCurrentState();
    integrateLinearMotion(dt);
    integrateRotationalMotion(dt);
}

void RigidBody::blendStates(float t)
{
    State& current = getCurrentState();
    State& blended = getBlendedState();
    State& previous = getPreviousState();

    blended.pos = lerp(previous.pos, current.pos, t);
    blended.vel = lerp(previous.vel, current.vel, t);
    blended.acc = lerp(previous.acc, current.acc, t);
    blended.rotQuat = slerpMin(previous.rotQuat, current.rotQuat, t);
    blended.rot = lerp(previous.rot, current.rot, t);
    blended.spin = lerp(previous.spin, current.spin, t);
    blended.mass = lerp(previous.mass, current.mass, t);
    blended.massInv = lerp(previous.massInv, current.massInv, t);
    blended.radius = lerp(previous.radius, current.radius, t);
}

void RigidBody::ApplyAbsoluteForce(const Vector3& force)
{
    State& state = getCurrentState();

    state.acc += state.massInv * force;
}

void RigidBody::ApplyRelativeForce(const Vector3& force)
{
    ApplyAbsoluteForce(getCurrentState().rotQuat * force);
}

void RigidBody::ApplySpin(float spin)
{
    getCurrentState().spin += spin;
}

const Vector3& RigidBody::getPosition() const
{
    return getBlendedState().pos;
}

const Vector3& RigidBody::getVelocity() const
{
    return getBlendedState().vel;
}

const Vector3& RigidBody::getAcceleration() const
{
    return getBlendedState().acc;
}

const Quaternion& RigidBody::getRotationQuat() const
{
    return getBlendedState().rotQuat;
}

float RigidBody::getRotation() const
{
    return getBlendedState().rot;
}

float RigidBody::getSpin() const
{
    return getBlendedState().spin;
}

float RigidBody::getMass() const
{
    return getBlendedState().mass;
}

float RigidBody::getRadius() const
{
    return getBlendedState().radius;
}

void RigidBody::setPosition(const Vector3& pos)
{
    getCurrentState().pos = pos;
    getPreviousState().pos = pos;
}

void RigidBody::setVelocity(const Vector3& vel)
{
    getCurrentState().vel = vel;
    getPreviousState().vel = vel;
}

void RigidBody::setAcceleration(const Vector3& acc)
{
    getCurrentState().acc = acc;
    getPreviousState().acc = acc;
}

void RigidBody::setRotation(float rot)
{
    getCurrentState().rot = rot;
    getPreviousState().rot = rot;
}

void RigidBody::setSpin(float spin)
{
    getCurrentState().spin = spin;
    getPreviousState().spin = spin;
}

void RigidBody::setMass(float mass)
{
    getCurrentState().mass = mass;
    getPreviousState().mass = mass;
    getCurrentState().massInv = 1.0f / mass;
    getPreviousState().massInv = 1.0f / mass;
}

void RigidBody::setRadius(float radius)
{
    getCurrentState().radius = radius;
    getPreviousState().radius = radius;
}

void RigidBody::ClearForce()
{
    getCurrentState().acc = Vector3::ZERO;
}

void RigidBody::ClearSpin()
{
    getCurrentState().spin = 0.0f;
}

void RigidBody::integrateLinearMotion(float dt)
{
    if (!needsLinearIntegration())
        return;

    State& state = getCurrentState();

    Vector3 prevPos = state.pos;

    state.pos += state.vel * dt;
    state.vel += state.acc * dt;

    if (_system == 0) 
        return;

    const CollisionGeometry& geom = _system->getCollisionGeom();

    Vector3 normal = Vector3::ZERO;
    vol::Sphere sphere(state.pos, state.radius);

    if (geom.checkCollision(sphere, normal)) {
        state.vel = 0.75f * reflect(state.vel, normal);
        state.pos = prevPos;
    }
}

void RigidBody::integrateRotationalMotion(float dt)
{
    if (!needsRotationalIntegration())
        return;

    State& state = getCurrentState();

    state.rot = fmod(state.rot + state.spin * dt, 2 * M_PI);

    if (state.rot < 0.0f) 
        state.rot += 2 * M_PI;

    state.rotQuat = Quaternion(state.rot, Vector3::UNIT_Z);
}

bool RigidBody::needsLinearIntegration() const
{
    return true;
}

bool RigidBody::needsRotationalIntegration() const
{
    return true;
}

void RigidBody::advanceCurrentState()
{
    getPreviousState() = getCurrentState();
}

RigidBody::State& RigidBody::getPreviousState()
{
    return _state[2];
}

const RigidBody::State& RigidBody::getPreviousState() const
{
    return _state[2];
}

RigidBody::State& RigidBody::getBlendedState()
{
    return _state[1];
}

const RigidBody::State& RigidBody::getBlendedState() const
{
    return _state[1];
}

RigidBody::State& RigidBody::getCurrentState()
{
    return _state[0];
}

const RigidBody::State& RigidBody::getCurrentState() const
{
    return _state[0];
}


////////// RigidBody::State //////////

RigidBody::State::State() :
    pos(Vector3::ZERO), vel(Vector3::ZERO), acc(Vector3::ZERO), 
    rotQuat(Quaternion::IDENTITY), rot(0.0f), spin(0.0f), 
    mass(1.0f), massInv(1.0f), radius(1.0f)
{

}


////////// Physics //////////

Physics::Physics(const vol::AABB& worldBounds, const char* collisionGeomFile) :
    _quadTree(worldBounds), _collisionGeom(collisionGeomFile), _accumulator(0.0f)
{

}

Physics::~Physics()
{
    foreach (RigidBody* body, _registered) 
        deregisterBody(*body);
}

struct CollisionVisitor {
    CollisionVisitor(RigidBody& body) : thisBody(body) {
        radiusSq = body.getRadius();
        radiusSq *= radiusSq;
    }
    void visit(RigidBody* otherBody) {
        float distSq = magnitudeSq(thisBody.getPosition() - otherBody->getPosition());

        if (distSq <= radiusSq) 
            thisBody.collisionWith(*otherBody);
    }
    RigidBody& thisBody;
    float radiusSq;
};

void Physics::accumulateAndIntegrate()
{
    _accumulator += _timer.elapsed();
    _timer.reset();

    while (_accumulator >= TIMESTEP_USEC) {
        integrateTimeDelta(TIMESTEP_SEC);
        _accumulator -= TIMESTEP_USEC;
    }

    float blend = float(_accumulator) / TIMESTEP_USEC;
    foreach (RigidBody* body, _registered)
        body->blendStates(blend);
}

void Physics::registerBody(RigidBody& body)
{
    if (body._system != 0)
        body._system->deregisterBody(body);

    _registered.insert(&body);
    body._system = this;
}

void Physics::deregisterBody(RigidBody& body)
{
    assert(body._system == this);

    _registered.erase(&body);
    body._system = 0;
}

const CollisionGeometry& Physics::getCollisionGeom() const
{
    return _collisionGeom;
}

void Physics::integrateTimeDelta(float dt)
{
    foreach (RigidBody* body, _registered) {
        body->integrate(dt);

        CollisionVisitor visitor(*body);
        _quadTree.process(visitor, vol::Circle(
            body->getPosition(), body->getRadius()));
    }
}

const float Physics::TIMESTEP_SEC = convUSecToSec(float(Physics::TIMESTEP_USEC));

