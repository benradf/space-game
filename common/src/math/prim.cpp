/// \file prim.cpp
/// \brief 3D Primitives.
/// \author Ben Radford 
/// \date 20th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "prim.hpp"
#include <algorithm>


////////// Triangle //////////

vol::AABB Triangle::determineBounds() const
{
    Vector3 min = _v0;
    Vector3 max = _v0;

    min.x = std::min(min.x, _v1.x);
    min.y = std::min(min.y, _v1.y);
    min.z = std::min(min.z, _v1.z);
    max.x = std::max(max.x, _v1.x);
    max.y = std::max(max.y, _v1.y);
    max.z = std::max(max.z, _v1.z);

    min.x = std::min(min.x, _v2.x);
    min.y = std::min(min.y, _v2.y);
    min.z = std::min(min.z, _v2.z);
    max.x = std::max(max.x, _v2.x);
    max.y = std::max(max.y, _v2.y);
    max.z = std::max(max.z, _v2.z);

    return vol::AABB(min, max);
}

