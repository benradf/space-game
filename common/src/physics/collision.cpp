/// \file collision.cpp
/// \brief Provides collision detection routines.
/// \author Ben Radford 
/// \date 23rd April 2009
///
/// Copyright (c) 2009 Ben Radford. All rights reserved.
///


#include "collision.hpp"


using namespace vol;


struct CheckTriangles {
    CheckTriangles(const Sphere& s);
    void operator()(const Triangle& triangle);
    std::vector<const Triangle*> triangles;
    const Sphere& _sphere;
};


////////// CheckTriangles //////////

CheckTriangles::CheckTriangles(const Sphere& s) :
    _sphere(s)
{

}

void CheckTriangles::operator()(const Triangle& triangle)
{
    if (intersects3d(triangle, _sphere)) 
        triangles.push_back(&triangle);
}


////////// CollisionGeometry //////////

CollisionGeometry::CollisionGeometry(const char* filename) :
    _kdtree(KDTree::load(filename))
{

}

bool CollisionGeometry::checkCollision(const Sphere& sphere, Vector3& normal) const
{
    float radius = sphere.getRadius();
    Vector3 radiusVec(radius, radius, radius);

    Vector3 min = sphere.getCentre() - radiusVec;
    Vector3 max = sphere.getCentre() + radiusVec;

    CheckTriangles checkTriangles(sphere);
    _kdtree->process(checkTriangles, vol::AABB(min, max));

    return (!checkTriangles.triangles.empty());
}

