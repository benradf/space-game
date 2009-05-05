/// \file kdtree.hpp
/// \brief Provides a generic kdtree data structure.
/// \author Ben Radford 
/// \date 7th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef KDTREE_HPP
#define KDTREE_HPP


#include <math/vecmath.hpp>
#include <math/volumes.hpp>
#include <math/prim.hpp>
#include <vector>
#include <memory>
#include <stdint.h>


enum SplitAxis {
    SPLIT_AXIS_X = 0,
    SPLIT_AXIS_Y = 1,
    SPLIT_AXIS_Z = 2,
    SPLIT_LEAF
};


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

        typedef uint32_t Reference;

        KDTreeData(const char* filename);
        KDTreeData(size_t nodes, size_t triangles, size_t references);
        ~KDTreeData();

        size_t getNodeCount() const;
        size_t getReferenceCount() const;
        size_t getTriangleCount() const;

        KDTreeNode& getNode(size_t index);
        const KDTreeNode& getNode(size_t index) const;

        Triangle& getTriangle(size_t index);
        const Triangle& getTriangle(size_t index) const;

        Reference& getReference(size_t index);
        const Reference& getReference(size_t index) const;

        void saveFile(const char* filename) const;
        void loadFile(const char* filename);

        Validity checkValidity() const;
        size_t getMemoryUsage() const;

    private:
        KDTreeData(const KDTreeData&);
        KDTreeData& operator=(const KDTreeData&);

        static const int MAX_NODES = 1 << 15;
        static const int MAX_TRIANGLES = 1 << 30;

        KDTreeNode* _nodes;
        size_t _nodeCount;

        Reference* _references;
        size_t _referenceCount;

        Triangle* _triangles;
        size_t _triangleCount;
};


class KDTree {
    public:
        static const int MAX_DEPTH = 22;

        typedef std::auto_ptr<KDTree> Ptr;
        typedef std::vector<Triangle> Triangles;

        KDTree(std::auto_ptr<KDTreeData> data);

        void save(const char* filename) const;

        template<typename T>
        void walkEntireTree(T& walker) const;

        template<typename T>
        void process(T& visitor, const vol::AABB& aabb) const;

        template<typename T>
        void process(T& visitor, const vol::Ray& ray) const;

        static Ptr create(const Triangles& triangles, const vol::AABB& bounds);
        static Ptr load(const char* filename);

        size_t getMemoryUsage() const;

    private:
        KDTree(const KDTree&);
        KDTree& operator=(const KDTree&);

        template<typename T>
        void walkEntireTree(const KDTreeNode& node, T& walker) const;

        template<typename T>
        void process(const KDTreeNode& node, T& visitor, const vol::AABB& aabb) const;

        template<typename T>
        void process(const KDTreeNode& node, T& visitor, const vol::Ray& ray) const;

        std::auto_ptr<KDTreeData> _data;
};


////////// KDTreeData //////////

inline size_t KDTreeData::getNodeCount() const
{
    return _nodeCount;
}

inline size_t KDTreeData::getTriangleCount() const
{
    return _triangleCount;
}

inline size_t KDTreeData::getReferenceCount() const
{
    return _referenceCount;
}

inline KDTreeNode& KDTreeData::getNode(size_t index)
{
    assert(index < _nodeCount);

    return _nodes[index];
}

inline const KDTreeNode& KDTreeData::getNode(size_t index) const
{
    assert(index < _nodeCount);

    return _nodes[index];
}

inline Triangle& KDTreeData::getTriangle(size_t index)
{
    assert(index < _triangleCount);

    return _triangles[index];
}

inline const Triangle& KDTreeData::getTriangle(size_t index) const
{
    assert(index < _triangleCount);

    return _triangles[index];
}

inline KDTreeData::Reference& KDTreeData::getReference(size_t index)
{
    assert(index <_referenceCount);

    return _references[index];
}

inline const KDTreeData::Reference& KDTreeData::getReference(size_t index) const
{
    assert(index <_referenceCount);

    return _references[index];
}


////////// KDTree //////////

template<typename T>
void KDTree::walkEntireTree(T& walker) const
{
    walkEntireTree(_data->getNode(0), walker);
}

template<typename T>
inline void KDTree::process(T& visitor, const vol::AABB& aabb) const
{
    process(_data->getNode(0), visitor, aabb);
}

template<typename T>
inline void KDTree::process(T& visitor, const vol::Ray& ray) const
{
    process(_data->getNode(0), visitor, ray);
}

template<typename T>
void KDTree::walkEntireTree(const KDTreeNode& node, T& walker) const
{
    if (node.splitAxis == SPLIT_LEAF) {
        for (size_t i = 0; i < node.triangleCount; i++)
            walker.triangle(_data->getTriangle(_data->getReference(node.triangles + i)));

        return;
    }

    walker.left(SplitAxis(node.splitAxis), node.splitPosition);
    walkEntireTree(_data->getNode(node.left), walker);
    walker.up();

    walker.right(SplitAxis(node.splitAxis), node.splitPosition);
    walkEntireTree(_data->getNode(node.right), walker);
    walker.up();
}

template<typename T>
void KDTree::process(const KDTreeNode& node, T& visitor, const vol::AABB& aabb) const
{
    if (node.splitAxis == SPLIT_LEAF) {
        for (size_t i = 0; i < node.triangleCount; i++)
            visitor(_data->getTriangle(_data->getReference(node.triangles + i)));

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
void KDTree::process(const KDTreeNode& node, T& visitor, const vol::Ray& ray) const
{
    // TODO: Implement this.
    assert(false);
}


#endif  // KDTREE_HPP

