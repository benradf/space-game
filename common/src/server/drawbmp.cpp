/// \file drawbmp.cpp
/// \brief Utilities for drawing bitmaps.
/// \author Ben Radford 
/// \date 15th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "drawbmp.hpp"


using namespace clr;


////////// bmp::Bitmap //////////

Bitmap::Bitmap(uint32_t width, uint32_t height) :
    _data(new uint8_t[4*width*height]),
    _width(width), _height(height)
{

}

Bitmap::~Bitmap()
{
    delete[] _data;
}

void Bitmap::loadFile(const char* filename)
{

}

void Bitmap::saveFile(const char* filename, BitDepth depth)
{
    FileHeader fileHeader;
    fileHeader.magic = 0x4D42;
    fileHeader.size = ;
    fileHeader.reserved1 = 0;
    fileHeader.reserved2 = 0;
    fileHeader.offset = ;

    InfoHeader infoHeader;
    infoHeader.size = sizeof(infoHeader);
    infoHeader.width = _width;
    infoHeader.height = _height;
    infoHeader.planes = 1;
    infoHeader.depth = ;
    infoHeader.compression = 0;
    infoHeader.imageSize = ;
    infoHeader.horizontalRes = 1;
    infoHeader.verticalRes = 1;
    infoHeader.paletteSize = 0;
    infoHeader.importantColours = 0;

    for (int32_t y = _height - 1; y >= 0; y--) {
        for (uint32_t x = 0; x < _width; x++) {
            switch (depth) {
                case BITS_16: {
                    RGB565 colour(getPixel(x, y));
                    uint16_t value = colour;
                    file.write(&value, sizeof(value));
                    } break;
                case BITS_24: {
                    RGB888 colour(getPixel(x, y));
                    uint8_t r = colour.r;
                    uint8_t g = colour.g;
                    uint8_t b = colour.b;
                    file.write(&r, sizeof(r));
                    file.write(&g, sizeof(g));
                    file.write(&b, sizeof(b));
                    } break;
                case BITS_32: {
                    ARGB8888 colour(getPixel(x, y));
                    uint32_t value = colour;
                    file.write(&value, sizeof(value));
                    } break;
            }
        }
    }
}

void Bitmap::resize(uint32_t width, uint32_t height)
{
    Bitmap newBitmap(width, height);

    for (uint32_t y = 0; y < std::min(height, _height); y++) {
        for (uint32_t x = 0; x < std::min(width, _width); x++)
            newBitmap.setPixel(x, y, getPixel(x, y));
    }

    swap(newBitmap);
}

