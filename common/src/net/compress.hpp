/// \file compress.hpp
/// \brief Compression for network traffic.
/// \author Ben Radford 
/// \date 26th April 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef COMPRESS_HPP
#define COMPRESS_HPP


namespace net {


template<typename T>
struct Vec2 {
    Vec2(T x_, T y_);
    T x, y;
};

template<typename T>
Vec2<T>::Vec2(T x_, T y_) :
    x(x_), y(y_)
{

}

template<typename T>
inline Vec2<T> makeVec2(T x, T y)
{
    return Vec2<T>(x, y);
}


inline Vec2<int16_t> packPos(const Vector3& pos)
{
    return Vec2<int16_t>(int16_t(pos.x * 10.0f), int16_t(pos.y * 10.0f));
}

inline Vector3 unpackPos(const Vec2<int16_t>& pos)
{
    return Vector3(float(pos.x) / 10.0f, float(pos.y) / 10.0f, 0.0f);
}

inline Vector3 unpackPos(int16_t x, int16_t y)
{
    return unpackPos(Vec2<int16_t>(x, y));
}

inline Vec2<int16_t> packVel(const Vector3& vel)
{
    return Vec2<int16_t>(int16_t(vel.x * 10.0f), int16_t(vel.y * 10.0f));
}

inline Vector3 unpackVel(const Vec2<int16_t>& vel)
{
    return Vector3(float(vel.x) / 10.0f, float(vel.y) / 10.0f, 0.0f);
}

inline Vector3 unpackVel(int16_t x, int16_t y)
{
    return unpackVel(Vec2<int16_t>(x, y));
}

inline uint8_t packRot(float rot)
{
    return uint8_t(rot * 40.0f);
}

inline float unpackRot(uint8_t rot)
{
    return (float(rot) / 40.0f);
}



}  // namespace net


#endif  // COMPRESS_HPP

