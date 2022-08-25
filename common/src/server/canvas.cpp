/// \file canvas.cpp
/// \brief Provides canvases to draw on.
/// \author Ben Radford 
/// \date 20th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "canvas.hpp"


////////// SpatialCanvas //////////

SpatialCanvas::SpatialCanvas(const vol::AABB& bounds, int scale, Axis plane) :
    _plane(plane),
    _bounds(bounds),
    _scale(scale),
    _bitmap(1, 1)
{
    switch (_plane) {
        case X_AXIS:
            _bitmap.resize(
                int(_bounds.getLengthY()) * scale + 1,
                int(_bounds.getLengthZ()) * scale + 1);
            break;
        case Y_AXIS:
            _bitmap.resize(
                int(_bounds.getLengthX()) * scale + 1,
                int(_bounds.getLengthZ()) * scale + 1);
            break;
        case Z_AXIS:
            _bitmap.resize(
                int(_bounds.getLengthX()) * scale + 1,
                int(_bounds.getLengthY()) * scale + 1);
            break;
    }

    _bitmap.fill(bmp::Bitmap::Colour(255, 255, 255, 0));
}

void SpatialCanvas::drawAABB(const vol::AABB& aabb, Colour colour)
{
    Point2D min = convertCoords(aabb.getMin());
    Point2D max = convertCoords(aabb.getMax());

    bmp::drawLine(_bitmap, colour, min.x, max.y, max.x, max.y);
    bmp::drawLine(_bitmap, colour, max.x, max.y, max.x, min.y);
    bmp::drawLine(_bitmap, colour, max.x, min.y, min.x, min.y);
    bmp::drawLine(_bitmap, colour, min.x, min.y, min.x, max.y);
}

void SpatialCanvas::drawTriangle(const Triangle& triangle, Colour colour)
{
    drawTriangle(triangle, colour, colour, colour);
}

void SpatialCanvas::drawTriangle(const Triangle& triangle, Colour c0, Colour c1, Colour c2)
{
    Point2D v0 = convertCoords(triangle.getV0());
    Point2D v1 = convertCoords(triangle.getV1());
    Point2D v2 = convertCoords(triangle.getV2());

    bmp::drawLine(_bitmap, c0, v0.x, v0.y, v1.x, v1.y);
    bmp::drawLine(_bitmap, c1, v1.x, v1.y, v2.x, v2.y);
    bmp::drawLine(_bitmap, c2, v2.x, v2.y, v0.x, v0.y);
}

const bmp::Bitmap& SpatialCanvas::getBitmap() const
{
    return _bitmap;
}

SpatialCanvas::Point2D SpatialCanvas::convertCoords(const Vector3& coords)
{
    Point2D point;

    switch (_plane) {
        case X_AXIS:
            point.x = int(coords.y * _scale);
            point.y = int(coords.z * _scale);
            break;
        case Y_AXIS:
            point.x = int(coords.x * _scale);
            point.y = int(coords.z * _scale);
            break;
        case Z_AXIS:
            point.x = int(coords.x * _scale);
            point.y = int(coords.y * _scale);
            break;
    }

    point.x = _bitmap.getWidth() / 2 + point.x;
    point.y = _bitmap.getHeight() / 2 - point.y;

    return point;
}

