/// \file physunits.cpp
/// \brief Physical units and conversion.
/// \author Ben Radford 
/// \date 12th May 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#ifndef PHYSUNITS_HPP
#define PHYSUNITS_HPP


#include <math.h>


template<typename T>
inline T convUSecToSec(T value)
{
    return (value / T(1000000));
}

template<typename T>
inline T convSecToUSec(T value)
{
    return (value * T(1000000));
}

template<typename T>
inline T convRadToDeg(T value)
{
    return (value * (T(180) / M_PI));
}

template<typename T>
inline T convDegToRad(T value)
{
    return (value * (M_PI / T(180)));
}


#endif  // PHYSUNITS_HPP

