/// \file sim.cpp
/// \brief Physics simulation.
/// \author Ben Radford 
/// \date 25th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef SIM_HPP
#define SIM_HPP


#include <tvmet/Vector.h>
#include <tvmet/Matrix.h>
#include <math.h>
#include <stdio.h>
#include <quaternion.hpp>


namespace sim {

void testSimulation();

typedef tvmet::Vector<float, 3> Vector3;



class Object {
    public:
        Object(float mass);
        ~Object();

        void integrate(float dt);

        void ApplyAbsoluteForce(const Vector3& force);
        void ApplyRelativeForce(const Vector3& force);
        void ApplySpin(const Quaternion& spin);

        const Vector3& getPosition() const;
        const Vector3& getVelocity() const;
        const Vector3& getAcceleration() const;

        const Quaternion& getRotation() const;
        const Quaternion& getSpin() const;

        void setPosition(const Vector3& pos);
        void setVelocity(const Vector3& vel);
        void setAcceleration(const Vector3& acc);

        void setRotation(const Quaternion& rot);
        void setSpin(const Quaternion& spin);

        void ClearForce();
        void ClearSpin();

    private:
        void integrateLinearMotion(float dt);
        void integrateRotationalMotion(float dt);

        Vector3 _pos;
        Vector3 _vel;
        Vector3 _acc;

        Quaternion _rot;
        Quaternion _spin;

        float _mass;
        float _massInverse;
};

class Ray {
    public:

    private:
        
};


class Simulation {
    public:

    private:

};

}  // namespace sim

#endif  // SIM_HPP

