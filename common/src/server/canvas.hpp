/// \file canvas.hpp
/// \brief Provides canvases to draw on.
/// \author Ben Radford 
/// \date 20th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef CANVAS_HPP
#define CANVAS_HPP


#include "drawbmp.hpp"
#include "volumes.hpp"
#include "prim.hpp"


class SpatialCanvas {
    public:
        typedef bmp::Bitmap::Colour Colour;

        enum Axis { X_AXIS, Y_AXIS, Z_AXIS };

        SpatialCanvas(const vol::AABB& bounds, int scale, Axis plane);

        void drawAABB(const vol::AABB& aabb, Colour colour);
        void drawTriangle(const Triangle& triangle, Colour colour);
        void drawTriangle(const Triangle& triangle, Colour c0, Colour c1, Colour c2);

        const bmp::Bitmap& getBitmap() const;

    private:
        struct Point2D { int x, y; };

        Point2D convertCoords(const Vector3& coords);

        Axis _plane;
        vol::AABB _bounds;
        float _scale;

        bmp::Bitmap _bitmap;
        int _width;
        int _height;
};


#endif  // CANVAS_HPP

