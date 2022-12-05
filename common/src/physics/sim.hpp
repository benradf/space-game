/// \file sim.hpp
/// \brief Physics simulation.
/// \author Ben Radford 
/// \date 25th March 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef SIM_HPP
#define SIM_HPP


#include <math.h>
#include <stdio.h>
#include <core/timer.hpp>
#include <math/vecmath.hpp>
#include <tr1/unordered_set>
#include "quadtree.hpp"
#include "collision.hpp"


class RigidBody {
    public:
        RigidBody();
        virtual ~RigidBody();

        void integrate(float dt);
        void blendStates(float t);

        virtual void collisionWith(const RigidBody& body) = 0;
        virtual void collisionDispatch(RigidBody& body) const = 0;

        void ApplyAbsoluteForce(const Vector3& force);
        void ApplyRelativeForce(const Vector3& force);
        void ApplySpin(float spin);

        const Vector3& getPosition() const;
        const Vector3& getVelocity() const;
        const Vector3& getAcceleration() const;
        const Quaternion& getRotationQuat() const;
        float getRotation() const;
        float getSpin() const;
        float getMass() const;
        float getRadius() const;

        void setPosition(const Vector3& pos);
        void setVelocity(const Vector3& vel);
        void setAcceleration(const Vector3& acc);
        void setRotation(float rot);
        void setSpin(float spin);
        void setMass(float mass);
        void setRadius(float radius);

        void ClearForce();
        void ClearSpin();

    private:
        void integrateLinearMotion(float dt);
        void integrateRotationalMotion(float dt);

        bool needsLinearIntegration() const;
        bool needsRotationalIntegration() const;

        friend class Physics;
        class Physics* _system;

        struct State {
            State();
            Vector3 pos;
            Vector3 vel;
            Vector3 acc;
            Quaternion rotQuat;
            float rot;
            float spin;
            float mass;
            float massInv;
            float radius;
        };

        void advanceCurrentState();

        State& getPreviousState();
        const State& getPreviousState() const;

        State& getBlendedState();
        const State& getBlendedState() const;

        State& getCurrentState();
        const State& getCurrentState() const;

        State _state[3];

};


class Physics {
    public:
        Physics(const vol::AABB& worldBounds, 
            const char* collisionGeomFile);
        ~Physics();

        void accumulateAndIntegrate();

        void registerBody(RigidBody& body);
        void deregisterBody(RigidBody& body);
        
        const CollisionGeometry& getCollisionGeom() const;

    private:
        static const int TIMESTEP_USEC = 10000;
        static const float TIMESTEP_SEC;

        Physics(const Physics&);
        Physics& operator=(const Physics&);

        void integrateTimeDelta(float dt);

        typedef std::tr1::unordered_set<RigidBody*> RigidBodySet;

        RigidBodySet _registered;
        QuadTree<RigidBody> _quadTree;
        CollisionGeometry _collisionGeom;

        uint64_t _accumulator;
        Timer _timer;
};


#endif  // SIM_HPP

