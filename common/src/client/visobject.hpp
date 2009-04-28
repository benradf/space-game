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
            std::auto_ptr<sim::MovableObject> object);

        void update();

        const Vector3& getApparentPosition() const;
        const Quaternion& getApparentRotation() const;

        const Vector3& getPosition() const;
        const Vector3& getVelocity() const;
        const Quaternion& getRotation() const;
        sim::ControlState getControlState() const;

        void setPosition(const Vector3& pos);
        void setVelocity(const Vector3& vel);
        void setRotation(const Quaternion& rot);
        void setControlState(sim::ControlState control);

    private:
        VisibleObject(const VisibleObject&);
        VisibleObject& operator=(const VisibleObject&);

        void beginInterpolation();

        static const int INTERP_TIME = 500000;

        std::auto_ptr<sim::MovableObject> _objectSim;
        std::auto_ptr<gfx::Entity> _objectGfx;
        
        bool _shouldInterpolate;
        int _timeToCorrect;
        Timer _timer;

        Vector3 _position;
        Quaternion _rotation;
};


std::auto_ptr<VisibleObject> createVisibleObject(sim::ObjectID objectID);


#endif  // VISOBJECT_HPP

