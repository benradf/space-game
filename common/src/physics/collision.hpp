/// \file collision.hpp
/// \brief Provides collision detection routines.
/// \author Ben Radford 
/// \date 23rd April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef COLLISION_HPP
#define COLLISION_HPP


#include "kdtree.hpp"
#include <math/volumes.hpp>


class CollisionGeometry {
    public:
        CollisionGeometry(const char* filename);

        bool checkCollision(const vol::Sphere& sphere, Vector3& normal) const;

    private:
        KDTree::Ptr _kdtree;
};


#endif  // COLLISION_HPP

