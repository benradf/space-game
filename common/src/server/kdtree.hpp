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
        vol::AABB determineBounds() const;

    private:
        Vector3 _verts[3];
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

