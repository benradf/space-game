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

        static SplitInsertIter createFromTriangle(const Triangle& triangle, const vol::AABB& global, SplitInsertIter iter);
        static SplitInsertIter createFromBounds(const vol::AABB& local, const vol::AABB& global, SplitInsertIter iter);
        static SplitInsertIter duplicatePlanes(const SplitPlane& plane, SplitInsertIter iter);

        void Update(const SplitPlane& split, int countL, int countR);

        bool IsFlat() const;

    //private:
        const SplitPlane* _others[6];

        SplitAxis _axis;
        float _position;
        bool _minBound;
        const Triangle* _tri;

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
        float d = fabs(a->_position - b->_position);
        return (d < 0.00001f ? a->_axis < b->_axis : a->_position < b->_position);
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

SplitInsertIter SplitPlane::createFromTriangle(const Triangle& triangle, const vol::AABB& global, SplitInsertIter iter)
{
    vol::AABB local(triangle.determineBounds());

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
        planes[i]->_tri = &triangle;
    }

    std::copy(planes, planes + 6, iter);

    return iter;
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

bool SplitPlane::IsFlat() const
{
    return (_others[2*_axis]->_position == _others[2*_axis+1]->_position);
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

struct IsFlat {
    bool operator()(const SplitPlane* plane) const {
        return plane->IsFlat();
    }
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
        //bool moveZeroDist = (posL == posR);

        ;//cout << "UpdateCosts::operator()(" << plane << ")" << endl;

        char axes[3] = { 'X', 'Y', 'Z' };

#if 0
        ;//cout << "    prevPos = " << prevPos << endl;
        ;//cout << "    plane->_position = " << plane->_position << endl;
        ;//cout << "    prevAxis = " << axes[prevAxis] << endl;
        ;//cout << "    plane->_axis = " << axes[plane->_axis] << endl;
#endif
        bool moveZeroDist = ((prevPos == plane->_position) && (prevAxis == plane->_axis));
        prevPos = plane->_position;
        prevAxis = plane->_axis;
#if 0
        ;//cout << "    moveZeroDist = " << flat << endl;
        printCounts();
#endif

//#if 0
        //if (!moveZeroDist) {
            if (!plane->_minBound) {
                if (clampMax && (plane->_position > clampPos) && (axis == plane->_axis)) {
                    plane->_position = clampPos;
                    cout << "clamping max " << plane << endl;
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
        if (moveZeroDist) {
            if (plane->_minBound) {
                //countL[plane->_axis]++;
                //printCounts();
            }
        } else
#endif
        
        if (!countPending.empty()) {
            if (moveZeroDist) {
            } else {
                int peakCountR = countR[countPending.back()->_axis];
                if (!countPending.back()->_minBound && (countPending.size() == 1)) {
                    peakCountR--;
                }
                typedef std::vector<SplitPlane*>::iterator Iter;
                Iter iterNonFlat = std::stable_partition(countPending.begin(), countPending.end(), IsFlat());
                for (Iter i = countPending.begin(); i != iterNonFlat; ++i) {
                    SplitPlane* p = *i;
                    if (!p->_minBound) {
                        countR[p->_axis]--;
                        printCounts();
                    }
                    ;//cout << "processing pending count for FLAT plane " << p << endl;
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
                for (Iter i = iterNonFlat; i != countPending.end(); ++i) {
                    SplitPlane* p = *i;
                    if (!p->_minBound) {
                        countR[p->_axis]--;
                        printCounts();
                    }
                    ;//cout << "processing pending count for NON-FLAT plane " << p << endl;
                    ;//cout << "    p->_position = " << p->_position << endl;
                    ;//cout << "    p->_axis = " << axes[p->_axis] << endl;
                    p->_countL = countL[p->_axis];
                    p->_countR = countR[p->_axis];
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
                cout << "countPending.back() = " << countPending.back() << endl;
                countL[countPending.back()->_axis]++;
                //countL[plane->_axis]++;
                printCounts();
            }
            if (!moveZeroDist) {
                countPending.clear();
            }
        }

        countPending.push_back(plane);

        //plane->_countL = countL[plane->_axis];
        //plane->_countR = countR[plane->_axis];
        //plane->_cost = plane->_volumeL * float(plane->_countL) + 
        //               plane->_volumeR * float(plane->_countR);

#if 0
        //if (!moveZeroDist) {
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

Node* createNode(SplitList& list, int depth, float cost, SpatialCanvas& canvas, const vol::AABB& bounds)
{
//    if (depth == 4) 
//        return;

    cout << "create node (depth = " << depth << ", cost = " << cost << ")" << endl;

    canvas.drawAABB(bounds, bmp::Bitmap::Colour(depth * 25, 0, 0, 0));

    cerr << depth << " \033[01;32m(cost: " << cost << ")\033[00m\t";
    for (int i = 1; i < depth; i++) 
        cerr << "| ";

    if (list.empty()) {
        cout << "    no splits to choose from" << endl;
        cerr << "\033[01;35msplit list empty\033[00m" << endl;
        return new Node;
    }

    char axes[3] = { 'X', 'Y', 'Z' };

    printSplits("ALL", list.begin(), list.end());

    SplitPlane& split = *std::accumulate(list.begin(), list.end(), *list.begin(), MinCost());
    cout << "    choose split(" << &split << ") on axis " << axes[split._axis] << " at " << split._position << endl;

    if (split._cost + 0.00001f >= cost) {
        cout << "cheapest to stop splitting here" << endl;
        cerr << "\033[01;33mTriangles: ";
        Node* node = new Node;
        foreach (const SplitPlane* plane, list) {
            if ((plane->_axis != SPLIT_AXIS_X) || (!plane->_minBound)) 
                continue;
            canvas.drawTriangle(*plane->_tri, bmp::Bitmap::Colour(0, 0, 255, 0));
            node->addTriangle(plane->_tri);
            cerr << "[";
            const Vector3& v0 = plane->_tri->_v0;
            cerr << "(" << v0.x << ", " << v0.y << ", " << v0.z << "), ";
            const Vector3& v1 = plane->_tri->_v1;
            cerr << "(" << v1.x << ", " << v1.y << ", " << v1.z << "), ";
            const Vector3& v2 = plane->_tri->_v2;
            cerr << "(" << v2.x << ", " << v2.y << ", " << v2.z << ")";
            cerr << "]   ";
        }
        cerr << "\033[00m" << endl;
        return node;
    }
    split._used = true;

    cerr << "\033[01;31msplit at " << split._position << " on axis " << axes[split._axis];
    cerr << "\033[00m (cost: " << split._cost << ")" << endl;

    const Vector3& boundsMinL = bounds.getMin();
    const Vector3& boundsMaxR = bounds.getMax();
    Vector3 boundsMaxL = boundsMaxR;
    Vector3 boundsMinR = boundsMinL;
    switch (split._axis) {
        case SPLIT_AXIS_X:
            boundsMaxL.x = split._position;
            boundsMinR.x = split._position;
            break;
        case SPLIT_AXIS_Y:
            boundsMaxL.y = split._position;
            boundsMinR.y = split._position;
            break;
        case SPLIT_AXIS_Z:
            boundsMaxL.z = split._position;
            boundsMinR.z = split._position;
            break;
    }

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

    Node* leftNode = createNode(listL, depth + 1, costL, canvas, vol::AABB(boundsMinL, boundsMaxL));

#if 0
    first = list.begin() + indexFirst;
    begin = list.begin() + indexBegin;
    beginBoth = list.begin() + indexBeginBoth;
    endBoth = list.begin() + indexEndBoth;
    end = list.begin() + indexEnd;
#endif

    Node* rightNode = createNode(listR, depth + 1, costR, canvas, vol::AABB(boundsMinR, boundsMaxR));

    return new Node(leftNode, rightNode);
}

#include <iostream>
using namespace std;

void createKDTree()
{
    std::vector<Triangle> triangles;

    triangles.push_back(Triangle(
        Vector3(-3.0f,  0.0f,  0.0f),
        Vector3( 2.0f,  3.0f,  0.0f),
        Vector3(-1.0f, -2.0f,  0.0f)));

    triangles.push_back(Triangle(
        Vector3(-1.0f, -4.0f,  0.0f),
        Vector3( 2.0f,  1.0f,  0.0f),
        Vector3( 4.0f, -1.0f,  0.0f)));

    triangles.push_back(Triangle(
        Vector3( 3.0f,  2.0f,  0.0f),
        Vector3( 4.0f,  4.0f,  0.0f),
        Vector3( 4.0f,  1.0f,  0.0f)));

    triangles.push_back(Triangle(
        Vector3(-3.0f,  0.0f,  0.0f),
        Vector3(-1.0f, -2.0f,  0.0f),
        Vector3(-2.0f, -4.0f,  0.0f)));

    SplitList list;

    vol::AABB bounds(Vector3(-5.0f, -5.0f, -5.0f), Vector3(5.0f, 5.0f, 5.0f));

    foreach (const Triangle& triangle, triangles) 
        SplitPlane::createFromTriangle(triangle, bounds, std::inserter(list, list.begin()));

#if 0
    vol::AABB b1(Vector3(-2.0f, 1.0f, 0.0f), Vector3(4.0f, 2.0f, 0.0f));
    SplitPlane::createFromBounds(b1, bounds, std::inserter(list, list.begin()));

    vol::AABB b2(Vector3(-3.0f, -3.0f, 0.0f), Vector3(-1.0f, -1.0f, 0.0f));
    //SplitPlane::createFromBounds(b2, bounds, std::inserter(list, list.begin()));

    vol::AABB b3(Vector3(-3.0f, 2.0f, 0.0f), Vector3(-1.0f, 4.0f, 0.0f));
    SplitPlane::createFromBounds(b3, bounds, std::inserter(list, list.begin()));

    vol::AABB b4(Vector3(1.0f, -4.0f, 0.0f), Vector3(2.0f, 0.0f, 0.0f));
    //SplitPlane::createFromBounds(b4, bounds, std::inserter(list, list.begin()));

    vol::AABB b5(Vector3(3.0f, -3.0f, 0.0f), Vector3(4.0f, 0.0f, 0.0f));
    //SplitPlane::createFromBounds(b5, bounds, std::inserter(list, list.begin()));

    vol::AABB b6(Vector3(-3.0f, -2.0f, 0.0f), Vector3(2.0f, 3.0f, 0.0f));
    //SplitPlane::createFromBounds(b6, bounds, std::inserter(list, list.begin()));

    vol::AABB b7(Vector3(-1.0f, -4.0f, 0.0f), Vector3(4.0f, 1.0f, 0.0f));
    //SplitPlane::createFromBounds(b7, bounds, std::inserter(list, list.begin()));
#endif

    //std::stable_sort(list.begin(), list.end(), LessThanPtrs());
    list.sort(LessThanPtrs());

    std::for_each(list.begin(), list.end(), UpdateCosts(4));

    SpatialCanvas canvas(bounds, 50, SpatialCanvas::Z_AXIS);

    // TODO: Not leak memory :)
    Node* root = createNode(list, 1, 100.0f, canvas, bounds);

    canvas.getBitmap().saveFile("kdtree.bmp");
}

Triangle::Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2) :
    _v0(v0), _v1(v1), _v2(v2)
{

}

vol::AABB Triangle::determineBounds() const
{
    Vector3 min = _v0;
    Vector3 max = _v0;

    min.x = std::min(min.x, _v1.x);
    min.y = std::min(min.y, _v1.y);
    min.z = std::min(min.z, _v1.z);
    max.x = std::max(max.x, _v1.x);
    max.y = std::max(max.y, _v1.y);
    max.z = std::max(max.z, _v1.z);

    min.x = std::min(min.x, _v2.x);
    min.y = std::min(min.y, _v2.y);
    min.z = std::min(min.z, _v2.z);
    max.x = std::max(max.x, _v2.x);
    max.y = std::max(max.y, _v2.y);
    max.z = std::max(max.z, _v2.z);

    return vol::AABB(min, max);
}

const Vector3& Triangle::getVertex(int index) const
{
    switch (index) {
        case 0: return _v0;
        case 1: return _v1;
        case 2: return _v2;
        default: assert(false);
    }
}

const Vector3& Triangle::getV0() const
{
    return _v0;
}

const Vector3& Triangle::getV1() const
{
    return _v1;
}

const Vector3& Triangle::getV2() const
{
    return _v2;
}

Node::Node() :
    _left(0), _right(0), _triangles(new Triangles)
{

}

Node::Node(Node* left, Node* right) :
    _left(left), _right(right), _triangles(0)
{

}

Node::~Node()
{
    delete _left;
    delete _right;
    delete _triangles;
}

bool Node::isLeaf() const
{
    return (_triangles != 0);
}

const Node& Node::getLeft() const
{
    assert(!isLeaf());

    return *_left;
}

const Node& Node::getRight() const
{
    assert(!isLeaf());

    return *_right;
}

const Node::Triangles& Node::getTriangles() const
{
    return *_triangles;
}

void Node::addTriangle(const Triangle* triangle)
{
    _triangles->push_back(triangle);
}



////////// SpatialCanvas //////////

SpatialCanvas::SpatialCanvas(const vol::AABB& bounds, int scale, Axis plane) :
    _bounds(bounds), _scale(scale), _plane(plane), _bitmap(1, 1)
{
    switch (_plane) {
        case X_AXIS:
            _bitmap.resize(
                int(_bounds.getLengthY()) * _scale + 1,
                int(_bounds.getLengthZ()) * _scale + 1);
            break;
        case Y_AXIS:
            _bitmap.resize(
                int(_bounds.getLengthX()) * _scale + 1,
                int(_bounds.getLengthZ()) * _scale + 1);
            break;
        case Z_AXIS:
            _bitmap.resize(
                int(_bounds.getLengthX()) * _scale + 1,
                int(_bounds.getLengthY()) * _scale + 1);
            break;
    }

    _bitmap.fill(bmp::Bitmap::Colour(255, 255, 255, 0));
}

void SpatialCanvas::drawAABB(const vol::AABB& aabb, bmp::Bitmap::Colour colour)
{
    Point2D min = convertCoords(aabb.getMin());
    Point2D max = convertCoords(aabb.getMax());

    bmp::drawLine(_bitmap, colour, min.x, max.y, max.x, max.y);
    bmp::drawLine(_bitmap, colour, max.x, max.y, max.x, min.y);
    bmp::drawLine(_bitmap, colour, max.x, min.y, min.x, min.y);
    bmp::drawLine(_bitmap, colour, min.x, min.y, min.x, max.y);
}

void SpatialCanvas::drawTriangle(const Triangle& triangle, bmp::Bitmap::Colour colour)
{
    Point2D v0 = convertCoords(triangle.getV0());
    Point2D v1 = convertCoords(triangle.getV1());
    Point2D v2 = convertCoords(triangle.getV2());

    bmp::drawLine(_bitmap, colour, v0.x, v0.y, v1.x, v1.y);
    bmp::drawLine(_bitmap, colour, v1.x, v1.y, v2.x, v2.y);
    bmp::drawLine(_bitmap, colour, v2.x, v2.y, v0.x, v0.y);
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
            point.x = int(coords.y) * _scale;
            point.y = int(coords.z) * _scale;
            break;
        case Y_AXIS:
            point.x = int(coords.x) * _scale;
            point.y = int(coords.z) * _scale;
            break;
        case Z_AXIS:
            point.x = int(coords.x) * _scale;
            point.y = int(coords.y) * _scale;
            break;
    }

    point.x = _bitmap.getWidth() / 2 + point.x;
    point.y = _bitmap.getHeight() / 2 - point.y;

    return point;
}










