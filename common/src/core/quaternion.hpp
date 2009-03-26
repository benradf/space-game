/// \file quaternion.hpp
/// \brief Provides an implementation of quaternions.
/// \author Ben Radford 
/// \date 26th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef QUATERNION_HPP
#define QUATERNION_HPP


#include <tvmet/Vector.h>
#include <math.h>


typedef tvmet::Vector<float, 3> Vector3;


class Quaternion {
    public:
        Quaternion(float w_, float x_, float y_, float z_);
        Quaternion(float a, const Vector3& v);
        Quaternion(const Vector3& v);

        Vector3 operator*(const Vector3& v) const;
        Quaternion operator*(const Quaternion& q) const;
        Quaternion& operator*=(const Quaternion& q);
        Quaternion conjugate() const;

        float w;
        float x;
        float y;
        float z;

    private:

};


////////// Quaternion //////////

inline Quaternion::Quaternion(float w_, float x_, float y_, float z_) :
    w(w_), x(x_), y(y_), z(z_)
{

}

inline Quaternion::Quaternion(float a, const Vector3& v)
{
    float tmp = 0.5f * a;
    w = std::cos(tmp);
    tmp = std::sin(tmp);
    x = v[0] * tmp;
    y = v[1] * tmp;
    z = v[2] * tmp;
}

inline Quaternion::Quaternion(const Vector3& v) :
    w(0.0f), x(v[0]), y(v[1]), z(v[2])
{

}

inline Vector3 Quaternion::operator*(const Vector3& v) const
{
    Quaternion q(*this);
    printf("q        = (%.2f, %.2f, %.2f, %.2f)\n", q.w, q.x, q.y, q.z);
    q *= Quaternion(v);
    printf("q*v      = (%.2f, %.2f, %.2f, %.2f)\n", q.w, q.x, q.y, q.z);
    q *= conjugate();
    printf("q*v*q^-1 = (%.2f, %.2f, %.2f, %.2f)\n", q.w, q.x, q.y, q.z);

    return Vector3(q.x, q.y, q.z);
}

inline Quaternion Quaternion::operator*(const Quaternion& q) const
{
    return Quaternion(*this) *= q;
}

inline Quaternion& Quaternion::operator*=(const Quaternion& q)
{
    float w2 = w * q.w - x * q.x - y * q.y - z * q.z;
    float x2 = w * q.x + x * q.w + y * q.z - z * q.y;
    float y2 = w * q.y - x * q.z + y * q.w + z * q.x;
    float z2 = w * q.z + x * q.y - y * q.x + z * q.w;

    w = w2, x = x2, y = y2, z = z2;

    return *this;
}

inline Quaternion Quaternion::conjugate() const
{
    return Quaternion(w, -x, -y, -z);
}


#endif  // QUATERNION_HPP

