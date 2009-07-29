/// \file object.hpp
/// \brief Implements the various game objects.
/// \author Ben Radford 
/// \date 3rd April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef OBJECT_HPP
#define OBJECT_HPP


#include "sim.hpp"
#include <stdint.h>
#include <core/timer.hpp>


namespace physics {


class Ship;
class Missile;


struct GameObjectBase : public RigidBody {
    virtual void updateState() = 0;
    virtual void collision(Ship& ship);
    virtual void collision(Missile& missile);
    virtual void collision(const Vector3& normal);
};


template<typename T>
struct GameObject : public GameObjectBase {
    virtual void collision(RigidBody& body);
};


////////// GameObject //////////

template<typename T>
inline void GameObject<T>::collision(RigidBody& body)
{
    static_cast<GameObjectBase&>(body).collision(static_cast<T&>(*this));
}


}  // namespace physics


#endif  // OBJECT_HPP

