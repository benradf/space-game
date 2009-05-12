/// \file interp.hpp
/// \brief Interpolation functions.
/// \author Ben Radford 
/// \date 27th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef VECMATH_HPP
#define VECMATH_HPP


#include "vecmath.hpp"


template<typename T>
inline T lerp(T a, T b, T t)
{
    return ((T(1) - t) * a + t * b);
}

template<typename T>
inline Vector3<T> lerp(const Vector3<T>& u, const Vector3<T>& v, T t)
{
    return ((T(1) - t) * u + t * v);
}

template<typename T>
inline Quaternion<T> lerp(const Quaternion<T>& p, const Quaternion<T>& q, T t)
{
    printf("lerp[(%.2f, %.2f, %.2f, %.2f), (%.2f, %.2f, %.2f, %.2f), %.3f]\n", p.w, p.x, p.y, p.z, q.w, q.x, q.y, q.z, t);
    return normalise((T(1) - t) * p + t * q);
}

template<typename T>
inline Quaternion<T> slerp(const Quaternion<T>& p, const Quaternion<T>& q, T t, T cosA)
{
    if (cosA >= T(999) / T(1000))
        return lerp(p, q, t);

    printf("slerp[(%.2f, %.2f, %.2f, %.2f), (%.2f, %.2f, %.2f, %.2f), %.3f] = ", p.w, p.x, p.y, p.z, q.w, q.x, q.y, q.z, t);

    T sinA = sqrt(T(1) - cosA * cosA);
    T a = acos(cosA);

    T coeff0 = sin((T(1) - t) * a);
    T coeff1 = sin(t * a);

    Quaternion<T> result = ((coeff0 * p + coeff1 * q) / sinA);

    printf("(%.2f, %.2f, %.2f, %.2f)\n", result.w, result.x, result.y, result.z);

    return result;
}

template<typename T>
inline Quaternion<T> slerp(const Quaternion<T>& p, const Quaternion<T>& q, T t)
{
    return slerp(p, q, t, dotProduct(p, q));
}

template<typename T>
inline Quaternion<T> slerpMin(const Quaternion<T>& p, const Quaternion<T>& q, T t)
{
    T cosA = dotProduct(p, q);

    return slerp(p, (cosA < T(0) ? -q : q), t, T(fabs(cosA)));
}


#endif  // VECMATH_HPP

