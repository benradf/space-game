/// \file kdtree.hpp
/// \brief Provides a generic kdtree data structure.
/// \author Ben Radford 
/// \date 7th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef KDTREE_HPP
#define KDTREE_HPP


#include <vecmath.hpp>
#include "volumes.hpp"
#include "drawbmp.hpp"
#include <vector>


class Triangle {
    public:
        Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2);

        vol::AABB determineBounds() const;

        const Vector3& getVertex(int index) const;

        const Vector3& getV0() const;
        const Vector3& getV1() const;
        const Vector3& getV2() const;


    //private:
        Vector3 _v0;
        Vector3 _v1;
        Vector3 _v2;
};

class Node {
    public:
        typedef std::vector<const Triangle*> Triangles;

        Node();
        Node(Node* left, Node* right);
        ~Node();

        bool isLeaf() const;

        const Node& getLeft() const;
        const Node& getRight() const;

        const Triangles& getTriangles() const;
        void addTriangle(const Triangle* triangle);

    private:
        Node* _left;
        Node* _right;

        Triangles* _triangles;
};

struct KDTreeNode {
    unsigned children : 30;
    unsigned splitAxis : 2;
    float splitPosition;
};

class KDTree {
    public:

    private:
};


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

        int _scale;
        Axis _plane;
        vol::AABB _bounds;

        bmp::Bitmap _bitmap;
        int _width;
        int _height;
};


#endif  // KDTREE_HPP

