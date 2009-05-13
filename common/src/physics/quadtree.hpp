/// \file quadtree.hpp
/// \brief Provides a generic quadtree data structure.
/// \author Ben Radford 
/// \date 30th March 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef QUADTREE_HPP
#define QUADTREE_HPP


#include <math/volumes.hpp>
#include <core/core.hpp>
#include <assert.h>
#include <vector>
#include <algorithm>


template<typename T>
class QuadTree {
    public:
        QuadTree(const vol::AABB& bounds);
        ~QuadTree();

        void insert(T* object);
        void remove(T* object);
        void update(T* object);

        void balanceNodes();
        int getNodeCount() const;
        size_t getMemoryUsage() const;

        template<typename U>
        void process(U& visitor);
        template<typename U, typename V>
        void process(U& visitor, const V& volume);

    private:
        struct Node {
            Node(const vol::AABB& bounds_);
            Node(Node* parent, int index);
            ~Node();

            template<typename U>
            void process(U& visitor);
            template<typename U, typename V>
            void process(U& visitor, const V& volume);

            std::vector<T*> objects;
            Node* children[4];
            vol::AABB bounds;
            bool leaf;
            int level;
        };

        static const int MAX_LEVELS = 10;  // worst case 30MB
        static const int OBJS_PER_NODE = 10;

        QuadTree(const QuadTree&);
        QuadTree& operator=(const QuadTree&);

        void addObject(Node* node, T* object);
        void delObject(Node* node, T* object);

        Node* findByObj(Node* node, T* object);
        Node* findByPos(Node* node, const Vector3& pos);

        void maybeSplitNode(Node* node);

        Node* _root;
        int _nodeCount;
};


////////// QuadTree //////////

template<typename T>
QuadTree<T>::QuadTree(const vol::AABB& bounds) :
    _root(new Node(bounds)), _nodeCount(1)
{

}

template<typename T>
QuadTree<T>::~QuadTree()
{
    delete _root;
}

template<typename T>
void QuadTree<T>::insert(T* object)
{
    Node* node = findByPos(_root, object->getPosition());

    addObject(node, object);
}

template<typename T>
void QuadTree<T>::remove(T* object)
{
    Node* node = findByObj(_root, object);

    if (node == 0) 
        return;

    delObject(node, object);
}

template<typename T>
void QuadTree<T>::update(T* object)
{
    remove(object);
    insert(object);
}

template<typename T>
struct InsertNode {
    InsertNode(QuadTree<T>& quadTree) : _quadTree(quadTree) { }
    void visit(T* object) { _quadTree.insert(object); }
    QuadTree<T>& _quadTree;
};

template<typename T>
void QuadTree<T>::balanceNodes()
{
    QuadTree<T> newQuadTree(_root->bounds);
    InsertNode<T> insertFunc(newQuadTree);

    process(insertFunc);

    std::swap(_root, newQuadTree._root);
    std::swap(_nodeCount, newQuadTree._nodeCount);
}

template<typename T>
int QuadTree<T>::getNodeCount() const
{
    return _nodeCount;
}

template<typename T>
size_t QuadTree<T>::getMemoryUsage() const
{
    return (_nodeCount * sizeof(Node));
}

template<typename T>
template<typename U>
void QuadTree<T>::process(U& visitor)
{
    _root->process(visitor);
}

template<typename T>
template<typename U, typename V>
void QuadTree<T>::process(U& visitor, const V& volume)
{
    _root->process(visitor, volume);
}

template<typename T>
inline void QuadTree<T>::addObject(Node* node, T* object)
{
    assert(node->leaf);

    maybeSplitNode(node);
    node->objects.push_back(object);
}

template<typename T>
inline void QuadTree<T>::delObject(Node* node, T* object)
{
    assert(node->leaf);

    typename std::vector<T*>::iterator iter = std::find(
        node->objects.begin(), node->objects.end(), object);

    if (iter != node->objects.end()) 
        node->objects.erase(iter);
}

template<typename T>
inline typename QuadTree<T>::Node* QuadTree<T>::findByObj(Node* node, T* object)
{
    node = findByPos(node, object->getPosition());

    foreach (T* obj, node->objects) {
        if (obj == object) 
            return node;
    }

    return 0;
}

template<typename T>
inline typename QuadTree<T>::Node* QuadTree<T>::findByPos(Node* node, const Vector3& pos)
{
    while (!node->leaf) {
        const Vector3& mid = node->bounds.getMid();
        int x = (pos.x < mid.x ? 0 : 1);
        int y = (pos.y < mid.y ? 0 : 1);
        node = node->children[x|(y<<1)];
    }

    return node;
}

template<typename T>
inline void QuadTree<T>::maybeSplitNode(Node* node)
{
    assert(node->leaf);

    if (node->objects.size() < OBJS_PER_NODE) 
        return;

    if (node->level == MAX_LEVELS) 
        return;

    for (int i = 0; i < 4; i++) 
        node->children[i] = new Node(node, i);

    node->leaf = false;
    _nodeCount += 4;

    foreach (T* object, node->objects) 
        addObject(findByPos(node, object->getPosition()), object);

    std::vector<T*>().swap(node->objects);
}


////////// QuadTree::Node //////////

template<typename T>
inline QuadTree<T>::Node::Node(const vol::AABB& bounds_) :
    bounds(bounds_), leaf(true), level(1)
{
    
}

template<typename T>
inline QuadTree<T>::Node::Node(Node* parent, int index) :
    bounds(Vector3::ZERO, Vector3::ZERO), 
    leaf(true), level(parent->level + 1)
{
    objects.reserve(OBJS_PER_NODE);

    Vector3 min = parent->bounds.getMin();
    Vector3 max = parent->bounds.getMax();
    Vector3 mid = 0.5f * (min + max);

    min.x = ((index & 1) == 0 ? min.x : mid.x);
    min.y = ((index & 2) == 0 ? min.y : mid.y);
    max.x = ((index & 1) == 0 ? mid.x : max.x);
    max.y = ((index & 2) == 0 ? mid.y : max.y);

    bounds = vol::AABB(min, max);

    for (int i = 0; i < 4; i++) 
        children[i] = 0;
}

template<typename T>
inline QuadTree<T>::Node::~Node()
{
    if (leaf)
        return;

    for (int i = 0; i < 4; i++) 
        delete children[i];
}

template<typename T>
template<typename U>
inline void QuadTree<T>::Node::process(U& visitor)
{
    if (!leaf) {
        for (int i = 0; i < 4; i++) 
            children[i]->process(visitor);

        return;
    }

    foreach (T* object, objects)
        visitor.visit(object);
}

template<typename T>
template<typename U, typename V>
inline void QuadTree<T>::Node::process(U& visitor, const V& volume)
{
    if (!intersects2d(volume, bounds)) 
        return;

    if (!leaf) {
        for (int i = 0; i < 4; i++) 
            children[i]->process(visitor, volume);

        return;
    }

    foreach (T* object, objects) {
        vol::Point point(object->getPosition());
        if (intersects2d(volume, point)) 
            visitor.visit(object);
    }
}


#endif  // QUADTREE_HPP


