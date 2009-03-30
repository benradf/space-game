/// \file volumes.cpp
/// \brief Bounding volumes and intersection tests.
/// \author Ben Radford 
/// \date 29th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "volumes.hpp"

bool vol::intersects2d(const Point& a, const Circle& b)
{
    float radius = b.getRadius();
    Vector3 distance(b.getCentre() - a.getPosition());

    return (magnitudeSq(distance) < radius * radius);
}

bool vol::intersects2d(const Circle& a, const Point& b)
{
    return intersects2d(b, a);
}

bool vol::intersects2d(const Point& a, const AABB& b)
{
    const Vector3& boxMin = b.getMin();
    const Vector3& boxMax = b.getMax();
    const Vector3& pos = a.getPosition();

    bool withinX = (pos.x >= boxMin.x) && (pos.x <= boxMax.x);
    bool withinY = (pos.y >= boxMin.y) && (pos.y <= boxMax.y);

    return (withinX && withinY);
}

bool vol::intersects2d(const AABB& a, const Point& b)
{
    return intersects2d(b, a);
}

bool vol::intersects2d(const Circle& a, const Circle& b)
{
    float radiusSum = a.getRadius() + b.getRadius();
    Vector3 distance(b.getCentre() - a.getCentre());
    distance.z = 0.0f;

    return (magnitudeSq(distance) <= radiusSum * radiusSum);
}

bool vol::intersects2d(const Circle& a, const AABB& b)
{
    const Vector3& boxMin = b.getMin();
    const Vector3& boxMax = b.getMax();
    const Vector3& centre = a.getCentre();
    float radius = a.getRadius();
    Vector3 point = centre;

    if (centre.x < boxMin.x) {
        point.x = boxMin.x;
    } else if (centre.x > boxMax.x) {
        point.x = boxMax.x;
    }

    if (centre.y < boxMin.y) {
        point.y = boxMin.y;
    } else if (centre.y > boxMax.y) {
        point.y = boxMax.y;
    }

    return (magnitudeSq(point - centre) <= radius * radius);
}

bool vol::intersects2d(const AABB& a, const Circle& b)
{
    return intersects2d(b, a);
}

bool vol::intersects2d(const AABB& a, const AABB& b)
{
    const Vector3& minA = a.getMin();
    const Vector3& maxA = a.getMax();
    const Vector3& minB = b.getMin();
    const Vector3& maxB = b.getMax();

    bool onXAxis = (maxA.x > minB.x) && (minA.x < maxB.x);
    bool onYAxis = (maxA.y > minB.y) && (minA.y < maxB.y);

    return (onXAxis && onYAxis);
}



