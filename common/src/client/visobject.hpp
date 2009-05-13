/// \file visobject.hpp
/// \brief Unites object graphics and physics.
/// \author Ben Radford 
/// \date 4th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef VISOBJECT_HPP
#define VISOBJECT_HPP


#include <physics/object.hpp>
#include "graphics.hpp"


class VisibleObject {
    public:
        VisibleObject(std::auto_ptr<gfx::Entity> entity,
            std::auto_ptr<sim::MovableObject> object,
            gfx::MovableParticleSystem* exhaust);

        void update();

        const Vector3& getApparentPosition() const;
        const Quaternion& getApparentRotation() const;

        float getRotation() const;
        const Vector3& getPosition() const;
        const Vector3& getVelocity() const;
        sim::ControlState getControlState() const;

        void setRotation(float rotation);
        void setPosition(const Vector3& pos);
        void setVelocity(const Vector3& vel);
        void setControlState(sim::ControlState control);

    private:
        VisibleObject(const VisibleObject&);
        VisibleObject& operator=(const VisibleObject&);

        void beginInterpPosition();
        void beginInterpRotation();

        void updateApparentPosition();
        void updateApparentRotation();
        void updateBankedTurnRoll();

        static const int INTERP_TIME = 200000;

        std::auto_ptr<sim::MovableObject> _objectSim;
        std::auto_ptr<gfx::Entity> _objectGfx;
        gfx::MovableParticleSystem* _exhaust;

        Vector3 _predictedPos;
        Vector3 _apparentPos;
        bool _shouldInterpPos;
        int _timeToInterpPos;
        Timer _interpPosTimer;

        Quaternion _predictedRot;
        Quaternion _apparentRot;
        bool _shouldInterpRot;
        int _timeToInterpRot;
        Timer _interpRotTimer;

        Quaternion _roll;
        float _rollFrom;
        float _rollTo;
};


std::auto_ptr<VisibleObject> createVisibleObject(sim::ObjectID objectID);


#endif  // VISOBJECT_HPP

