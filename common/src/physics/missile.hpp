/// \file missile.hpp
/// \brief Implementation of missile weapons.
/// \author Ben Radford 
/// \date 25th July 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef MISSILE_HPP
#define MISSILE_HPP


#include "object.hpp"


namespace physics {


typedef uint32_t MissileID;


class Missile : public GameObject<Missile> {
    public:
        Missile(MissileID id);
        virtual ~Missile();

        virtual void updateState();
        virtual MissileID getID() const;

        virtual void collision(Ship& ship);
        virtual void collision(Missile& missile);
        virtual void collision(const Vector3& normal);

    private:
};


}  // namespace physics


#endif  // MISSILE_HPP

