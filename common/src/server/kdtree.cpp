/// \file kdtree.cpp
/// \brief Provides a generic kdtree data structure.
/// \author Ben Radford 
/// \date 7th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "kdtree.hpp"

enum SplitAxis {
    SPLIT_AXIS_X = 0,
    SPLIT_AXIS_Y = 1,
    SPLIT_AXIS_Z = 2,
    SPLIT_LEAF
};

class SplitPlane {
    public:
        enum Side { LEFT, BOTH, RIGHT };

        bool operator<(const SplitPlane& plane) const;
        Side whichSide(const SplitPlane& plane) const;

        static void createFromBounds(const vol::AABB& bounds, std::vector<SplitPlane> vec);

    private:
        const SplitPlane* _others[6];

        SplitAxis axis;
        float position;
        bool minBound;
        Triangle* tri;

        float percent;
        int countL;
        int countR;
        int cost;
};

bool SplitPlane::operator<(const SplitPlane& plane) const
{
    return (whichSide(plane) == RIGHT);
}

Side SplitPlane::whichSide(const SplitPlane& plane) const
{
    assert((axis >= 0) && (axis <= 2));

    const SplitPlane& planeL = plane._other[2*axis];
    const SplitPlane& planeR = plane._other[2*axis+1];

    if (planeL.position > position)
        return RIGHT;

    if (planeR.position < position)
        return LEFT;
    
    return BOTH;
}

void SplitPlane::createFromBounds(const vol::AABB& bounds, std::vector<SplitPlane*> vec)
{
    const Vector3& min = bounds.getMin();
    const Vector3& max = bounds.getMax();

    SplitPlane* left = new SplitPlane(SPLIT_AXIS_X, min.x);
    SplitPlane* right = new SplitPlane(SPLIT_AXIS_X, max.x);
    SplitPlane* front = new SplitPlane(SPLIT_AXIS_Y, min.y);
    SplitPlane* back = new SplitPlane(SPLIT_AXIS_Y, max.y);
    SplitPlane* bottom = new SplitPlane(SPLIT_AXIS_Z, min.z);
    SplitPlane* top = new SplitPlane(SPLIT_AXIS_Z, max.z);

    left->_others[0] = left;
    left->_others[1] = right;
    left->_others[2] = front;
    left->_others[3] = back;
    left->_others[4] = bottom;
    left->_others[5] = top;

    for (int i = 0; i < 6; i++) {
        right->_others[i] = left->_other[i];
        front->_others[i] = left->_other[i];
        back->_others[i] = left->_other[i];
        bottom->_others[i] = left->_other[i];
        top->_others[i] = left->_other[i];
    }
}

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

void splitBounds(SplitIter split, const vol::AABB& bounds)
{
    const Vector3& leftMin = bounds.getMin();
    const Vector3& rightMax = bounds.getMax();

    Vector3 leftMax = bounds.getMax();
    Vector3 rightMin = bounds.getMin();

    switch (split->axis) {
        case SPLIT_AXIS_X:
            leftMax.x = split->position;
            rightMin.x = split->position;
            break;
        case SPLIT_AXIS_Y:
            leftMax.y = split->position;
            rightMin.y = split->position;
            break;
        case SPLIT_AXIS_Z:
            leftMax.z = split->position;
            rightMin.y = split->position;
            break;
    }

    split->boundsL = vol::AABB(leftMin, leftMax);
    split->boundsR = vol::AABB(rightMin, rightMax);
}

void determineCost(SplitIter split, int& countLeft, int& countRight, const vol::AABB& bounds)
{
    if (!split->minBound) 
        countRight++;

    split->cost = split->percent * float(countLeft) + 
        (1.0f - split->percent) * float(countRight);

    if (split->minBound) 
        countLeft++;
}

SplitIter findOptimalSplit(SplitIter begin, SplitIter end, const vol::AABB& bounds, int count)
{
    SplitIter min = begin;

    Vector3<int> countLeft = Vector3<int>::ZERO;
    Vector3<int> countRight = Vector3(count, count, count);

    for (SplitIter i = begin; i != end; ++i) {
        switch (i->axis) {
            case SPLIT_AXIS_X:
                determineCost(i, countLeft.x, countRight.x, bounds);
                break;
            case SPLIT_AXIS_Y:
                determineCost(i, countLeft.y, countRight.y, bounds);
                break;
            case SPLIT_AXIS_Z:
                determineCost(i, countLeft.z, countRight.z, bounds);
                break;
        }

        if (i->cost < min->cost) 
            min = i;
    }

    return min;
}

struct SplitSort {
    SplitSort(const SplitPlane& split_) : split(split_) {}
    bool operator()(const SplitPlane& a, const SplitPlane& b) {
        SplitPlane::Side sideA = a.whichSide(split);
        SplitPlane::Side sideB = b.whichSide(split);

        if (sideA == sideB) 
            return (a < b);

        return (sideA < sideB);
    }
    const SplitPlane& split;
};

struct OnSide {
    OnSide(const SplitPlane& split_, SplitPlane::Side side_) :
        split(split_), side(side_) {}
    bool operator()(const SplitSplane& plane) const {
        return split.whichSide(plane, side);
    }
    const SplitPlane& split;
    SplitPlane::Side side;
};

struct Clamp {
    Clamp(const SplitPlane& split_, SplitPlane::Side side_) :
        split(split_), side(side_) {}
    void operator()(SplitPlane& plane) const {
        switch (side) {
            case SplitPlane::LEFT:
                if (!plane.minBound) 
                    plane.position = split.position;
            case SplitPlane::RIGHT:
                if (plane.minBound) 
                    plane.position = split.position;
        }
    }
    const SplitPlane& split;
    SplitPlane::Side side;
};

void createNode(SplitList& list, SplitIter begin, SplitIter end)
{
    SplitIter split = findOptimalSplit(begin, end);
    SplitIter first = begin - 1;

    SplitIter beginBoth = std::stable_partition(begin, end, OnSide(split, SplitPlane::LEFT));
    SplitIter endBoth = std::stable_partition(beginBoth, end, OnSide(split, SplitPlane::BOTH));
    SplitIter beginBothPred = beginBoth - 1;

    std::copy(beginBoth, endBoth, std::inserter(list, beginBoth));
    std::for_each(beginBothPred + 1, beginBoth, Clamp(split, SplitPlane::LEFT));
    std::for_each(beginBoth, endBoth, Clamp(split, SplitPlane::RIGHT));

    createNode(list, first + 1, beginBoth);
    createNode(list, beginBoth, end);
}

void something()
{
    
}










void initialiseSplits(SplitIter begin, SplitIter end)
{
    for (SplitIter i = begin; i != end; ++i) {
    }
}


SplitIter findBestSplit(SplitIter begin, SplitIter end)
{
    SplitIter bestSplit = begin;

    for (SplitIter i = begin; i != end; ++i) {
        if (!i->minBound) 
            i->countR++;

        i->cost = percent * float(i->countL) + 
            (1.0f - percent) * float(i->countR);

        if (i->minBound) 
            i->countL++;
    }

    return bestSplit;
}

void 

void createNode(const std::vector<Triangle*> triangles)
{
    std::vector<SplitPlane> 
}








