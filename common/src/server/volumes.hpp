/// \file volumes.hpp
/// \brief Bounding volumes and intersection tests.
/// \author Ben Radford 
/// \date 29th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef VOLUMES_HPP
#define VOLUMES_HPP


#include <vecmath.hpp>


namespace vol {


class Point {
    public:
        Point(const Vector3& pos);
        const Vector3& getPosition() const;

    private:
        Vector3 _position;
};


class Circle {
    public:
        Circle(const Vector3& centre, float radius);
        const Vector3& getCentre() const;
        float getCircumference() const;
        float getRadius() const;
        float getArea() const;

    private:
        Vector3 _centre;
        float _radius;
};


class AABB {
    public:
        AABB(const Vector3& min, const Vector3& max);
        const Vector3& getMin() const;
        const Vector3& getMax() const;
        Vector3 getMid() const;

    private:
        Vector3 _min;
        Vector3 _max;
};


bool intersects2d(const Point& a, const Circle& b);
bool intersects2d(const Circle& a, const Point& b);
bool intersects2d(const Point& a, const AABB& b);
bool intersects2d(const AABB& a, const Point& b);
bool intersects2d(const Circle& a, const Circle& b);
bool intersects2d(const Circle& a, const AABB& b);
bool intersects2d(const AABB& a, const Circle& b);
bool intersects2d(const AABB& a, const AABB& b);


////////// Point //////////

inline const Vector3& Point::getPosition() const
{
    return _position;
}


////////// Circle //////////

inline Circle::Circle(const Vector3& centre, float radius) :
    _centre(centre), _radius(radius)
{

}

inline const Vector3& Circle::getCentre() const
{
    return _centre;
}

inline float Circle::getCircumference() const
{
    return (2 * M_PI * _radius);
}

inline float Circle::getRadius() const
{
    return _radius;
}

inline float Circle::getArea() const
{
    return (M_PI * _radius * _radius);
}


////////// AABB //////////

inline AABB::AABB(const Vector3& min, const Vector3& max) :
    _min(min), _max(max)
{

}

inline const Vector3& AABB::getMin() const
{
    return _min;
}

inline const Vector3& AABB::getMax() const
{
    return _max;
}

inline Vector3 AABB::getMid() const
{
    return (0.5f * (_min + _max));
}


};


#endif  // VOLUMES_HPP
