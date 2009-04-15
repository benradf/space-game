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

        SplitPlane(SplitAxis axis, float pos, const vol::AABB& bounds, bool minBound);

        bool operator<(const SplitPlane& plane) const;
        Side whichSide(const SplitPlane& plane) const;

        static SplitInsertIter createFromBounds(const vol::AABB& bounds, SplitInsertIter iter);
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
};

SplitPlane::SplitPlane(SplitAxis axis, float pos, const vol::AABB& bounds, bool minBound) :
    _axis(axis), _position(pos), _minBound(minBound), _tri(0), 
    _volumeL(0.0f), _volumeR(0.0f), _cost(0.0f)
{
    const Vector3& min = bounds.getMin();
    const Vector3& max = bounds.getMax();

    switch (axis) {
        case SPLIT_AXIS_X:
            _volumeL = (pos - min.x) / (max.x - min.x);
            break;
        case SPLIT_AXIS_Y:
            _volumeL = (pos - min.x) / (max.x - min.x);
            break;
        case SPLIT_AXIS_Z:
            _volumeL = (pos - min.x) / (max.x - min.x);
            break;
    }

    _volumeR = 1.0f - _volumeL;
}

bool SplitPlane::operator<(const SplitPlane& plane) const
{
    return (whichSide(plane) == RIGHT);
}

SplitPlane::Side SplitPlane::whichSide(const SplitPlane& plane) const
{
    assert((_axis >= 0) && (_axis <= 2));

    const SplitPlane& planeL = *plane._others[2*_axis];
    const SplitPlane& planeR = *plane._others[2*_axis+1];

    if (planeL._position >= _position)
        return RIGHT;

    if (planeR._position <= _position)
        return LEFT;
    
    return BOTH;
}

SplitInsertIter SplitPlane::createFromBounds(const vol::AABB& bounds, SplitInsertIter iter)
{
    const Vector3& min = bounds.getMin();
    const Vector3& max = bounds.getMax();

    SplitPlane* planes[6] = {
        new SplitPlane(SPLIT_AXIS_X, min.x, bounds, true),
        new SplitPlane(SPLIT_AXIS_X, max.x, bounds, false),
        new SplitPlane(SPLIT_AXIS_Y, min.y, bounds, true),
        new SplitPlane(SPLIT_AXIS_Y, max.y, bounds, false),
        new SplitPlane(SPLIT_AXIS_Z, min.z, bounds, true),
        new SplitPlane(SPLIT_AXIS_Z, max.z, bounds, false)
    };

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            cout << "planes[" << i << "]->_others[" << j << "] = planes[" << j << "] = " << planes[j] << endl;
            planes[i]->_others[j] = planes[j];
        }
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
    const SplitPlane* operator()(const SplitPlane* min, const SplitPlane* next) const {
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
    UpdateCosts(int count, float vol, bool clampMin_ = false, 
            bool clampMax_ = false, float clampPos_ = 0.0f) :
        clampMin(clampMin_), clampMax(clampMax_), 
        clampPos(clampPos_), volume(vol)
    {
        for (int i = 0; i < 3; i++) 
            countL[i] = 0, countR[i] = count;
    }
    static UpdateCosts makeL(const SplitPlane& split) {
        return UpdateCosts(split._countL, split._volumeL, false, true, split._position);
    }
    static UpdateCosts makeR(const SplitPlane& split) {
        return UpdateCosts(split._countR, split._volumeR, true, false, split._position);
    }
    void operator()(SplitPlane* plane) {
        if (!plane->_minBound) {
            if (clampMax && (plane->_position > clampPos)) 
                plane->_position = clampPos;
            countR[plane->_axis]--;
        }

        plane->_volumeL *= volume;
        plane->_volumeR *= volume;
        plane->_countL = countL[plane->_axis];
        plane->_countR = countR[plane->_axis];
        plane->_cost = plane->_volumeL * float(plane->_countL) + 
                      plane->_volumeR * float(plane->_countR);

        if (plane->_minBound) {
            if (clampMin && (plane->_position < clampPos))
                plane->_position = clampPos;
            countL[plane->_axis]++;
        }
    }
    float clampPos;
    bool clampMin;
    bool clampMax;
    int countL[3];
    int countR[3];
    float volume;
};

struct DuplicateOverlapping {
    DuplicateOverlapping(SplitList list, SplitIter iter) :
        insertIter(inserter(list, iter)) {}
    void operator()(const SplitPlane* plane) {
        insertIter = SplitPlane::duplicatePlanes(*plane, insertIter);
    };
    SplitInsertIter insertIter;
};

void createNode(SplitList& list, SplitIter begin, SplitIter end, int depth)
{
    if (depth == 2) 
        return;

    SplitIter first = begin - 1;
    //                                                    FIX THIS C-STYLE CAST
    const SplitPlane& split = *std::accumulate(begin, end, (const SplitPlane*)*begin, MinCost());
    cout << "split on axis " << split._axis << " at " << split._position << endl;

    SplitIter beginBoth = std::stable_partition(begin, end, OnSide(split, SplitPlane::LEFT));
    SplitIter endBoth = std::stable_partition(beginBoth, end, OnSide(split, SplitPlane::BOTH));
    begin = first + 1;

    std::for_each(beginBoth, endBoth, DuplicateOverlapping(list, beginBoth));
    std::for_each(begin, beginBoth, UpdateCosts::makeL(split));
    std::for_each(beginBoth, end, UpdateCosts::makeR(split));

    createNode(list, first + 1, beginBoth, depth + 1);
    createNode(list, beginBoth, end, depth + 1);
}

#include <iostream>
using namespace std;

void createKDTree()
{
    SplitList list;
    list.push_back(new SplitPlane(SPLIT_AXIS_Y, 0.0f, vol::AABB::EMPTY, true));

    vol::AABB b1(Vector3(-2.0f, 1.0f, 0.0f), Vector3(4.0f, 2.0f, 0.0f));
    SplitPlane::createFromBounds(b1, std::inserter(list, list.begin()));

    vol::AABB b2(Vector3(-3.0f, -3.0f, 0.0f), Vector3(-1.0f, -1.0f, 0.0f));
    SplitPlane::createFromBounds(b2, std::inserter(list, list.begin()));

    std::for_each(list.begin(), list.end(), UpdateCosts(2, 1.0f));

    createNode(list, list.begin() + 1, list.end(), 1);
}


