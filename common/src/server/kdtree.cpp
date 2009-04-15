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
#include <limits>
#include <core.hpp>
#include <list>


#include <iostream>
using namespace std;

enum SplitAxis {
    SPLIT_AXIS_X = 0,
    SPLIT_AXIS_Y = 1,
    SPLIT_AXIS_Z = 2,
    SPLIT_LEAF
};

typedef std::list<class SplitPlane*> SplitList;
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
        float _costL;
        float _costR;
        float _cost;
        int _countL;
        int _countR;

        bool _used;
};

struct LessThanPtrs {
    bool operator()(const SplitPlane* a, const SplitPlane* b) {
        return (*a < *b);
    }
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

    cout << "duplicatePlanes of " << &plane << endl;

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

        cout << plane._others[i] << " -> " << planes[i] << endl;
    }

    std::copy(planes, planes + 6, iter);

    return iter;
}

struct MinCost {
    SplitPlane* operator()(SplitPlane* min, SplitPlane* next) const {
        return ((next->_cost < min->_cost) || ((next->_cost == min->_cost) && next->_used) ? next : min);
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
        clampPos(clampPos_), volumeL(volL), volumeR(volR), axis(axis_),
        prevPos(std::numeric_limits<float>::quiet_NaN()), prevAxis(SPLIT_AXIS_X)
    {
        for (int i = 0; i < 3; i++) 
            countL[i] = 0, countR[i] = count;
    }
    ~UpdateCosts() {
        char axes[3] = { 'X', 'Y', 'Z' };
        if (countPending.empty()) 
            return;
        ;//cout << "COUNTS STILL PENDING ON UpdateCosts DESTRUCTION" << endl;
        int peakCountR = countR[countPending.back()->_axis];
        if (!countPending.back()->_minBound && (countPending.size() == 1)) {
            peakCountR--;
        }
        foreach (SplitPlane* p, countPending) {
            if (!p->_minBound) {
                countR[p->_axis]--;
                printCounts();
            }
            ;//cout << "processing pending count for plane " << p << endl;
            ;//cout << "    p->_position = " << p->_position << endl;
            ;//cout << "    p->_axis = " << axes[p->_axis] << endl;
            p->_countL = countL[p->_axis];
            p->_countR = peakCountR;
            p->_costL = p->_volumeL * float(p->_countL);
            p->_costR = p->_volumeR * float(p->_countR);
            p->_cost = p->_costL + p->_costR;
            if ((p->_volumeL < 0.00001f) || (p->_volumeR < 0.00001f)) {
                p->_cost = 100.0f;
            }
            //cout << "update " << p << ": cost = " << p->_cost << endl;
            assert(p->_countL >= 0.0f);
            assert(p->_countR >= 0.0f);
            assert(p->_cost >= 0.0f);
            ;//cout << "    p->_countL = " << p->_countL << endl;
            ;//cout << "    p->_countR = " << p->_countR << endl;
        }
    }
    static UpdateCosts makeL(const SplitPlane& split) {
        return UpdateCosts(split._countL, true, split._volumeL, split._volumeR, split._axis, false, true, split._position);
    }
    static UpdateCosts makeR(const SplitPlane& split) {
        return UpdateCosts(split._countR, true, split._volumeL, split._volumeR, split._axis, true, false, split._position);
    }
    void printCounts() {
        ;//cout << "countL = { ";
        char axes[3] = { 'X', 'Y', 'Z' };
        for (int i = 0; i < 3; i++) {
            ;//cout << axes[i] << " = " << countL[i];
            if (i != 2) 
                ;//cout << ", ";
        }
        ;//cout << " }" << endl;
        ;//cout << "countR = { ";
        for (int i = 0; i < 3; i++) {
            ;//cout << axes[i] << " = " << countR[i];
            if (i != 2) 
                ;//cout << ", ";
        }
        ;//cout << " }" << endl;
    }
    void operator()(SplitPlane* plane) {
        //float posL = plane->_others[2*plane->_axis]->_position;
        //float posR = plane->_others[2*plane->_axis+1]->_position;
        //bool flat = (posL == posR);

        ;//cout << "UpdateCosts::operator()(" << plane << ")" << endl;

        char axes[3] = { 'X', 'Y', 'Z' };

#if 0
        ;//cout << "    prevPos = " << prevPos << endl;
        ;//cout << "    plane->_position = " << plane->_position << endl;
        ;//cout << "    prevAxis = " << axes[prevAxis] << endl;
        ;//cout << "    plane->_axis = " << axes[plane->_axis] << endl;
#endif
        bool flat = ((prevPos == plane->_position) && (prevAxis == plane->_axis));
        prevPos = plane->_position;
        prevAxis = plane->_axis;
#if 0
        ;//cout << "    flat = " << flat << endl;
        printCounts();
#endif

//#if 0
        //if (!flat) {
            if (!plane->_minBound) {
                if (clampMax && (plane->_position > clampPos) && (axis == plane->_axis)) {
                    plane->_position = clampPos;
                    cout << "clamping max " << plane << endl;
                    // TODO: Adjust when clamping.
                    plane->_volumeL -= (volumeR - plane->_volumeR);
                    plane->_volumeR = volumeR;
                }
                //countR[plane->_axis]--;
            } else {
                if (clampMin && (plane->_position < clampPos) && (axis == plane->_axis)) {
                    plane->_position = clampPos;
                    cout << "clamping min " << plane << endl;
                    plane->_volumeR -= (volumeL - plane->_volumeL);
                    plane->_volumeL = volumeL;
                }
                //countL[plane->_axis]++;
                //printCounts();
            }
        //}
//#endif

        if (updateVol) {
            if (plane->_axis == axis) {
                if (clampMax) {
                    assert(plane->_volumeR >= volumeR);
                    plane->_volumeR -= volumeR;
                } else {
                    assert(plane->_volumeL >= volumeL);
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
            assert(plane->_volumeR >= 0.0f);
            assert(plane->_volumeL >= 0.0f);
        }

#if 0
        if (flat) {
            if (plane->_minBound) {
                //countL[plane->_axis]++;
                //printCounts();
            }
        } else
#endif
        
        if (!countPending.empty()) {
            if (flat) {
            } else {
                int peakCountR = countR[countPending.back()->_axis];
                if (!countPending.back()->_minBound && (countPending.size() == 1)) {
                    peakCountR--;
                }
                foreach (SplitPlane* p, countPending) {
                    if (!p->_minBound) {
                        countR[p->_axis]--;
                        printCounts();
                    }
                    ;//cout << "processing pending count for plane " << p << endl;
                    ;//cout << "    p->_position = " << p->_position << endl;
                    ;//cout << "    p->_axis = " << axes[p->_axis] << endl;
                    p->_countL = countL[p->_axis];
                    p->_countR = peakCountR;
                    p->_costL = p->_volumeL * float(p->_countL);
                    p->_costR = p->_volumeR * float(p->_countR);
                    p->_cost = p->_costL + p->_costR;
                    if ((p->_volumeL < 0.00001f) || (p->_volumeR < 0.00001f)) {
                        p->_cost = 100.0f;
                    }
                    //cout << "update " << p << ": cost = " << p->_cost << endl;
                    assert(p->_countL >= 0.0f);
                    assert(p->_countR >= 0.0f);
                    assert(p->_cost >= 0.0f);
                    ;//cout << "    p->_countL = " << p->_countL << endl;
                    ;//cout << "    p->_countR = " << p->_countR << endl;
                }
            }
            if (countPending.back()->_minBound) {
                countL[countPending.back()->_axis]++;
                //countL[plane->_axis]++;
                printCounts();
            }
            if (!flat) {
                countPending.clear();
            }
        }

        countPending.push_back(plane);

        //plane->_countL = countL[plane->_axis];
        //plane->_countR = countR[plane->_axis];
        //plane->_cost = plane->_volumeL * float(plane->_countL) + 
        //               plane->_volumeR * float(plane->_countR);

#if 0
        //if (!flat) {
            if (plane->_minBound) {
                if (clampMin && (plane->_position < clampPos) && (axis == plane->_axis)) {
                    plane->_position = clampPos;
                    plane->_volumeR -= (volumeL - plane->_volumeL);
                    plane->_volumeL = volumeL;
                }
                //countL[plane->_axis]++;
                //printCounts();
            }
        //}
#endif
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
    std::vector<SplitPlane*> countPending;
    float prevPos;
    SplitAxis prevAxis;
};

struct DuplicateOverlapping {
    DuplicateOverlapping(SplitList& list, SplitIter iter) :
        insertIter(inserter(list, iter)) {}
    void operator()(const SplitPlane* plane) {
        cout << "duplicate " << plane << endl;
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

void createNode(SplitList& list, int depth, float cost)
{
//    if (depth == 4) 
//        return;

    cout << "create node (depth = " << depth << ", cost = " << cost << ")" << endl;

    if (list.empty()) {
        cout << "    no splits to choose from" << endl;
        return;
    }

    char axes[3] = { 'X', 'Y', 'Z' };

    printSplits("ALL", list.begin(), list.end());

    SplitPlane& split = *std::accumulate(list.begin(), list.end(), *list.begin(), MinCost());
    cout << "    choose split(" << &split << ") on axis " << axes[split._axis] << " at " << split._position << endl;
    if (split._cost + 0.00001f >= cost) {
        cout << "cheapest to stop splitting here" << endl;
        return;
    }
    split._used = true;

    cerr << depth << " \033[01;32m(cost: " << cost << ")\033[00m\t";
    for (int i = 1; i < depth; i++) 
        cerr << "| ";
    cerr << "\033[01;31msplit at " << split._position << " on axis " << axes[split._axis];
    cerr << "\033[00m (cost: " << split._cost << ")" << endl;

    float costL = split._costL;
    float costR = split._costR;
    SplitIter beginBoth = std::stable_partition(list.begin(), list.end(), OnSide(split, SplitPlane::LEFT));
    cout << "beginBoth = " << *beginBoth << endl;
    //printSplits("PARTITIONED1", first + 1, end);
    SplitIter endBoth = std::stable_partition(beginBoth, list.end(), OnSide(split, SplitPlane::BOTH));
    cout << "endBoth = " << *endBoth << endl;
    //printSplits("PARTITIONED", list.begin(), list.end());

    UpdateCosts updaterL = UpdateCosts::makeL(split);
    UpdateCosts updaterR = UpdateCosts::makeR(split);

#if 0
    SplitList::difference_type indexFirst = std::distance(list.begin(), first);
    SplitList::difference_type indexBegin = std::distance(list.begin(), begin);
    SplitList::difference_type indexBeginBoth = std::distance(list.begin(), beginBoth);
    SplitList::difference_type indexEndBoth = std::distance(list.begin(), endBoth);
    SplitList::difference_type indexEnd = std::distance(list.begin(), end);

    ptrdiff_t overlap = std::distance(beginBoth, endBoth);
    list.reserve(list.size() + overlap);

    first = list.begin() + indexFirst;
    begin = list.begin() + indexBegin;
    beginBoth = list.begin() + indexBeginBoth;
    endBoth = list.begin() + indexEndBoth;
    end = list.begin() + indexEnd;
#endif

    //printSplits("BOTH", beginBoth, endBoth);
    //cout << "about to duplicate" << endl;

    std::for_each(beginBoth, endBoth, DuplicateOverlapping(list, beginBoth));
    //std::advance(end, overlap);

    //printSplits("DUPLICATED", list.begin(), list.end());

    //std::stable_sort(begin, endBoth, LessThanPtrs());
    //std::stable_sort(endBoth, end, LessThanPtrs());

    SplitList listL;
    listL.splice(listL.begin(), list, list.begin(), beginBoth);
    listL.sort(LessThanPtrs());
    cout << "listL.size() = " << listL.size() << endl; // O(n), for debugging only

    SplitList listR;
    listR.splice(listR.begin(), list, beginBoth, list.end());
    listR.sort(LessThanPtrs());
    cout << "listR.size() = " << listR.size() << endl; // O(n), for debugging only

    //printSplits("ALL AFTER DUPLICATE", first + 1, end);
    std::for_each(listL.begin(), listL.end(), updaterL);
    printSplits("UPDATE_L", listL.begin(), listL.end());
    std::for_each(listR.begin(), listR.end(), updaterR);
    printSplits("UPDATE_R", listR.begin(), listR.end());

    //createNode(list, first + 1, beginBoth, depth + 1);
    //createNode(list, beginBoth, end, depth + 1);
    
#if 0
    indexFirst = std::distance(list.begin(), first);
    indexBegin = std::distance(list.begin(), begin);
    indexBeginBoth = std::distance(list.begin(), beginBoth);
    indexEndBoth = std::distance(list.begin(), endBoth);
    indexEnd = std::distance(list.begin(), end);
#endif

    // TODO: Fix indices getting messed up by vector insertions.

    createNode(listL, depth + 1, costL);

#if 0
    first = list.begin() + indexFirst;
    begin = list.begin() + indexBegin;
    beginBoth = list.begin() + indexBeginBoth;
    endBoth = list.begin() + indexEndBoth;
    end = list.begin() + indexEnd;
#endif

    createNode(listR, depth + 1, costR);
}

#include <iostream>
using namespace std;

void createKDTree()
{
    SplitList list;

    vol::AABB bounds(Vector3(-5.0f, -5.0f, -5.0f), Vector3(5.0f, 5.0f, 5.0f));

    vol::AABB b1(Vector3(-2.0f, 1.0f, 0.0f), Vector3(4.0f, 2.0f, 0.0f));
    //SplitPlane::createFromBounds(b1, bounds, std::inserter(list, list.begin()));

    vol::AABB b2(Vector3(-3.0f, -3.0f, 0.0f), Vector3(-1.0f, -1.0f, 0.0f));
    //SplitPlane::createFromBounds(b2, bounds, std::inserter(list, list.begin()));

    vol::AABB b3(Vector3(-3.0f, 2.0f, 0.0f), Vector3(-1.0f, 4.0f, 0.0f));
    //SplitPlane::createFromBounds(b3, bounds, std::inserter(list, list.begin()));

    vol::AABB b4(Vector3(1.0f, -4.0f, 0.0f), Vector3(2.0f, 0.0f, 0.0f));
    //SplitPlane::createFromBounds(b4, bounds, std::inserter(list, list.begin()));

    vol::AABB b5(Vector3(3.0f, -3.0f, 0.0f), Vector3(4.0f, 0.0f, 0.0f));
    //SplitPlane::createFromBounds(b5, bounds, std::inserter(list, list.begin()));

    vol::AABB b6(Vector3(-3.0f, -2.0f, 0.0f), Vector3(2.0f, 3.0f, 0.0f));
    SplitPlane::createFromBounds(b6, bounds, std::inserter(list, list.begin()));

    vol::AABB b7(Vector3(-1.0f, -4.0f, 0.0f), Vector3(4.0f, 1.0f, 0.0f));
    SplitPlane::createFromBounds(b7, bounds, std::inserter(list, list.begin()));

    //std::stable_sort(list.begin(), list.end(), LessThanPtrs());
    list.sort(LessThanPtrs());

    std::for_each(list.begin(), list.end(), UpdateCosts(2));

    createNode(list, 1, 100.0f);
}


