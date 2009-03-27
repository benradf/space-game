#ifndef CORE_HPP
#define CORE_HPP


#include "log.hpp"
#include "exception.hpp"

#include <foreach.hpp>
#define foreach BOOST_FOREACH


static const int GAMEPORT = 12345;

static const float degPerRad = 180.0f / 3.141592654f;
static const float radPerDeg = 3.141592654f / 180.0f;

#define DEG2RAD(x) (x * radPerDeg)
#define RAD2DEG(x) (x * degPerRad)

#endif  // CORE_HPP
