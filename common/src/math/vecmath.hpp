/// \file vecmath.hpp
/// \brief Provides vectors, matrices and quaternions.
/// \author Ben Radford 
/// \date 27th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef VECMATH_HPP
#define VECMATH_HPP


#include <math.h>


namespace math {


template<typename T> struct Quaternion;
template<typename T> struct Vector3;


template<typename T>
struct Vector3 {
    Vector3(T x_, T y_, T z_);
    Vector3(const Quaternion<T>& q);

    Vector3& operator+=(const Vector3& v);
    Vector3& operator-=(const Vector3& v);
    Vector3& operator*=(T s);
    Vector3& operator/=(T s);

    Vector3& normalise();

    static const Vector3 ZERO;
    static const Vector3 UNIT_X;
    static const Vector3 UNIT_Y;
    static const Vector3 UNIT_Z;

    T x;
    T y;
    T z;
};


template<typename T>
struct Quaternion {
    Quaternion(T w_, T x_, T y_, T z_);
    Quaternion(T a, const Vector3<T>& v);
    Quaternion(const Vector3<T>& v);

    Quaternion& operator+=(const Quaternion& q);
    Quaternion& operator-=(const Quaternion& q);
    Quaternion& operator*=(const Quaternion& q);
    Quaternion& operator*=(T s);
    Quaternion& operator/=(T s);

    Quaternion& normalise();
    Quaternion& conjugate();
    Quaternion& inverse();

    static const Quaternion ZERO;
    static const Quaternion IDENTITY;

    T w;
    T x;
    T y;
    T z;
};


////////// Vector3 //////////

template<typename T>
inline Vector3<T>::Vector3(T x_, T y_, T z_) :
    x(x_), y(y_), z(z_)
{

}

template<typename T>
inline Vector3<T>::Vector3(const Quaternion<T>& q) :
    x(q.x), y(q.y), z(q.z)
{

}

template<typename T>
inline Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& v)
{
    x += v.x, y += v.y, z += v.z;

    return *this;
}

template<typename T>
inline Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& v)
{
    x -= v.x, y -= v.y, z -= v.z;

    return *this;
}

template<typename T>
inline Vector3<T>& Vector3<T>::operator*=(T s)
{
    x *= s, y *= s, z *= s;

    return *this;
}

template<typename T>
inline Vector3<T>& Vector3<T>::operator/=(T s)
{
    return operator*=(T(1) / s);
}

template<typename T>
inline Vector3<T>& Vector3<T>::normalise()
{
    return operator/=(magnitude(*this));
}

template<typename T>
inline Vector3<T> operator+(const Vector3<T>& u, const Vector3<T>& v)
{
    return (Vector3<T>(u) += v);
}

template<typename T>
inline Vector3<T> operator-(const Vector3<T>& u, const Vector3<T>& v)
{
    return (Vector3<T>(u) -= v);
}

template<typename T>
inline Vector3<T> operator*(const Vector3<T>&v, T s)
{
    return (Vector3<T>(v) *= s);
}

template<typename T>
inline Vector3<T> operator*(T s, const Vector3<T>& v)
{
    return operator*(v, s);
}

template<typename T>
inline Vector3<T> operator/(const Vector3<T>& v, T s)
{
    return (Vector3<T>(v) /= s);
}

template<typename T>
inline Vector3<T> operator-(const Vector3<T>& v)
{
    return Vector3<T>(-v.x, -v.y, -v.z);
}

template<typename T>
inline T dotProduct(const Vector3<T>& u, const Vector3<T>& v)
{
    return (u.x * v.x + u.y * v.y + u.z * v.z);
}

template<typename T>
inline Vector3<T> crossProduct(const Vector3<T>& u, const Vector3<T>& v)
{
    return Vector3<T>(
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    );
}

template<typename T>
inline T magnitudeSq(const Vector3<T>& v)
{
    return dotProduct(v, v);
}

template<typename T>
inline T magnitude(const Vector3<T>& v)
{
    return sqrt(magnitudeSq(v));
}

template<typename T>
inline Vector3<T> normalise(const Vector3<T>& v)
{
    return Vector3<T>(v).normalise();
}

template<typename T>
inline Vector3<T> reflect(const Vector3<T>& v, const Vector3<T>& n)
{
    return (v - 2 * dotProduct(v, n) * n);
}

template<typename T>
inline Vector3<T> lerp(const Vector3<T>& u, const Vector3<T>& v, T t)
{
    return ((T(1) - t) * u + t * v);
}

template<typename T>
const Vector3<T> Vector3<T>::ZERO(T(0), T(0), T(0));

template<typename T>
const Vector3<T> Vector3<T>::UNIT_X(T(1), T(0), T(0));

template<typename T>
const Vector3<T> Vector3<T>::UNIT_Y(T(0), T(1), T(0));

template<typename T>
const Vector3<T> Vector3<T>::UNIT_Z(T(0), T(0), T(1));


////////// Quaternion //////////

template<typename T>
inline Quaternion<T>::Quaternion(T w_, T x_, T y_, T z_) :
    w(w_), x(x_), y(y_), z(z_)
{

}

template<typename T>
inline Quaternion<T>::Quaternion(T a, const Vector3<T>& v)
{
    T tmp = a / T(2);
    w = cos(tmp);
    tmp = sin(tmp);
    x = v.x * tmp;
    y = v.y * tmp;
    z = v.z * tmp;
}

template<typename T>
inline Quaternion<T>::Quaternion(const Vector3<T>& v) :
    w(T(0)), x(v.x), y(v.y), z(v.z)
{

}

template<typename T>
Quaternion<T>& Quaternion<T>::operator+=(const Quaternion& q)
{
    w += q.w, x += q.x, y += q.y, z += q.z;

    return *this;
}

template<typename T>
Quaternion<T>& Quaternion<T>::operator-=(const Quaternion& q)
{
    w -= q.w, x -= q.x, y -= q.y, z -= q.z;

    return *this;
}

template<typename T>
inline Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& q)
{
    float w_ = w * q.w - x * q.x - y * q.y - z * q.z;
    float x_ = w * q.x + x * q.w + y * q.z - z * q.y;
    float y_ = w * q.y - x * q.z + y * q.w + z * q.x;
    float z_ = w * q.z + x * q.y - y * q.x + z * q.w;

    w = w_, x = x_, y = y_, z = z_;

    return *this;
}

template<typename T>
inline Quaternion<T>& Quaternion<T>::operator*=(T s)
{
    w *= s, x *= s, y *= s, z *= s;

    return *this;
}

template<typename T>
inline Quaternion<T>& Quaternion<T>::operator/=(T s)
{
    return operator*=(T(1) / s);
}

template<typename T>
inline Quaternion<T>& Quaternion<T>::normalise()
{
    return operator/=(magnitude(*this));
}

template<typename T>
inline Quaternion<T>& Quaternion<T>::conjugate()
{
    x = -x, y = -y, z = -z;

    return *this;
}

template<typename T>
inline Quaternion<T>& Quaternion<T>::inverse()
{
    return conjugate().normalise();
}

template<typename T>
inline Quaternion<T> operator+(const Quaternion<T>& p, const Quaternion<T>& q)
{
    return (Quaternion<T>(p) += q);
}

template<typename T>
inline Quaternion<T> operator-(const Quaternion<T>& p, const Quaternion<T>& q)
{
    return (Quaternion<T>(p) -= q);
}

template<typename T>
inline Quaternion<T> operator*(const Quaternion<T>& p, const Quaternion<T>& q)
{
    return (Quaternion<T>(p) *= q);
}

template<typename T>
inline Quaternion<T> operator*(const Quaternion<T>& q, T s)
{
    return (Quaternion<T>(q) *= s);
}

template<typename T>
inline Quaternion<T> operator*(T s, const Quaternion<T>& q)
{
    return operator*(q, s);
}

template<typename T>
inline Quaternion<T> operator/(const Quaternion<T>& q, T s)
{
    return (Quaternion<T>(q) /= s);
}

template<typename T>
inline Quaternion<T> operator-(const Quaternion<T>& q)
{
    return Quaternion<T>(-q.w, -q.x, -q.y, -q.z);
}

template<typename T>
inline T dotProduct(const Quaternion<T>& p, const Quaternion<T>& q)
{
    return (p.w * q.w + p.x * q.x + p.y * q.y + p.z * q.z);
}

template<typename T>
inline T magnitudeSq(const Quaternion<T>& q)
{
    return dotProduct(q, q);
}

template<typename T>
inline T magnitude(const Quaternion<T>& q)
{
    return sqrt(magnitudeSq(q));
}

template<typename T>
inline Quaternion<T> normalise(const Quaternion<T>& q)
{
    return Quaternion<T>(q).normalise();
}

template<typename T>
inline Quaternion<T> conjugate(const Quaternion<T>& q)
{
    return Quaternion<T>(q).conjugate();
}

template<typename T>
inline Quaternion<T> inverse(const Quaternion<T>& q)
{
    return Quaternion<T>(q).inverse();
}

template<typename T>
inline Vector3<T> operator*(const Quaternion<T>& q, const Vector3<T> v)
{
    Vector3<T> u(q.x, q.y, q.z);
    Vector3<T> b = crossProduct(v, u);
    Vector3<T> a = crossProduct(b, u);

    return (v + T(2) * (a - q.w * b));
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

template<typename T>
const Quaternion<T> Quaternion<T>::ZERO(T(0), T(0), T(0), T(0));

template<typename T>
const Quaternion<T> Quaternion<T>::IDENTITY(T(1), T(0), T(0), T(0));


}  // namespace math


typedef math::Vector3<float> Vector3;
typedef math::Quaternion<float> Quaternion;


#endif  // VECMATH_HPP

