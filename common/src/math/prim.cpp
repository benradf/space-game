/// \file prim.cpp
/// \brief 3D Primitives.
/// \author Ben Radford 
/// \date 20th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "prim.hpp"
#include <algorithm>


using namespace vol;


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


////////// Intersection Functions //////////

bool intersects3d(const Triangle& a, const Sphere& b)
{
    // Get triangle vertices.
    const Vector3& v0 = a.getV0();
    const Vector3& v1 = a.getV1();
    const Vector3& v2 = a.getV2();

    // Distance from centre of sphere to triangle plane.
    float d = dotProduct(a.getNormal(), v0 - b.getCentre());

    // Check sphere intersects plane.
    if (fabs(d) > b.getRadius())
        return false;

    // Slice sphere to get a circle in the plane.
    Vector3 c = b.getCentre() + d * a.getNormal();
    float r_sq = b.getRadius() * b.getRadius() - d * d;

    // Is v0 inside circle?
    Vector3 c_v0 = c - v0;
    if (magnitudeSq(c_v0) < r_sq)
        return true;

    // Is v1 inside circle?
    Vector3 c_v1 = c - v1;
    if (magnitudeSq(c_v1) < r_sq)
        return true;

    // Is v2 inside circle?
    Vector3 c_v2 = c - v2;
    if (magnitudeSq(c_v2) < r_sq)
        return true;

    // Get edge vectors.
    Vector3 v0_v1 = v0 - v1;
    Vector3 v1_v2 = v1 - v2;
    Vector3 v2_v0 = v2 - v0;

    // Calculate edge normal vectors.
    Vector3 v0_v1_norm = crossProduct(v0_v1, a.getNormal());
    Vector3 v1_v2_norm = crossProduct(v1_v2, a.getNormal());
    Vector3 v2_v0_norm = crossProduct(v2_v0, a.getNormal());

    // Check which edges the sphere centre is inside.
    bool inside0 = (dotProduct(c_v1, v0_v1_norm) >= 0.0f);
    bool inside1 = (dotProduct(c_v2, v1_v2_norm) >= 0.0f);
    bool inside2 = (dotProduct(c_v0, v2_v0_norm) >= 0.0f);

    // Is it inside all three edges?
    if (inside0 && inside1 && inside2)
        return true;

    // Calculate vars for edge v1->v0.
    float magSq_v0_v1 = magnitudeSq(v0_v1);
    float dp0 = dotProduct(c_v1, v0_v1);
    float dp0_sq = dp0 * dp0;

    // Does circle intersect edge v1->v0?
    if ((dp0 >= 0.0f) && (dp0_sq <= magSq_v0_v1 * magSq_v0_v1)) {
        if (magSq_v0_v1 * magnitudeSq(c_v1) - dp0_sq <= magSq_v0_v1 * r_sq)
            return true;
    }

    // Calculate vars for edge v2->v1.
    float magSq_v1_v2 = magnitudeSq(v1_v2);
    float dp1 = dotProduct(c_v2, v1_v2);
    float dp1_sq = dp1 * dp1;

    // Does circle intersect edge v2->v1?
    if ((dp1 >= 0.0f) && (dp1_sq <= magSq_v1_v2 * magSq_v1_v2)) {
        if (magSq_v1_v2 * magnitudeSq(c_v2) - dp1_sq <= magSq_v1_v2 * r_sq)
            return true;
    }

    // Calculate vars for edge v0->v2.
    float magSq_v2_v0 = magnitudeSq(v2_v0);
    float dp2 = dotProduct(c_v0, v2_v0);
    float dp2_sq = dp2 * dp2;

    // Does circle intersect edge v0->v2?
    if ((dp2 >= 0.0f) && (dp2_sq <= magSq_v2_v0 * magSq_v2_v0)) {
        if (magSq_v2_v0 * magnitudeSq(c_v0) - dp2_sq <= magSq_v2_v0 * r_sq)
            return true;
    }

    return false;
}

bool intersects3d(const Sphere& a, const Triangle& b)
{
    return intersects3d(b, a);
}

bool intersects2d(const Triangle& a, const vol::Circle& b)
{
    // TODO: Write a 2D optimised version of above.

    assert(false);

    return false;
}

bool intersects2d(const vol::Circle& a, const Triangle& b)
{
    return intersects2d(b, a);
}

