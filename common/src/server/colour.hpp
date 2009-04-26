/// \file colour.hpp
/// \brief Various colour formats.
/// \author Ben Radford 
/// \date 15th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef COLOUR_HPP
#define COLOUR_HPP


#include <stdint.h>


namespace clr {


#define TAKE1(X1, X2, X3, X4) X1
#define TAKE2(X1, X2, X3, X4) X2
#define TAKE3(X1, X2, X3, X4) X3
#define TAKE4(X1, X2, X3, X4) X4

#define COL_ZERO(N) 0
#define COL_VAR(N) TAKE##N(r, g, b, a)
#define COL_ARG(N) TAKE##N(r_, g_, b_, a_)
#define COL_SIZE(N) TAKE##N(R, G, B, A)
#define COL_SIZE2(N) TAKE##N(_R, _G, _B, _A)
#define COL_CONV_D(X, Y, z) ConvertBitDepth<X, Y>::calc(c.z)
#define COL_CONV(N) COL_CONV_D(COL_SIZE2(N), COL_SIZE(N), COL_VAR(N))


#define COL3_NAME_CONCAT_D(X, Y, Z) X##Y##Z
#define COL3_NAME_CONCAT(X, Y, Z) COL3_NAME_CONCAT_D(X, Y, Z)
#define COL3_NAME(X1, X2, X3) COL3_NAME_CONCAT(COL_SIZE(X1), COL_SIZE(X2), COL_SIZE(X3))
#define COL3_INIT_D(X1, x1, X2, x2, X3, x3) COL_VAR(X3)(x3), COL_VAR(X2)(x2), COL_VAR(X1)(x1)
#define COL3_INIT(F, X1, X2, X3) COL3_INIT_D(X1, F(X1), X2, F(X2), X3, F(X3))
#define COL3_ARGLIST(X1, X2, X3) uint8_t COL_ARG(X1), uint8_t COL_ARG(X2), uint8_t COL_ARG(X3)

#define COLOUR3(X1, X2, X3)                                                             \
    template<int R, int G, int B>                                                       \
    struct COL3_NAME(X1, X2, X3) {                                                      \
        COL3_NAME(X1, X2, X3)() :                                                       \
            COL3_INIT(COL_ZERO, X1, X2, X3) {}                                          \
        COL3_NAME(X1, X2, X3)(COL3_ARGLIST(X1, X2, X3)) :                               \
            COL3_INIT(COL_ARG, X1, X2, X3) {}                                           \
        template<template<int, int, int> class _T, int _R, int _G, int _B>              \
        COL3_NAME(X1, X2, X3)(_T<_R, _G, _B> c) :                                       \
            COL3_INIT(COL_CONV, X1, X2, X3) {}                                          \
        template<template<int, int, int, int> class _T, int _R, int _G, int _B, int _A> \
        COL3_NAME(X1, X2, X3)(_T<_R, _G, _B, _A> c) :                                   \
            COL3_INIT(COL_CONV, X1, X2, X3) {}                                          \
        operator uint32_t() const {                                                     \
            return *reinterpret_cast<const uint32_t*>(this);                            \
        }                                                                               \
        unsigned COL_VAR(X3) : COL_SIZE(X3);                                            \
        unsigned COL_VAR(X2) : COL_SIZE(X2);                                            \
        unsigned COL_VAR(X1) : COL_SIZE(X1);                                            \
    };


#define COL4_NAME_CONCAT_D(W, X, Y, Z) W##X##Y##Z
#define COL4_NAME_CONCAT(W, X, Y, Z) COL4_NAME_CONCAT_D(W, X, Y, Z)
#define COL4_NAME(X1, X2, X3, X4) COL4_NAME_CONCAT(COL_SIZE(X1), COL_SIZE(X2), COL_SIZE(X3), COL_SIZE(X4))
#define COL4_INIT_D(X1, x1, X2, x2, X3, x3, X4, x4) COL_VAR(X4)(x4), COL_VAR(X3)(x3), COL_VAR(X2)(x2), COL_VAR(X1)(x1)
#define COL4_INIT(F, X1, X2, X3, X4) COL4_INIT_D(X1, F(X1), X2, F(X2), X3, F(X3), X4, F(X4))
#define COL4_ARGLIST(X1, X2, X3, X4) uint8_t COL_ARG(X1), uint8_t COL_ARG(X2), uint8_t COL_ARG(X3), uint8_t COL_ARG(X4)

#define COLOUR4(X1, X2, X3, X4)                                                         \
    template<int R, int G, int B, int A>                                                \
    struct COL4_NAME(X1, X2, X3, X4) {                                                  \
        COL4_NAME(X1, X2, X3, X4)() :                                                   \
            COL4_INIT(COL_ZERO, X1, X2, X3, X4) {}                                      \
        COL4_NAME(X1, X2, X3, X4)(COL4_ARGLIST(X1, X2, X3, X4)) :                       \
            COL4_INIT(COL_ARG, X1, X2, X3, X4) {}                                       \
        template<template<int, int, int> class _T, int _R, int _G, int _B>              \
        COL4_NAME(X1, X2, X3, X4)(_T<_R, _G, _B> c) :                                   \
            r(COL_CONV_D(_R, R, r)),                                                    \
            g(COL_CONV_D(_G, G, g)),                                                    \
            b(COL_CONV_D(_B, B, b)),                                                    \
            a(0) {}                                                                     \
        template<template<int, int, int, int> class _T, int _R, int _G, int _B, int _A> \
        COL4_NAME(X1, X2, X3, X4)(_T<_R, _G, _B, _A> c) :                               \
            COL4_INIT(COL_CONV, X1, X2, X3, X4) {}                                      \
        operator uint32_t() const {                                                     \
            return *reinterpret_cast<const uint32_t*>(this);                            \
        }                                                                               \
        unsigned COL_VAR(X4) : COL_SIZE(X4);                                            \
        unsigned COL_VAR(X3) : COL_SIZE(X3);                                            \
        unsigned COL_VAR(X2) : COL_SIZE(X2);                                            \
        unsigned COL_VAR(X1) : COL_SIZE(X1);                                            \
    };


template<int From, int To, bool b = From < To> struct ConvertBitDepth {};

template<int From, int To> struct ConvertBitDepth<From, To, true> {
    static uint8_t calc(uint8_t from) { return (from << (To - From)); } };

template<int From, int To> struct ConvertBitDepth<From, To, false> {
    static uint8_t calc(uint8_t from) { return (from >> (From - To)); } };


#pragma pack(push, 1) 

COLOUR3(1, 2, 3)  // RGB
COLOUR3(3, 2, 1)  // BGR

COLOUR4(1, 2, 3, 4)  // RGBA
COLOUR4(4, 1, 2, 3)  // ARGB
COLOUR4(3, 2, 1, 4)  // BGRA
COLOUR4(4, 3, 2, 1)  // ABGR

#pragma pack(pop)


typedef RGB<5, 6, 5> RGB565;
typedef BGR<5, 6, 5> BGR565;
typedef RGB<8, 8, 8> RGB888;
typedef BGR<8, 8, 8> BGR888;

typedef ARGB<8, 8, 8, 8> ARGB8888;
typedef RGBA<8, 8, 8, 8> RGBA8888;
typedef ABGR<8, 8, 8, 8> ABGR8888;
typedef BGRA<8, 8, 8, 8> BGRA8888;


};  // namespace clr


#endif  // COLOUR_HPP

