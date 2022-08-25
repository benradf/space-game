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

using namespace physics;


////////// RigidBody //////////

physics::RigidBody::RigidBody() :
    _system(0)
{

}

physics::RigidBody::~RigidBody()
{
    if (_system != 0) 
        _system->deregisterBody(*this);
}

void physics::RigidBody::integrate(float dt)
{
    advanceCurrentState();
    integrateLinearMotion(dt);
    integrateRotationalMotion(dt);
}

void physics::RigidBody::blendStates(float t)
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

void physics::RigidBody::applyAbsoluteForce(const Vector3& force)
{
    State& state = getCurrentState();

    state.acc += state.massInv * force;
}

void physics::RigidBody::applyRelativeForce(const Vector3& force)
{
    applyAbsoluteForce(getCurrentState().rotQuat * force);
}

void physics::RigidBody::applySpin(float spin)
{
    getCurrentState().spin += spin;
}

const Vector3& physics::RigidBody::getPosition() const
{
    return getBlendedState().pos;
}

const Vector3& physics::RigidBody::getVelocity() const
{
    return getBlendedState().vel;
}

const Vector3& physics::RigidBody::getAcceleration() const
{
    return getBlendedState().acc;
}

const Quaternion& physics::RigidBody::getRotationQuat() const
{
    return getBlendedState().rotQuat;
}

float physics::RigidBody::getRotation() const
{
    return getBlendedState().rot;
}

float physics::RigidBody::getSpin() const
{
    return getBlendedState().spin;
}

float physics::RigidBody::getMass() const
{
    return getBlendedState().mass;
}

float physics::RigidBody::getRadius() const
{
    return getBlendedState().radius;
}

void physics::RigidBody::setPosition(const Vector3& pos)
{
    getCurrentState().pos = pos;
    getPreviousState().pos = pos;
}

void physics::RigidBody::setVelocity(const Vector3& vel)
{
    getCurrentState().vel = vel;
    getPreviousState().vel = vel;
}

void physics::RigidBody::setAcceleration(const Vector3& acc)
{
    getCurrentState().acc = acc;
    getPreviousState().acc = acc;
}

void physics::RigidBody::setRotation(float rot)
{
    getCurrentState().rot = rot;
    getPreviousState().rot = rot;
}

void physics::RigidBody::setSpin(float spin)
{
    getCurrentState().spin = spin;
    getPreviousState().spin = spin;
}

void physics::RigidBody::setMass(float mass)
{
    getCurrentState().mass = mass;
    getPreviousState().mass = mass;
    getCurrentState().massInv = 1.0f / mass;
    getPreviousState().massInv = 1.0f / mass;
}

void physics::RigidBody::setRadius(float radius)
{
    getCurrentState().radius = radius;
    getPreviousState().radius = radius;
}

void physics::RigidBody::clearForce()
{
    getCurrentState().acc = Vector3::ZERO;
}

void physics::RigidBody::clearSpin()
{
    getCurrentState().spin = 0.0f;
}

void physics::RigidBody::integrateLinearMotion(float dt)
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

void physics::RigidBody::integrateRotationalMotion(float dt)
{
    if (!needsRotationalIntegration())
        return;

    State& state = getCurrentState();

    state.rot = fmod(state.rot + state.spin * dt, 2 * M_PI);

    if (state.rot < 0.0f) 
        state.rot += 2 * M_PI;

    state.rotQuat = Quaternion(state.rot, Vector3::UNIT_Z);
}

bool physics::RigidBody::needsLinearIntegration() const
{
    return true;
}

bool physics::RigidBody::needsRotationalIntegration() const
{
    return true;
}

void physics::RigidBody::advanceCurrentState()
{
    getPreviousState() = getCurrentState();
}

physics::RigidBody::State& physics::RigidBody::getPreviousState()
{
    return _state[2];
}

const physics::RigidBody::State& physics::RigidBody::getPreviousState() const
{
    return _state[2];
}

physics::RigidBody::State& physics::RigidBody::getBlendedState()
{
    return _state[1];
}

const physics::RigidBody::State& physics::RigidBody::getBlendedState() const
{
    return _state[1];
}

physics::RigidBody::State& physics::RigidBody::getCurrentState()
{
    return _state[0];
}

const physics::RigidBody::State& physics::RigidBody::getCurrentState() const
{
    return _state[0];
}


////////// RigidBody::State //////////

physics::RigidBody::State::State() :
    pos(Vector3::ZERO), vel(Vector3::ZERO), acc(Vector3::ZERO), 
    rotQuat(Quaternion::IDENTITY), rot(0.0f), spin(0.0f), 
    mass(1.0f), massInv(1.0f), radius(1.0f)
{

}


////////// Physics //////////

physics::Physics::Physics(const vol::AABB& worldBounds, const char* collisionGeomFile) :
    _quadTree(worldBounds), _collisionGeom(collisionGeomFile), _accumulator(0.0f)
{

}

physics::Physics::~Physics()
{
    for (auto body : _registered) 
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
            otherBody->collision(thisBody);
    }
    RigidBody& thisBody;
    float radiusSq;
};

void physics::Physics::accumulateAndIntegrate()
{
    _accumulator += _timer.elapsed();
    _timer.reset();

    while (_accumulator >= TIMESTEP_USEC) {
        integrateTimeDelta(TIMESTEP_SEC);
        _accumulator -= TIMESTEP_USEC;
    }

    float blend = float(_accumulator) / TIMESTEP_USEC;
    for (auto body : _registered)
        body->blendStates(blend);
}

void physics::Physics::registerBody(RigidBody& body)
{
    if (body._system != 0)
        body._system->deregisterBody(body);

    _registered.insert(&body);
    body._system = this;
}

void physics::Physics::deregisterBody(RigidBody& body)
{
    assert(body._system == this);

    _registered.erase(&body);
    body._system = 0;
}

const CollisionGeometry& physics::Physics::getCollisionGeom() const
{
    return _collisionGeom;
}

void physics::Physics::integrateTimeDelta(float dt)
{
    for (auto body : _registered) {
        body->integrate(dt);

        CollisionVisitor visitor(*body);
        _quadTree.process(visitor, vol::Circle(
            body->getPosition(), body->getRadius()));
    }
}

const float physics::Physics::TIMESTEP_SEC = convUSecToSec(float(physics::Physics::TIMESTEP_USEC));

