/// \file drawbmp.hpp
/// \brief Utilities for drawing bitmaps.
/// \author Ben Radford 
/// \date 15th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef DRAWBMP_HPP
#define DRAWBMP_HPP


#include <stdint.h>


namespace bmp {


#pragma pack(push, 1)

struct FileHeader {
    uint16_t magic;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
};

struct InfoHeader {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t depth;
    uint32_t compression;
    uint32_t imageSize;
    uint32_t horizontalRes;
    uint32_t verticalRes;
    uint32_t paletteSize;
    uint32_t importantColours;
};

#pragma pack(pop)


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

        void swap(Bitmap& other);

    private:
        Bitmap(const Bitmap&);
        Bitmap& operator=(const Bitmap&);

        uint8* _data;
        uint32_t _width;
        uint32_t _height;
};


};  // namespace bmp


////////// bmp::Bitmap //////////

inline bmp::Colour bmp::Bitmap::getPixel(uint32_t x, uint32_t y) const
{
    assert((x < _width) && (y < _height));

    return *reinterpret_cast<Colour*>(_data[4*(y*_width+x)]);
}

inline void bmp::Bitmap::setPixel(uint32_t x, uint32_t y, Colour c)
{
    assert((x < _width) && (y < _height));

    *reinterpret_cast<Colour*>(_data[4*(y*_width+x)]) = c;
}

inline void bmp::Bitmap::swap(Bitmap& other)
{
    std::swap(_data, other._data);
    std::swap(_width, other._width);
    std::swap(_height, other._height);
}

#endif  // DRAWBMP_HPP

