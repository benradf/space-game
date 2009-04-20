/// \file kdtree.cpp
/// \brief Provides a generic kdtree data structure.
/// \author Ben Radford 
/// \date 7th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "kdtree.hpp"
#include <core.hpp>
#include <assert.h>
#include <stdlib.h>
#include <algorithm>
#include <numeric>
#include <memory>
#include <limits>
#include <vector>
#include <queue>
#include <list>
#include <set>
#include <tr1/unordered_set>


#include <iostream>
using namespace std;

typedef std::list<class SplitPlane*> SplitList;
typedef SplitList::iterator SplitIter;
typedef std::insert_iterator<SplitList> SplitInsertIter;

class SplitPlane {
    public:
        enum Side { LEFT, BOTH, RIGHT };

        SplitPlane(SplitAxis axis, float pos, const vol::AABB& global, bool minBound);

        bool operator<(const SplitPlane& that) const;
        Side whichSide(const SplitPlane& that) const;

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

bool SplitPlane::operator<(const SplitPlane& that) const
{
    return (_position < that._position);
}

SplitPlane::Side SplitPlane::whichSide(const SplitPlane& that) const
{
    const SplitPlane& thisL = *_others[2*_axis];
    const SplitPlane& thisR = *_others[2*_axis+1];

    const SplitPlane& thatL = *that._others[2*_axis];
    const SplitPlane& thatR = *that._others[2*_axis+1];

    bool thatIsFlat = (thatL._position == thatR._position);
    bool thisIsFlat = (thisL._position == thisR._position);

    Side side = (this == &thisL ? LEFT : RIGHT);

    if (thatIsFlat) {
        if (thatL._position > _position) 
            return RIGHT;

        if (thatR._position < _position) 
            return LEFT;

        if (thisIsFlat) 
            return BOTH;

        return side;
    }
        
    if (thatL._position >= _position) 
        return RIGHT;

    if (thatR._position <= _position) 
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

    //cout << "duplicatePlanes of " << &plane << endl;

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

        //cout << plane._others[i] << " -> " << planes[i] << endl;
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
                    //cout << "clamping max " << plane << endl;
                    plane->_volumeL -= (volumeR - plane->_volumeR);
                    plane->_volumeR = volumeR;
                }
                //countR[plane->_axis]--;
            } else {
                if (clampMin && (plane->_position < clampPos) && (axis == plane->_axis)) {
                    plane->_position = clampPos;
                    //cout << "clamping min " << plane << endl;
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
                //cout << "countPending.back() = " << countPending.back() << endl;
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
        //cout << "duplicate " << plane << endl;
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

TemporaryNode::TemporaryNode() :
    _left(0), _right(0), _triangles(new Triangles)
{

}

TemporaryNode::TemporaryNode(TemporaryNode* left, TemporaryNode* right, SplitAxis axis, float position) :
    _left(left), _right(right), _axis(axis), _position(position), _triangles(0)
{

}

TemporaryNode::~TemporaryNode()
{
    delete _left;
    delete _right;
    delete _triangles;
}

bool TemporaryNode::isLeaf() const
{
    return (_triangles != 0);
}

SplitAxis TemporaryNode::getAxis() const
{
    return _axis;
}

float TemporaryNode::getPosition() const
{
    return _position;
}

const TemporaryNode& TemporaryNode::getLeft() const
{
    assert(!isLeaf());

    return *_left;
}

const TemporaryNode& TemporaryNode::getRight() const
{
    assert(!isLeaf());

    return *_right;
}

const TemporaryNode::Triangles& TemporaryNode::getTriangles() const
{
    return *_triangles;
}

void TemporaryNode::addTriangle(const Triangle* triangle)
{
    _triangles->push_back(triangle);
}

size_t TemporaryNode::getDescendantCount() const
{
    if (!isLeaf()) 
        return (_left->getDescendantCount() + _right->getDescendantCount() + 2);

    return 0;
}

size_t TemporaryNode::getTriangleCount() const
{
    if (!isLeaf()) 
        return (_left->getTriangleCount() + _right->getTriangleCount());

    return _triangles->size();
}


////////// SpatialCanvas //////////

SpatialCanvas::SpatialCanvas(const vol::AABB& bounds, int scale, Axis plane) :
    _bounds(bounds), _scale(scale), _plane(plane), _bitmap(1, 1)
{
    switch (_plane) {
        case X_AXIS:
            _bitmap.resize(
                int(_bounds.getLengthY()) * scale + 1,
                int(_bounds.getLengthZ()) * scale + 1);
            break;
        case Y_AXIS:
            _bitmap.resize(
                int(_bounds.getLengthX()) * scale + 1,
                int(_bounds.getLengthZ()) * scale + 1);
            break;
        case Z_AXIS:
            _bitmap.resize(
                int(_bounds.getLengthX()) * scale + 1,
                int(_bounds.getLengthY()) * scale + 1);
            break;
    }

    _bitmap.fill(bmp::Bitmap::Colour(255, 255, 255, 0));
}

void SpatialCanvas::drawAABB(const vol::AABB& aabb, Colour colour)
{
    Point2D min = convertCoords(aabb.getMin());
    Point2D max = convertCoords(aabb.getMax());

    bmp::drawLine(_bitmap, colour, min.x, max.y, max.x, max.y);
    bmp::drawLine(_bitmap, colour, max.x, max.y, max.x, min.y);
    bmp::drawLine(_bitmap, colour, max.x, min.y, min.x, min.y);
    bmp::drawLine(_bitmap, colour, min.x, min.y, min.x, max.y);
}

void SpatialCanvas::drawTriangle(const Triangle& triangle, Colour colour)
{
    drawTriangle(triangle, colour, colour, colour);
}

void SpatialCanvas::drawTriangle(const Triangle& triangle, Colour c0, Colour c1, Colour c2)
{
    Point2D v0 = convertCoords(triangle.getV0());
    Point2D v1 = convertCoords(triangle.getV1());
    Point2D v2 = convertCoords(triangle.getV2());

    bmp::drawLine(_bitmap, c0, v0.x, v0.y, v1.x, v1.y);
    bmp::drawLine(_bitmap, c1, v1.x, v1.y, v2.x, v2.y);
    bmp::drawLine(_bitmap, c2, v2.x, v2.y, v0.x, v0.y);
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
            point.x = int(coords.y * _scale);
            point.y = int(coords.z * _scale);
            break;
        case Y_AXIS:
            point.x = int(coords.x * _scale);
            point.y = int(coords.z * _scale);
            break;
        case Z_AXIS:
            point.x = int(coords.x * _scale);
            point.y = int(coords.y * _scale);
            break;
    }

    point.x = _bitmap.getWidth() / 2 + point.x;
    point.y = _bitmap.getHeight() / 2 - point.y;

    return point;
}


////////// KDTreeData //////////

KDTreeData::KDTreeData(const char* filename) :
    _nodeCount(0), _nodes(0), _triangles(0), _triangleCount(0)
{
    loadFile(filename);
}

KDTreeData::KDTreeData(size_t nodeCount, size_t triangleCount) :
    _nodeCount(nodeCount), _nodes(0), _triangles(0), _triangleCount(triangleCount)
{
    if (nodeCount > MAX_NODES) 
        throw MemoryException("node count exceeds max");

    if (triangleCount > MAX_TRIANGLES) 
        throw MemoryException("triangle count exceeds max");

    _nodes = new KDTreeNode[nodeCount];

    try {
        _triangles = new Triangle[triangleCount];
    } catch (...) {
        delete[] _nodes;
        throw;
    }
}

KDTreeData::~KDTreeData()
{
    delete[] _nodes;
    delete[] _triangles;
}

KDTreeNode& KDTreeData::getNode(size_t index)
{
    assert(index < _nodeCount);

    return _nodes[index];
}

const KDTreeNode& KDTreeData::getNode(size_t index) const
{
    assert(index < _nodeCount);

    return _nodes[index];
}

Triangle& KDTreeData::getTriangle(size_t index)
{
    assert(index < _triangleCount);

    return _triangles[index];
}

const Triangle& KDTreeData::getTriangle(size_t index) const
{
    assert(index < _triangleCount);

    return _triangles[index];
}

size_t KDTreeData::getNodeCount() const
{
    return _nodeCount;
}

size_t KDTreeData::getTriangleCount() const
{
    return _triangleCount;
}

struct KDTreeFileHeader {
    uint16_t magic;
    uint32_t fileSize;
    uint32_t nodeCount;
    uint32_t triangleCount;
    uint32_t nodeOffset;
    uint32_t triangleOffset;
};

void KDTreeData::saveFile(const char* filename) const
{
    assert(checkValidity() == KDTreeData::VALID);

    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) 
        throw FileException(std::string("unable to save to ") + filename);

    size_t nodeSpace = sizeof(KDTreeNode) * _nodeCount;
    size_t triangleSpace = sizeof(Triangle) * _triangleCount;

    KDTreeFileHeader fileHeader;
    fileHeader.magic = 0x444b;
    fileHeader.fileSize = sizeof(fileHeader) + nodeSpace + triangleSpace;
    fileHeader.nodeCount = _nodeCount;
    fileHeader.triangleCount = _triangleCount;
    fileHeader.nodeOffset = sizeof(fileHeader);
    fileHeader.triangleOffset = fileHeader.nodeOffset + nodeSpace;

    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(_nodes), nodeSpace);
    file.write(reinterpret_cast<char*>(_triangles), triangleSpace);

    file.close();
}

void KDTreeData::loadFile(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) 
        throw FileException(std::string("unable to load from ") + filename);

    KDTreeFileHeader fileHeader;
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    KDTreeData newData(fileHeader.nodeCount, fileHeader.triangleCount);

    size_t nodeSpace = sizeof(KDTreeNode) * newData._nodeCount;
    size_t triangleSpace = sizeof(Triangle) * newData._triangleCount;

    file.read(reinterpret_cast<char*>(newData._nodes), nodeSpace);
    file.read(reinterpret_cast<char*>(newData._triangles), triangleSpace);

    if (newData.checkValidity() != KDTreeData::VALID) 
        throw FileException(std::string("invalid kdtree data in ") + filename);

    std::swap(_nodeCount, newData._nodeCount);
    std::swap(_nodes, newData._nodes);
    std::swap(_triangleCount, newData._triangleCount);
    std::swap(_triangles, newData._triangles);
}

KDTreeData::Validity KDTreeData::checkValidity() const
{
    size_t nodeCount = getNodeCount();
    size_t triangleCount = getTriangleCount();

    std::tr1::unordered_set<size_t> visited;

    for (size_t i = 0; i < nodeCount; i++) {
        visited.insert(i);
        const KDTreeNode& node = getNode(i);
        if (node.splitAxis == SPLIT_LEAF) {
            if (node.triangles + node.triangleCount > triangleCount) {
                assert(false);
                return INVALID_INDEX;
            }
        } else {
            if (node.left >= nodeCount) {
                assert(false);
                return INVALID_INDEX;
            }
            if (node.right >= nodeCount) {
                assert(false);
                return INVALID_INDEX;
            }
            if (visited.find(node.left) != visited.end())
                return INVALID_STRUCTURE;
            if (visited.find(node.right) != visited.end())
                return INVALID_STRUCTURE;
        }
    }

    if (visited.size() < nodeCount) 
        return UNREACHABLE_NODES;

    return VALID;
}

class KDTreeConstructor {
    public:
        KDTreeConstructor(const std::vector<Triangle>& triangles);

        std::auto_ptr<const TemporaryNode> getRoot() const;

    private:
};

std::auto_ptr<KDTreeData> compressKDTree(const TemporaryNode& root)
{
    size_t nodeCount = root.getDescendantCount() + 1;
    size_t triangleCount = root.getTriangleCount();

    std::auto_ptr<KDTreeData> data(new KDTreeData(nodeCount, triangleCount));

    size_t nextNode = 0;
    size_t nextChild = 1;
    size_t nextTriangle = 0;

    std::queue<const TemporaryNode*> edge;
    edge.push(&root);

    while (!edge.empty()) {
        const TemporaryNode& temporaryNode = *edge.front();
        KDTreeNode& compressedNode = data->getNode(nextNode++);

        if (temporaryNode.isLeaf()) {
            compressedNode.splitAxis = SPLIT_LEAF;
            compressedNode.triangles = nextTriangle;
            compressedNode.triangleCount = 0;

            foreach (const Triangle* triangle, temporaryNode.getTriangles()) {
                data->getTriangle(nextTriangle++) = *triangle;
                compressedNode.triangleCount++;
            }

        } else {
            compressedNode.splitAxis = temporaryNode.getAxis();
            compressedNode.splitPosition = temporaryNode.getPosition();
            compressedNode.left = nextChild++;
            compressedNode.right = nextChild++;

            edge.push(&temporaryNode.getLeft());
            edge.push(&temporaryNode.getRight());
        }

        edge.pop();
    }

    assert(nextNode == nodeCount);
    assert(nextChild == nodeCount);
    assert(nextTriangle == triangleCount);

    return data;
}

class KDTreeCompressor {
    public:
        KDTreeCompressor(const TemporaryNode& root);

        std::auto_ptr<const KDTreeData> getData() const;

    private:
        
};


////////// KDTree //////////

KDTree::KDTree(std::auto_ptr<KDTreeData> data) :
    _data(data)
{

}

































TemporaryNode* createNode(SplitList& list, int depth, float cost, SpatialCanvas* (&canvases)[3], const vol::AABB& bounds)
{
//    if (depth == 4) 
//        return;

    //cout << "create node (depth = " << depth << ", cost = " << cost << ")" << endl;

    foreach (SpatialCanvas* canvas, canvases) 
        canvas->drawAABB(bounds, bmp::Bitmap::Colour(0, 0, 0, 0));

    cerr << depth << " \033[01;32m(cost: " << cost << ")\033[00m\t";
    for (int i = 1; i < depth; i++) 
        cerr << "| ";

    if (list.empty()) {
        //cout << "    no splits to choose from" << endl;
        cerr << "\033[01;35msplit list empty\033[00m" << endl;
        return new TemporaryNode;
    }

    char axes[3] = { 'X', 'Y', 'Z' };

    //printSplits("ALL", list.begin(), list.end());

    SplitPlane& split = *std::accumulate(list.begin(), list.end(), *list.begin(), MinCost());
    //cout << "    choose split(" << &split << ") on axis " << axes[split._axis] << " at " << split._position << endl;

    if ((split._cost + 0.00001f >= cost) || (depth > 10)) {
        //cout << "cheapest to stop splitting here" << endl;
        cerr << "\033[01;33mTriangles: ";
        TemporaryNode* node = new TemporaryNode;
        foreach (const SplitPlane* plane, list) {
            if ((plane->_axis != SPLIT_AXIS_X) || (!plane->_minBound)) 
                continue;
            foreach (SpatialCanvas* canvas, canvases) {
                canvas->drawTriangle(*plane->_tri, 
                    bmp::Bitmap::Colour(255, 0, 0, 0), 
                    bmp::Bitmap::Colour(0, 255, 0, 0), 
                    bmp::Bitmap::Colour(0, 0, 255, 0));
            }
            node->addTriangle(plane->_tri);
            cerr << "[";
            const Vector3& v0 = plane->_tri->getV0();
            cerr << "(" << v0.x << ", " << v0.y << ", " << v0.z << "), ";
            const Vector3& v1 = plane->_tri->getV1();
            cerr << "(" << v1.x << ", " << v1.y << ", " << v1.z << "), ";
            const Vector3& v2 = plane->_tri->getV2();
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

    float splitPos = split._position;
    SplitAxis splitAxis = split._axis;

    float costL = split._costL;
    float costR = split._costR;
    SplitIter beginBoth = std::stable_partition(list.begin(), list.end(), OnSide(split, SplitPlane::LEFT));
    SplitIter endBoth = std::stable_partition(beginBoth, list.end(), OnSide(split, SplitPlane::BOTH));

    UpdateCosts updaterL = UpdateCosts::makeL(split);
    UpdateCosts updaterR = UpdateCosts::makeR(split);
    std::for_each(beginBoth, endBoth, DuplicateOverlapping(list, beginBoth));
    SplitList listL;
    listL.splice(listL.begin(), list, list.begin(), beginBoth);
    listL.sort(LessThanPtrs());

    SplitList listR;
    listR.splice(listR.begin(), list, beginBoth, list.end());
    listR.sort(LessThanPtrs());
    std::for_each(listL.begin(), listL.end(), updaterL);
    std::for_each(listR.begin(), listR.end(), updaterR);

    TemporaryNode* leftNode = createNode(listL, depth + 1, costL, canvases, vol::AABB(boundsMinL, boundsMaxL));
    TemporaryNode* rightNode = createNode(listR, depth + 1, costR, canvases, vol::AABB(boundsMinR, boundsMaxR));

    return new TemporaryNode(leftNode, rightNode, splitAxis, splitPos);
}

#include <iostream>
using namespace std;

float randFloat(float min, float max)
{
    float val = (float(rand()) * (max - min) / float(RAND_MAX) + min);
    assert(val >= min);
    assert(val <= max);
    return val;
}

void makeRandomTriangles(std::vector<Triangle>& vec, size_t count, const vol::AABB& bounds)
{
    //srand(time(0));
    srand(1);

    const Vector3& min = bounds.getMin();
    const Vector3& max = bounds.getMax();

    for (size_t i = 0; i < count; i++) {
        Vector3 v0(randFloat(min.x, max.x), randFloat(min.y, max.y), randFloat(min.z, max.z));
        Vector3 v1(randFloat(min.x, max.x), randFloat(min.y, max.y), randFloat(min.z, max.z));
        Vector3 v2(randFloat(min.x, max.x), randFloat(min.y, max.y), randFloat(min.z, max.z));
        vec.push_back(Triangle(v0, v1, v2));
    }
}

void makeTestTriangles(std::vector<Triangle>& vec)
{
    vec.push_back(Triangle(
        Vector3(-3.0f,  0.0f,  0.0f),
        Vector3( 2.0f,  3.0f,  1.0f),
        Vector3(-1.0f, -2.0f,  0.0f)));
    vec.push_back(Triangle(
        Vector3(-1.0f, -4.0f,  0.0f),
        Vector3( 2.0f,  1.0f,  0.0f),
        Vector3( 4.0f, -1.0f,  0.0f)));
    vec.push_back(Triangle(
        Vector3( 3.0f,  2.0f,  0.0f),
        Vector3( 4.0f,  4.0f,  0.0f),
        Vector3( 4.0f,  1.0f,  0.0f)));
    vec.push_back(Triangle(
        Vector3(-3.0f,  0.0f,  0.0f),
        Vector3(-1.0f, -2.0f,  0.0f),
        Vector3(-2.0f, -4.0f,  1.0f)));
}

void createKDTree()
{
    vol::AABB bounds(Vector3(-5.0f, -5.0f, -5.0f), Vector3(5.0f, 5.0f, 5.0f));

    std::vector<Triangle> triangles;
    //makeRandomTriangles(triangles, 50, bounds);
    makeTestTriangles(triangles);

    SplitList list;
    foreach (const Triangle& triangle, triangles) 
        SplitPlane::createFromTriangle(triangle, bounds, std::inserter(list, list.begin()));

    list.sort(LessThanPtrs());

    std::for_each(list.begin(), list.end(), UpdateCosts(triangles.size()));

    SpatialCanvas canvasX(bounds, 50, SpatialCanvas::X_AXIS);
    SpatialCanvas canvasY(bounds, 50, SpatialCanvas::Y_AXIS);
    SpatialCanvas canvasZ(bounds, 50, SpatialCanvas::Z_AXIS);

    SpatialCanvas* canvases[3] = { &canvasX, &canvasY, &canvasZ };

    // TODO: Not leak memory :)
    TemporaryNode* root = createNode(list, 1, 100.0f, canvases, bounds);

    canvasX.getBitmap().saveFile("kdtree_x.bmp");
    canvasY.getBitmap().saveFile("kdtree_y.bmp");
    canvasZ.getBitmap().saveFile("kdtree_z.bmp");

    std::auto_ptr<const KDTreeData> data(compressKDTree(*root));
    data->saveFile("kdtree.dat");

    KDTreeData tempData(1, 1);
    tempData.loadFile("kdtree.dat");
    tempData.saveFile("kdtree2.dat");

    delete root;
}

