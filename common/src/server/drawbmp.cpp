/// \file drawbmp.cpp
/// \brief Utilities for drawing bitmaps.
/// \author Ben Radford 
/// \date 15th April 2009
///
/// Copyright (c) 2009 Ben Radford.
///


#include "drawbmp.hpp"
#include <fstream>


using namespace clr;


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


////////// bmp::Bitmap //////////

bmp::Bitmap::Bitmap(uint32_t width, uint32_t height) :
    _data(new Colour[width * height]),
    _width(width), _height(height)
{

}

bmp::Bitmap::~Bitmap()
{
    delete[] _data;
}

void bmp::Bitmap::loadFile(const char* filename)
{
    FileHeader fileHeader;
    InfoHeader infoHeader;

    std::ifstream file(filename, std::ios::binary);
    assert(file.is_open());

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    uint16_t bitDepth = infoHeader.depth;
    if ((bitDepth != 16) && (bitDepth != 24) && (bitDepth != 32)) 
        return;

    uint32_t unpaddedWidth = (infoHeader.depth >> 3) * _width;
    uint32_t paddedWidth = ((unpaddedWidth - 1) & 0xFFFFFFFC) + 4;
    uint32_t padBytes = paddedWidth - unpaddedWidth;

    Bitmap newBitmap(infoHeader.width, infoHeader.height);
    swap(newBitmap);

    for (int32_t y = _height - 1; y >= 0; y--) {
        for (uint32_t x = 0; x < _width; x++) {
            switch (infoHeader.depth) {
                case 16: {
                    RGB565 colour;
                    file.read(reinterpret_cast<char*>(&colour), 2);
                    setPixel(x, y, colour);
                    } break;
                case 24: {
                    RGB888 colour;
                    file.read(reinterpret_cast<char*>(&colour), 3);
                    setPixel(x, y, colour);
                    } break;
                case 32: {
                    ARGB8888 colour;
                    file.read(reinterpret_cast<char*>(&colour), 4);
                    setPixel(x, y, colour);
                    } break;
            }
        }

        uint8_t padding = 0;
        for (uint32_t i = 0; i < padBytes; i++) 
            file.read(reinterpret_cast<char*>(&padding), sizeof(padding));
    }

    file.close();
}

void bmp::Bitmap::saveFile(const char* filename, BitDepth depth) const
{
    FileHeader fileHeader;
    fileHeader.magic = 0x4D42;
    fileHeader.size = sizeof(fileHeader) + sizeof(InfoHeader);
    fileHeader.reserved1 = 0;
    fileHeader.reserved2 = 0;
    fileHeader.offset = fileHeader.size;

    InfoHeader infoHeader;
    infoHeader.size = sizeof(infoHeader);
    infoHeader.width = _width;
    infoHeader.height = _height;
    infoHeader.planes = 1;
    infoHeader.compression = 0;
    infoHeader.horizontalRes = 1;
    infoHeader.verticalRes = 1;
    infoHeader.paletteSize = 0;
    infoHeader.importantColours = 0;

    switch (depth) {
        case BITS_16: infoHeader.depth = 16; break;
        case BITS_24: infoHeader.depth = 24; break;
        case BITS_32: infoHeader.depth = 32; break;
        default:      infoHeader.depth = 24; break;
    }

    uint32_t unpaddedWidth = (infoHeader.depth >> 3) * _width;
    uint32_t paddedWidth = ((unpaddedWidth - 1) & 0xFFFFFFFC) + 4;
    uint32_t padBytes = paddedWidth - unpaddedWidth;

    infoHeader.imageSize = paddedWidth * _height;
    fileHeader.size += infoHeader.imageSize;

    std::ofstream file(filename, std::ios::binary);
    assert(file.is_open());

    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    for (int32_t y = _height - 1; y >= 0; y--) {
        for (uint32_t x = 0; x < _width; x++) {
            switch (depth) {
                case BITS_16: {
                    RGB565 colour(getPixel(x, y));
                    file.write(reinterpret_cast<char*>(&colour), 2);
                    } break;
                case BITS_24: {
                    RGB888 colour(getPixel(x, y));
                    file.write(reinterpret_cast<char*>(&colour), 3);
                    } break;
                case BITS_32: {
                    ARGB8888 colour(getPixel(x, y));
                    file.write(reinterpret_cast<char*>(&colour), 4);
                    } break;
            }
        }

        for (uint32_t i = 0; i < padBytes; i++) 
            file.put(0);
    }

    file.close();
}

void bmp::Bitmap::fill(Colour colour)
{
    for (uint32_t y = 0; y < _height; y++) {
        for (uint32_t x = 0; x < _width; x++)
            setPixel(x, y, colour);
    }
}

void bmp::Bitmap::resize(uint32_t width, uint32_t height)
{
    Bitmap newBitmap(width, height);

    for (uint32_t y = 0; y < std::min(height, _height); y++) {
        for (uint32_t x = 0; x < std::min(width, _width); x++)
            newBitmap.setPixel(x, y, getPixel(x, y));
    }

    swap(newBitmap);
}


////////// Utility Functions //////////

void bmp::drawLine(Bitmap& bitmap, Bitmap::Colour c, int x0, int y0, int x1, int y1)
{
    bool steep = abs(y1 - y0) > abs(x1 - x0);

    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int deltaX = x1 - x0;
    int deltaY = abs(y1 - y0);
    int error = deltaX / 2;
    int stepY = (y0 < y1 ? 1 : -1);
    int y = y0;

    for (int x = x0; x < x1; x++) {
        if (steep) {
            bitmap.setPixelSafe(y, x, c);
        } else {
            bitmap.setPixelSafe(x, y, c);
        }
        if ((error -= deltaY) < 0) {
            error += deltaX;
            y += stepY;
        }
    }
}

