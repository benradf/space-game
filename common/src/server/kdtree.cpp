/// \file kdtree.cpp
/// \brief Provides a generic kdtree data structure.
/// \author Ben Radford 
/// \date 7th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "kdtree.hpp"

enum SplitAxis {
    SPLIT_AXIS_X,
    SPLIT_AXIS_Y,
    SPLIT_AXIS_Z,
    SPLIT_LEAF
};

struct SplitPlane {
    SplitAxis axis;
    float position;
    bool minBound;
    Triangle* tri;
};

typedef std::vector<SplitPlane>::iterator SplitIter;

std::auto_ptr<KDTree> constructKDTree(const std::vector<Triangle>& triangles)
{
    std::vector<SplitPlane> splitPlanes;

    foreach (const Triangle& tri, triangles) {
        vol::AABB bounds = tri.determineBounds();
        const Vector3& min = bounds.getMin();
        const Vector3& max = bounds.getMax();

        splitPlanes.push(SplitPlane(SPLIT_AXIS_X, min.x));
        splitPlanes.push(SplitPlane(SPLIT_AXIS_X, max.x));
        splitPlanes.push(SplitPlane(SPLIT_AXIS_Y, min.y));
        splitPlanes.push(SplitPlane(SPLIT_AXIS_Y, max.y));
        splitPlanes.push(SplitPlane(SPLIT_AXIS_Z, min.z));
        splitPlanes.push(SplitPlane(SPLIT_AXIS_Z, max.z));
    }
}

SplitIter findOptimalSplit(SplitIter begin, SplitIter end)
{
    int minCost = -1;
    SplitIter minSplit = begin;

    for (SplitIter i = begin; i != end; ++i) {
        int cost = 
    }
}

void createNode(SplitIter x0, SplitIter x1, SplitIter y0, SplitIter y1, SplitIter z0, SplitIter z1)
{
    SplitIter
}

