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


class TemporaryNode {
    public:
        typedef std::auto_ptr<TemporaryNode> Ptr;
        typedef std::vector<const Triangle*> Triangles;

        TemporaryNode();
        TemporaryNode(Ptr left, Ptr right, SplitAxis axis, float position);
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
        Ptr _left;
        Ptr _right;

        SplitAxis _axis;
        float _position;

        Triangles* _triangles;
};


class SplitPlane {
    public:
        enum Side { LEFT, BOTH, RIGHT };

        SplitPlane(SplitAxis axis, float pos, const vol::AABB& global, bool minBound);

        static SplitInsertIter createFromTriangle(const Triangle& triangle, const vol::AABB& global, SplitInsertIter iter);
        static SplitInsertIter createFromBounds(const vol::AABB& local, const vol::AABB& global, SplitInsertIter iter);
        static SplitInsertIter duplicatePlanes(const SplitPlane& plane, SplitInsertIter iter);

        bool operator<(const SplitPlane& that) const;
        Side whichSide(const SplitPlane& that) const;

        const Triangle* getTriangle() const;
        void recalculateCosts();
        bool isFlat() const;

        SplitAxis getAxis() const;
        void setAxis(SplitAxis axis);

        float getPosition() const;
        void setPosition(float position);

        bool getMinBound() const;
        void setMinBound(bool minBound);

        float getVolumeL() const;
        void setVolumeL(float volumeL);

        float getVolumeR() const;
        void setVolumeR(float volumeR);

        int getCountL() const;
        void setCountL(int countL);

        int getCountR() const;
        void setCountR(int countR);

        float getCostL() const;
        float getCostR() const;
        float getCost() const;

    private:
        const SplitPlane* _others[6];

        SplitAxis _axis;
        float _position;
        bool _minBound;
        const Triangle* _tri;

        float _volumeL;
        float _volumeR;
        int _countL;
        int _countR;

        float _costL;
        float _costR;
        float _cost;
};


class UpdateBase {
    public:
        UpdateBase(int count, float volumeL, float volumeR, SplitAxis axis, float pos);
        ~UpdateBase();

        void setCount(SplitPlane* plane, int& countL, int& countR);

        virtual void maybeClampPosition(SplitPlane* plane) = 0;
        virtual void maybeUpdateVolume(SplitPlane* plane) = 0;

        void handlePending(bool moveZeroDist);
        void operator()(SplitPlane* plane);

    protected:
        int& getCountL(SplitPlane* plane);
        int& getCountR(SplitPlane* plane);

        float _volumeL;
        float _volumeR;
        int _countL[3];
        int _countR[3];
        SplitAxis _axis;
        float _position;

        std::vector<SplitPlane*> countPending;
        float prevPos;
        SplitAxis prevAxis;
};


struct UpdateLeft : public UpdateBase {
    UpdateLeft(const SplitPlane& split);
    UpdateLeft(int count, float volumeL, float volumeR, SplitAxis axis, float pos);
    virtual void maybeClampPosition(SplitPlane* plane);
    virtual void maybeUpdateVolume(SplitPlane* plane);
};


struct UpdateRight : public UpdateBase {
    UpdateRight(const SplitPlane& split);
    UpdateRight(int count, float volumeL, float volumeR, SplitAxis axis, float pos);
    virtual void maybeClampPosition(SplitPlane* plane);
    virtual void maybeUpdateVolume(SplitPlane* plane);
};


struct UpdateInitial : public UpdateBase {
    UpdateInitial(int count);
    virtual void maybeClampPosition(SplitPlane* plane);
    virtual void maybeUpdateVolume(SplitPlane* plane);
};


struct DuplicateOverlapping {
    DuplicateOverlapping(SplitList& list, SplitIter iter);
    void operator()(const SplitPlane* plane);
    SplitInsertIter insertIter;
};


struct OnSide {
    OnSide(const SplitPlane& split_, SplitPlane::Side side_);
    bool operator()(const SplitPlane* plane) const;
    const SplitPlane& split;
    SplitPlane::Side side;
};


struct LessThanPtrs {
    bool operator()(const SplitPlane* a, const SplitPlane* b);
};


struct MinCost {
    SplitPlane* operator()(SplitPlane* min, SplitPlane* next) const;
};


struct IsFlat {
    bool operator()(const SplitPlane* plane) const;
};


////////// TemporaryNode //////////

TemporaryNode::TemporaryNode() :
    _left(0), _right(0), _triangles(new Triangles)
{

}

TemporaryNode::TemporaryNode(Ptr left, Ptr right, SplitAxis axis, float position) :
    _left(left), _right(right), _axis(axis), _position(position), _triangles(0)
{

}

TemporaryNode::~TemporaryNode()
{
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
    assert(_left.get() != 0);

    return *_left;
}

const TemporaryNode& TemporaryNode::getRight() const
{
    assert(!isLeaf());
    assert(_right.get() != 0);

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


////////// SplitPlane //////////

SplitPlane::SplitPlane(SplitAxis axis, float pos, const vol::AABB& global, bool minBound) :
    _axis(axis), _position(pos), _minBound(minBound), _tri(0), _volumeL(0.0f), _volumeR(0.0f)
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

    recalculateCosts();
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

    bool thatisFlat = (thatL._position == thatR._position);
    bool thisisFlat = (thisL._position == thisR._position);

    Side side = (this == &thisL ? LEFT : RIGHT);

    if (thatisFlat) {
        if (thatL._position > _position) 
            return RIGHT;

        if (thatR._position < _position) 
            return LEFT;

        if (thisisFlat) 
            return BOTH;

        return side;
    }
        
    if (thatL._position >= _position) 
        return RIGHT;

    if (thatR._position <= _position) 
        return LEFT;

    return BOTH;
}

void SplitPlane::recalculateCosts()
{
    _costL = getVolumeL() * float(getCountL());
    _costR = getVolumeR() * float(getCountR());
    _cost = _costL + _costR;

    if ((getVolumeL() < 0.0001f) || (getVolumeR() < 0.00001f)) 
        _cost = 100.0f;
}

const Triangle* SplitPlane::getTriangle() const
{
    return _tri;
}

bool SplitPlane::isFlat() const
{
    return (_others[2*_axis]->_position == _others[2*_axis+1]->_position);
}

inline SplitAxis SplitPlane::SplitPlane::getAxis() const
{
    return _axis;
}

inline void SplitPlane::setAxis(SplitAxis axis)
{
    assert(axis >= 0);
    assert(axis <= 2);

    _axis = axis;
}

inline float SplitPlane::getPosition() const
{
    return _position;
}

inline void SplitPlane::setPosition(float position)
{
    _position = position;
}

inline bool SplitPlane::getMinBound() const
{
    return _minBound;
}

inline void SplitPlane::setMinBound(bool minBound)
{
    _minBound = minBound;
}

inline float SplitPlane::getVolumeL() const
{
    return _volumeL;
}

inline void SplitPlane::setVolumeL(float volumeL)
{
    assert(volumeL >= 0.0f);

    _volumeL = volumeL;
}

inline float SplitPlane::getVolumeR() const
{
    return _volumeR;
}

inline void SplitPlane::setVolumeR(float volumeR)
{
    assert(volumeR >= 0.0f);

    _volumeR = volumeR;
}

inline int SplitPlane::getCountL() const
{
    return _countL;
}

inline void SplitPlane::setCountL(int countL)
{
    assert(countL >= 0);

    _countL = countL;
}

inline int SplitPlane::getCountR() const
{
    return _countR;
}

inline void SplitPlane::setCountR(int countR)
{
    assert(countR >= 0);

    _countR = countR;
}

inline float SplitPlane::getCostL() const
{
    return _costL;
}

inline float SplitPlane::getCostR() const
{
    return _costR;
}

inline float SplitPlane::getCost() const
{
    return _cost;
}


////////// UpdateBase //////////

UpdateBase::UpdateBase(int count, float volumeL, float volumeR, SplitAxis axis, float pos) :
    _volumeL(volumeL), _volumeR(volumeR), _axis(axis), _position(pos)
{
    for (int i = 0; i < 3; i++) 
        _countL[i] = 0, _countR[i] = count;
}

UpdateBase::~UpdateBase()
{
    if (countPending.empty()) 
        return;

    int peakCountR = _countR[countPending.back()->getAxis()];
    if (!countPending.back()->getMinBound() && (countPending.size() == 1))
        peakCountR--;

    foreach (SplitPlane* p, countPending)
        setCount(p, getCountL(p), peakCountR);
}

void UpdateBase::setCount(SplitPlane* plane, int& countL, int& countR)
{
    if (!plane->getMinBound()) 
        _countR[plane->getAxis()]--;

    plane->setCountL(countL);
    plane->setCountR(countR);
    plane->recalculateCosts();
}

void UpdateBase::handlePending(bool moveZeroDist)
{
    if (!countPending.empty()) {
        if (!moveZeroDist) {
            int peakCountR = _countR[countPending.back()->getAxis()];
            if (!countPending.back()->getMinBound() && (countPending.size() == 1))
                peakCountR--;

            typedef std::vector<SplitPlane*>::iterator Iter;
            Iter iterNonFlat = std::stable_partition(countPending.begin(), countPending.end(), IsFlat());

            for (Iter i = countPending.begin(); i != iterNonFlat; ++i)
                setCount(*i, getCountL(*i), peakCountR);

            for (Iter i = iterNonFlat; i != countPending.end(); ++i)
                setCount(*i, getCountL(*i), getCountR(*i));
        }

        if (countPending.back()->getMinBound())
            _countL[countPending.back()->getAxis()]++;

        if (!moveZeroDist)
            countPending.clear();
    }
}

void UpdateBase::operator()(SplitPlane* plane)
{
    maybeClampPosition(plane);
    maybeUpdateVolume(plane);

    bool samePos = (prevPos == plane->getPosition());
    bool sameAxis = (prevAxis == plane->getAxis());

    handlePending(samePos && sameAxis);

    prevPos = plane->getPosition();
    prevAxis = plane->getAxis();

    countPending.push_back(plane);
}

int& UpdateBase::getCountL(SplitPlane* plane)
{
    return _countL[plane->getAxis()];
}

int& UpdateBase::getCountR(SplitPlane* plane)
{
    return _countR[plane->getAxis()];
}


////////// UpdateLeft //////////

UpdateLeft::UpdateLeft(const SplitPlane& split) :
    UpdateBase(split.getCountL(), split.getVolumeL(), split.getVolumeR(), split.getAxis(), split.getPosition())
{

}

UpdateLeft::UpdateLeft(int count, float volumeL, float volumeR, SplitAxis axis, float pos) :
    UpdateBase(count, volumeL, volumeR, axis, pos)
{

}

void UpdateLeft::maybeClampPosition(SplitPlane* plane)
{
    if (!plane->getMinBound() && (plane->getPosition() > _position) && (_axis == plane->getAxis())) {
        plane->setVolumeL(plane->getVolumeL() + plane->getVolumeR() - _volumeR);
        plane->setVolumeR(_volumeR);
        plane->setPosition(_position);
    }
}

void UpdateLeft::maybeUpdateVolume(SplitPlane* plane)
{
    if (plane->getAxis() == _axis) {
        plane->setVolumeR(plane->getVolumeR() - _volumeR);
    } else {
        float s = _volumeL / (_volumeL + _volumeR);
        plane->setVolumeL(plane->getVolumeL() * s);
        plane->setVolumeR(plane->getVolumeR() * s);
    }
}


////////// UpdateRight //////////

UpdateRight::UpdateRight(const SplitPlane& split) :
    UpdateBase(split.getCountR(), split.getVolumeL(), split.getVolumeR(), split.getAxis(), split.getPosition())
{

}

UpdateRight::UpdateRight(int count, float volumeL, float volumeR, SplitAxis axis, float pos) :
    UpdateBase(count, volumeL, volumeR, axis, pos)
{

}

void UpdateRight::maybeClampPosition(SplitPlane* plane)
{
    if (plane->getMinBound() && (plane->getPosition() < _position) && (_axis == plane->getAxis())) {
        plane->setVolumeR(plane->getVolumeR() + plane->getVolumeL() - _volumeL);
        plane->setVolumeL(_volumeL);
        plane->setPosition(_position);
    }
}

void UpdateRight::maybeUpdateVolume(SplitPlane* plane)
{
    if (plane->getAxis() == _axis) {
        plane->setVolumeL(plane->getVolumeL() - _volumeL);
    } else {
        float s = _volumeR / (_volumeL + _volumeR);
        plane->setVolumeL(plane->getVolumeL() * s);
        plane->setVolumeR(plane->getVolumeR() * s);
    }
}


////////// UpdateInitial //////////

UpdateInitial::UpdateInitial(int count) :
    UpdateBase(count, 0.0f, 0.0f, SPLIT_LEAF, 0.0f)
{

}

void UpdateInitial::maybeClampPosition(SplitPlane* plane)
{

}

void UpdateInitial::maybeUpdateVolume(SplitPlane* plane)
{

}


////////// DuplicateOverlapping //////////

DuplicateOverlapping::DuplicateOverlapping(SplitList& list, SplitIter iter) :
    insertIter(inserter(list, iter))
{

}

void DuplicateOverlapping::operator()(const SplitPlane* plane)
{
    insertIter = SplitPlane::duplicatePlanes(*plane, insertIter);
};


////////// OnSide //////////

OnSide::OnSide(const SplitPlane& split_, SplitPlane::Side side_) :
    split(split_), side(side_)
{

}

bool OnSide::operator()(const SplitPlane* plane) const
{
    return (split.whichSide(*plane) == side);
}


////////// LessThanPtrs //////////

bool LessThanPtrs::operator()(const SplitPlane* a, const SplitPlane* b)
{
    float d = fabs(a->getPosition() - b->getPosition());
    return (d < 0.00001f ? a->getAxis() < b->getAxis() : a->getPosition() < b->getPosition());
}


////////// MinCost //////////

SplitPlane* MinCost::operator()(SplitPlane* min, SplitPlane* next) const
{
    return (next->getCost() < min->getCost() ? next : min);
}


////////// IsFlat //////////

bool IsFlat::operator()(const SplitPlane* plane) const
{
    return plane->isFlat();
}


////////// Construction Functions //////////

inline bool shouldStopSplitting(float splitCost, float currentCost, int depth)
{
    return ((splitCost + 0.0001f >= currentCost) || (depth > KDTree::MAX_DEPTH));
}

std::auto_ptr<TemporaryNode> createTemporaryNode(SplitList& list, int depth, float cost)
{
    if (list.empty())
        return std::auto_ptr<TemporaryNode>(new TemporaryNode);

    SplitPlane& split = *std::accumulate(list.begin(), list.end(), *list.begin(), MinCost());

    if (shouldStopSplitting(split.getCost(), cost, depth)) {
        std::auto_ptr<TemporaryNode> node(new TemporaryNode);

        foreach (const SplitPlane* plane, list) {
            if ((plane->getAxis() == SPLIT_AXIS_X) && (!plane->getMinBound())) 
                node->addTriangle(plane->getTriangle());
        }

        return node;
    }

    UpdateLeft updaterL(split);
    UpdateRight updaterR(split);

    float costL = split.getCostL();
    float costR = split.getCostR();
    float splitPos = split.getPosition();
    SplitAxis splitAxis = split.getAxis();

    SplitIter beginBoth = std::stable_partition(list.begin(), list.end(), OnSide(split, SplitPlane::LEFT));
    SplitIter endBoth = std::stable_partition(beginBoth, list.end(), OnSide(split, SplitPlane::BOTH));
    std::for_each(beginBoth, endBoth, DuplicateOverlapping(list, beginBoth));

    SplitList listL;
    listL.splice(listL.begin(), list, list.begin(), beginBoth);
    listL.sort(LessThanPtrs());

    SplitList listR;
    listR.splice(listR.begin(), list, beginBoth, list.end());
    listR.sort(LessThanPtrs());

    std::for_each(listL.begin(), listL.end(), updaterL);
    std::for_each(listR.begin(), listR.end(), updaterR);

    return std::auto_ptr<TemporaryNode>(new TemporaryNode(
        createTemporaryNode(listL, depth + 1, costL),
        createTemporaryNode(listR, depth + 1, costR),
        splitAxis, splitPos));
}

std::auto_ptr<TemporaryNode> constructKDTree(const std::vector<Triangle>& triangles, const vol::AABB& bounds)
{
    SplitList list;
    foreach (const Triangle& triangle, triangles) 
        SplitPlane::createFromTriangle(triangle, bounds, std::inserter(list, list.begin()));

    list.sort(LessThanPtrs());

    std::for_each(list.begin(), list.end(), UpdateInitial(triangles.size()));

    return createTemporaryNode(list, 1, 100.0f);
}

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

#pragma pack(push, 1)

struct KDTreeFileHeader {
    uint16_t magic;
    uint32_t fileSize;
    uint32_t nodeCount;
    uint32_t triangleCount;
    uint32_t nodeOffset;
    uint32_t triangleOffset;
};

#pragma pack(pop)

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


////////// KDTree //////////

KDTree::KDTree(std::auto_ptr<KDTreeData> data) :
    _data(data)
{

}

void KDTree::save(const char* filename) const
{
    _data->saveFile(filename);
}

KDTree::Ptr KDTree::create(const Triangles& triangles, const vol::AABB& bounds)
{
    std::auto_ptr<TemporaryNode> root(constructKDTree(triangles, bounds));
    std::auto_ptr<KDTreeData> data(compressKDTree(*root));
    return std::auto_ptr<KDTree>(new KDTree(data));
}

KDTree::Ptr KDTree::load(const char* filename)
{
    std::auto_ptr<KDTreeData> data(new KDTreeData(filename));
    return std::auto_ptr<KDTree>(new KDTree(data));
}

