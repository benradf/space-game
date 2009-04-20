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
#include "prim.hpp"
#include <vector>
#include <memory>


enum SplitAxis {
    SPLIT_AXIS_X = 0,
    SPLIT_AXIS_Y = 1,
    SPLIT_AXIS_Z = 2,
    SPLIT_LEAF
};


class TemporaryNode {
    public:
        typedef std::vector<const Triangle*> Triangles;

        TemporaryNode();
        TemporaryNode(TemporaryNode* left, TemporaryNode* right, SplitAxis axis, float position);
        ~TemporaryNode();

        bool isLeaf() const;

        SplitAxis getAxis() const;
        float getPosition() const;

        const TemporaryNode& getLeft() const;
        const TemporaryNode& getRight() const;

        const Triangles& getTriangles() const;
        void addTriangle(const Triangle* triangle);

        size_t getDescendantCount() const;
        size_t getTriangleCount() const;

        template<typename T>
        void accept(T& visitor);

    private:
        TemporaryNode* _left;
        TemporaryNode* _right;

        SplitAxis _axis;
        float _position;

        Triangles* _triangles;
};

template<typename T>
inline void TemporaryNode::accept(T& visitor)
{
    visitor.visit(*_left);
    visitor.visit(*_right);

    _left->accept(visitor);
    _right->accept(visitor);

}


struct KDTreeNode {
    union {
        struct {
            unsigned splitAxis : 2;
            unsigned left : 15;
            unsigned right : 15;
        };
        struct {
            unsigned : 2;
            unsigned triangleCount : 30;
        };
    };
    union {
        float splitPosition;
        unsigned triangles : 32;
    };
};

class KDTreeData {
    public:
        enum Validity {
            VALID,
            INVALID_STRUCTURE,
            INVALID_INDEX,
            UNREACHABLE_NODES
        };

        KDTreeData(const char* filename);
        KDTreeData(size_t nodeCount, size_t triangleCount);
        ~KDTreeData();

        size_t getNodeCount() const;
        size_t getTriangleCount() const;

        KDTreeNode& getNode(size_t index);
        const KDTreeNode& getNode(size_t index) const;

        Triangle& getTriangle(size_t index);
        const Triangle& getTriangle(size_t index) const;

        void saveFile(const char* filename) const;
        void loadFile(const char* filename);

        Validity checkValidity() const;

    private:
        KDTreeData(const KDTreeData&);
        KDTreeData& operator=(const KDTreeData&);

        static const int MAX_NODES = 1 << 15;
        static const int MAX_TRIANGLES = 1 << 30;

        KDTreeNode* _nodes;
        size_t _nodeCount;

        Triangle* _triangles;
        size_t _triangleCount;
};


class KDTree {
    public:
        KDTree(std::auto_ptr<KDTreeData> data);

        template<typename T>
        void process(T& visitor, const vol::AABB& aabb) const;

        template<typename T>
        void process(T& visitor, const vol::Ray& ray) const;

        static std::auto_ptr<KDTree> load(const char* filename);

    private:
        KDTree(const KDTree&);
        KDTree& operator=(const KDTree&);

        template<typename T>
        void process(const KDTreeNode& node, T& visitor, const vol::AABB& aabb);

        template<typename T>
        void process(const KDTreeNode& node, T& visitor, const vol::Ray& ray);

        std::auto_ptr<KDTreeData> _data;
};


template<typename T>
void KDTree::process(T& visitor, const vol::AABB& aabb) const
{
    process(_data->getNode(0), visitor, aabb);
}

template<typename T>
void KDTree::process(T& visitor, const vol::Ray& ray) const
{
    process(_data->getNode(0), visitor, ray);
}

template<typename T>
void KDTree::process(const KDTreeNode& node, T& visitor, const vol::AABB& aabb)
{
    if (node.splitAxis == SPLIT_LEAF) {
        for (size_t i = 0; i < node.triangleCount; i++) 
            visitor(_data->getTriangle(node.triangles + i));

        return;
    }

    float min = 0.0f, max = 0.0f;

    switch (node.splitAxis) {
        case SPLIT_AXIS_X: min = aabb.getMin().x; max = aabb.getMax().x; break;
        case SPLIT_AXIS_Y: min = aabb.getMin().y; max = aabb.getMax().y; break;
        case SPLIT_AXIS_Z: min = aabb.getMin().z; max = aabb.getMax().z; break;
    }

    if (max <= node.splitPosition) {
        process(_data->getNode(node.left), visitor, aabb);
    } else if (min >= node.splitPosition) {
        process(_data->getNode(node.right), visitor, aabb);
    } else {
        process(_data->getNode(node.left), visitor, aabb);
        process(_data->getNode(node.right), visitor, aabb);
    }
}

template<typename T>
void KDTree::process(const KDTreeNode& node, T& visitor, const vol::Ray& ray)
{
    // TODO: Implement this.
    assert(false);
}


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


#endif  // KDTREE_HPP

