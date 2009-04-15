/// \file kdtree.cpp
/// \brief Provides a generic kdtree data structure.
/// \author Ben Radford 
/// \date 7th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "kdtree.hpp"
#include <assert.h>
#include <vector>
#include <numeric>
#include <algorithm>


#include <iostream>
using namespace std;

enum SplitAxis {
    SPLIT_AXIS_X = 0,
    SPLIT_AXIS_Y = 1,
    SPLIT_AXIS_Z = 2,
    SPLIT_LEAF
};

typedef std::vector<class SplitPlane*> SplitList;
typedef SplitList::iterator SplitIter;
typedef std::insert_iterator<SplitList> SplitInsertIter;

class SplitPlane {
    public:
        enum Side { LEFT, BOTH, RIGHT };

        SplitPlane(SplitAxis axis, float pos, const vol::AABB& global, bool minBound);

        bool operator<(const SplitPlane& plane) const;
        Side whichSide(const SplitPlane& plane) const;

        static SplitInsertIter createFromBounds(const vol::AABB& local, const vol::AABB& global, SplitInsertIter iter);
        static SplitInsertIter duplicatePlanes(const SplitPlane& plane, SplitInsertIter iter);

        void Update(const SplitPlane& split, int countL, int countR);

    //private:
        const SplitPlane* _others[6];

        SplitAxis _axis;
        float _position;
        bool _minBound;
        Triangle* _tri;

        float _volumeL;
        float _volumeR;
        float _cost;
        int _countL;
        int _countR;

        bool _used;
};

SplitPlane::SplitPlane(SplitAxis axis, float pos, const vol::AABB& global, bool minBound) :
    _axis(axis), _position(pos), _minBound(minBound), _tri(0), 
    _volumeL(0.0f), _volumeR(0.0f), _cost(0.0f), _used(false)
{
    const Vector3& min = global.getMin();
    const Vector3& max = global.getMax();

    switch (axis) {
        case SPLIT_AXIS_X:
            _volumeL = (pos - min.x) / (max.x - min.x);
            break;
        case SPLIT_AXIS_Y:
            _volumeL = (pos - min.x) / (max.y - min.y);
            break;
        case SPLIT_AXIS_Z:
            _volumeL = (pos - min.x) / (max.z - min.z);
            break;
    }

    _volumeR = 1.0f - _volumeL;
}

bool SplitPlane::operator<(const SplitPlane& plane) const
{
    return (_position < plane._position);
}

SplitPlane::Side SplitPlane::whichSide(const SplitPlane& plane) const
{
    assert((_axis >= 0) && (_axis <= 2));

    const SplitPlane& planeL = *plane._others[2*_axis];
    const SplitPlane& planeR = *plane._others[2*_axis+1];

    if ((planeL._position == planeR._position) && (planeL._position == _position)) 
        return BOTH;

    if (planeL._position >= _position)
        return RIGHT;

    if (planeR._position <= _position)
        return LEFT;
    
    return BOTH;
}

SplitInsertIter SplitPlane::createFromBounds(const vol::AABB& local, const vol::AABB& global, SplitInsertIter iter)
{
    const Vector3& min = local.getMin();
    const Vector3& max = local.getMax();

    SplitPlane* planes[6] = {
        new SplitPlane(SPLIT_AXIS_X, min.x, global, true),
        new SplitPlane(SPLIT_AXIS_X, max.x, global, false),
        new SplitPlane(SPLIT_AXIS_Y, min.y, global, true),
        new SplitPlane(SPLIT_AXIS_Y, max.y, global, false),
        new SplitPlane(SPLIT_AXIS_Z, min.z, global, true),
        new SplitPlane(SPLIT_AXIS_Z, max.z, global, false)
    };

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++)
            planes[i]->_others[j] = planes[j];
    }

    std::copy(planes, planes + 6, iter);

    return iter;
}

SplitInsertIter SplitPlane::duplicatePlanes(const SplitPlane& plane, SplitInsertIter iter)
{
    if ((plane._axis != SPLIT_AXIS_X) || !plane._minBound) 
        return iter;

    SplitPlane* planes[6] = {
        new SplitPlane(*plane._others[0]),
        new SplitPlane(*plane._others[1]),
        new SplitPlane(*plane._others[2]),
        new SplitPlane(*plane._others[3]),
        new SplitPlane(*plane._others[4]),
        new SplitPlane(*plane._others[5])
    };

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++)
            planes[i]->_others[j] = planes[j];
    }

    std::copy(planes, planes + 6, iter);

    return iter;
}

struct MinCost {
    SplitPlane* operator()(SplitPlane* min, SplitPlane* next) const {
        return (next->_cost < min->_cost ? next : min);
    }
};

struct OnSide {
    OnSide(const SplitPlane& split_, SplitPlane::Side side_) :
        split(split_), side(side_) {}
    bool operator()(const SplitPlane* plane) const {
        return (split.whichSide(*plane) == side);
    }
    const SplitPlane& split;
    SplitPlane::Side side;
};

struct UpdateCosts {
    UpdateCosts(int count, bool updateVol_ = false, float volL = 0.0f, float volR = 0.0f, SplitAxis axis_ = SPLIT_AXIS_X, 
            bool clampMin_ = false, bool clampMax_ = false, float clampPos_ = 0.0f) :
        clampMin(clampMin_), clampMax(clampMax_), updateVol(updateVol_),
        clampPos(clampPos_), volumeL(volL), volumeR(volR), axis(axis_)
    {
        for (int i = 0; i < 3; i++) 
            countL[i] = 0, countR[i] = count;
    }
    static UpdateCosts makeL(const SplitPlane& split) {
        return UpdateCosts(split._countL, true, split._volumeL, split._volumeR, split._axis, false, true, split._position);
    }
    static UpdateCosts makeR(const SplitPlane& split) {
        return UpdateCosts(split._countR, true, split._volumeL, split._volumeR, split._axis, true, false, split._position);
    }
    void operator()(SplitPlane* plane) {
        if (!plane->_minBound) {
            if (clampMax && (plane->_position > clampPos) && (axis == plane->_axis)) 
                plane->_position = clampPos;
            countR[plane->_axis]--;
        }

        // TODO: Only clamp if the axis matches the split.

        if (updateVol) {
            if (plane->_axis == axis) {
                if (clampMax) {
                    plane->_volumeR -= volumeR;
                } else {
                    plane->_volumeL -= volumeL;
                }
            } else {
                if (clampMax) {
                    float s = volumeL / (volumeL + volumeR);
                    plane->_volumeL *= s;
                    plane->_volumeR *= s;
                } else {
                    float s = volumeR / (volumeL + volumeR);
                    plane->_volumeL *= s;
                    plane->_volumeR *= s;
                }
            }
        }

        plane->_countL = countL[plane->_axis];
        plane->_countR = countR[plane->_axis];
        plane->_cost = plane->_volumeL * float(plane->_countL) + 
                      plane->_volumeR * float(plane->_countR);

        if (plane->_minBound) {
            if (clampMin && (plane->_position < clampPos) && (axis == plane->_axis))
                plane->_position = clampPos;
            countL[plane->_axis]++;
        }
    }
    float clampPos;
    bool clampMin;
    bool clampMax;
    int countL[3];
    int countR[3];
    float volumeL;
    float volumeR;
    SplitAxis axis;
    bool updateVol;
};

struct DuplicateOverlapping {
    DuplicateOverlapping(SplitList list, SplitIter iter) :
        insertIter(inserter(list, iter)) {}
    void operator()(const SplitPlane* plane) {
        insertIter = SplitPlane::duplicatePlanes(*plane, insertIter);
    };
    SplitInsertIter insertIter;
};

void printSplits(const char* prefix, SplitIter begin, SplitIter end)
{
    char axes[3] = { 'X', 'Y', 'Z' };

    for (SplitIter i = begin; i != end; ++i) {
        const SplitPlane& p = **i;
        cout << "  " << prefix << " split(" << *i << "): [ ";
        cout << "axis = " << axes[p._axis] << ", ";
        cout << "pos = " << p._position << ", ";
        cout << "countL = " << p._countL << ", ";
        cout << "countR = " << p._countR << ", ";
        cout << "volumeL = " << p._volumeL << ", ";
        cout << "volumeR = " << p._volumeR << ", ";
        cout << "cost = " << p._cost << ", ";
        cout << "used = " << p._used << " ]" << endl;
    }

    cout << endl;
}

void createNode(SplitList& list, SplitIter begin, SplitIter end, int depth)
{
//    if (depth == 4) 
//        return;

    cout << "create node (depth = " << depth << ")" << endl;

    if (begin == end) {
        cout << "    no splits to choose from" << endl;
        return;
    }

    char axes[3] = { 'X', 'Y', 'Z' };

    printSplits("ALL", begin, end);

    SplitIter first = begin - 1;

    SplitPlane& split = *std::accumulate(begin, end, *begin, MinCost());
    cout << "    choose split(" << &split << ") on axis " << axes[split._axis] << " at " << split._position << endl;
    if (split._used) {
        cout << "cheapest to stop splitting here" << endl;
        return;
    }
    split._used = true;

    SplitIter beginBoth = std::stable_partition(begin, end, OnSide(split, SplitPlane::LEFT));
    cout << "beginBoth = " << *beginBoth << endl;
    printSplits("PARTITIONED1", first + 1, end);
    SplitIter endBoth = std::stable_partition(beginBoth, end, OnSide(split, SplitPlane::BOTH));
    cout << "endBoth = " << *endBoth << endl;
    printSplits("PARTITIONED2", first + 1, end);
    begin = first + 1;

    printSplits("BOTH", beginBoth, endBoth);

    UpdateCosts updaterL = UpdateCosts::makeL(split);
    UpdateCosts updaterR = UpdateCosts::makeR(split);

    std::for_each(beginBoth, endBoth, DuplicateOverlapping(list, beginBoth));
    cout << "a" << endl;
    //printSplits("DUPLICATE", first + 1, end);
    std::for_each(begin, beginBoth, updaterL);
    cout << "b" << endl;
    //printSplits("UPDATE_L", first + 1, end);
    std::for_each(beginBoth, end, updaterR);
    cout << "c" << endl;
    //printSplits("UPDATE_R", first + 1, end);

    createNode(list, first + 1, beginBoth, depth + 1);
    createNode(list, beginBoth, end, depth + 1);
}

#include <iostream>
using namespace std;

struct LessThanPtrs {
    bool operator()(const SplitPlane* a, const SplitPlane* b) {
        return (*a < *b);
    }
};

void createKDTree()
{
    SplitList list;

    vol::AABB bounds(Vector3(-5.0f, -5.0f, -5.0f), Vector3(5.0f, 5.0f, 5.0f));

    vol::AABB b1(Vector3(-2.0f, 1.0f, 0.0f), Vector3(4.0f, 2.0f, 0.0f));
    SplitPlane::createFromBounds(b1, bounds, std::inserter(list, list.begin()));

    vol::AABB b2(Vector3(-3.0f, -3.0f, 0.0f), Vector3(-1.0f, -1.0f, 0.0f));
    SplitPlane::createFromBounds(b2, bounds, std::inserter(list, list.begin()));

    std::sort(list.begin(), list.end(), LessThanPtrs());

    std::for_each(list.begin(), list.end(), UpdateCosts(2));

    list.insert(list.begin(), new SplitPlane(SPLIT_AXIS_Y, 0.0f, vol::AABB::EMPTY, true));

    createNode(list, list.begin() + 1, list.end(), 1);
}


