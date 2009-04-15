/// \file kdtree.hpp
/// \brief Provides a generic kdtree data structure.
/// \author Ben Radford 
/// \date 7th April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#ifndef KDTREE_HPP
#define KDTREE_HPP


#include <vecmath.hpp>
#include "volumes.hpp"


class Triangle {
    public:
        Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2);

        vol::AABB determineBounds() const;

    //private:
        Vector3 _v0;
        Vector3 _v1;
        Vector3 _v2;
};

struct KDTreeNode {
    unsigned children : 30;
    unsigned splitAxis : 2;
    float splitPosition;
};

class KDTree {
    public:

    private:
};


#endif  // KDTREE_HPP

