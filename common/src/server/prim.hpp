/// \file prim.hpp
/// \brief 3D Primitives.
/// \author Ben Radford 
/// \date 20th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef PRIM_HPP
#define PRIM_HPP


#include "volumes.hpp"
#include <vecmath.hpp>
#include <assert.h>


class Triangle {
    public:
        Triangle();
        Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2);

        vol::AABB determineBounds() const;

        const Vector3& getVertex(int index) const;

        const Vector3& getV0() const;
        const Vector3& getV1() const;
        const Vector3& getV2() const;

    private:
        Vector3 _v0;
        Vector3 _v1;
        Vector3 _v2;
};


////////// Triangle //////////

inline Triangle::Triangle() :
    _v0(Vector3::ZERO), _v1(Vector3::ZERO), _v2(Vector3::ZERO)
{

}

inline Triangle::Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2) :
    _v0(v0), _v1(v1), _v2(v2)
{

}

inline const Vector3& Triangle::getVertex(int index) const
{
    switch (index) {
        case 0: return _v0;
        case 1: return _v1;
        case 2: return _v2;
        default: assert(false);
    }
}

inline const Vector3& Triangle::getV0() const
{
    return _v0;
}

inline const Vector3& Triangle::getV1() const
{
    return _v1;
}

inline const Vector3& Triangle::getV2() const
{
    return _v2;
}


#endif  // PRIM_HPP

