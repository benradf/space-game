/// \file drawbmp.hpp
/// \brief Utilities for drawing bitmaps.
/// \author Ben Radford 
/// \date 15th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef DRAWBMP_HPP
#define DRAWBMP_HPP


#include <assert.h>
#include <stdint.h>
#include <algorithm>
#include "colour.hpp"


namespace bmp {


class Bitmap {
    public:
        typedef clr::RGBA8888 Colour;

        enum BitDepth { BITS_32, BITS_24, BITS_16 };

        Bitmap(uint32_t width, uint32_t height);
        ~Bitmap();

        void loadFile(const char* filename);
        void saveFile(const char* filename, BitDepth depth);

        void resize(uint32_t width, uint32_t height);

        Colour getPixel(uint32_t x, uint32_t y) const;
        void setPixel(uint32_t x, uint32_t y, Colour c);

        void fill(Colour colour);
        void swap(Bitmap& other);

    private:
        Bitmap(const Bitmap&);
        Bitmap& operator=(const Bitmap&);

        Colour* _data;
        uint32_t _width;
        uint32_t _height;
};


void drawLine(Bitmap& bitmap, Bitmap::Colour c, int x0, int y0, int x1, int y1);


};  // namespace bmp


////////// bmp::Bitmap //////////

inline bmp::Bitmap::Colour bmp::Bitmap::getPixel(uint32_t x, uint32_t y) const
{
    assert((x < _width) && (y < _height));

    return _data[y*_width+x];
}

inline void bmp::Bitmap::setPixel(uint32_t x, uint32_t y, Colour c)
{
    assert((x < _width) && (y < _height));

    _data[y*_width+x] = c;
}

inline void bmp::Bitmap::swap(Bitmap& other)
{
    std::swap(_data, other._data);
    std::swap(_width, other._width);
    std::swap(_height, other._height);
}

#endif  // DRAWBMP_HPP

