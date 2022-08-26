#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "concurrency.hpp"
#include <core/core.hpp>
#include "settings.hpp"
#include "scriptmodule.hpp"
#include "postoffice.hpp"
#include "network.hpp"
#include "player.hpp"
#include "idle.hpp"
#include "zone.hpp"
#include <math/prim.hpp>
#include "canvas.hpp"
#include <physics/kdtree.hpp>
#include <math/volumes.hpp>


using namespace std;


////////// KDTree Test Code //////////

float randFloat(float min, float max)
{
    float val = (float(rand()) * (max - min) / float(RAND_MAX) + min);
    assert(val >= min);
    assert(val <= max);
    return val;
}

void makeRandomTriangles(std::vector<Triangle>& vec, size_t count, const vol::AABB& bounds)
{
    srand(time(0));

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

struct DrawTriangles {
    DrawTriangles(SpatialCanvas& canvas) : _canvas(canvas) {}
    void operator()(const Triangle& triangle) {
        _canvas.drawTriangle(triangle,
            SpatialCanvas::Colour(255, 0, 0, 0),
            SpatialCanvas::Colour(0, 255, 0, 0),
            SpatialCanvas::Colour(0, 0, 255, 0));
    }
    SpatialCanvas& _canvas;
};

struct DoNothing { void operator()(const Triangle&) const {}; };

void checkIntersection(KDTree::Ptr& tree, const vol::AABB& bounds)
{
    float x = 3435.0f, y = 3145.0f, sz = 5.0f;
    vol::AABB intersection(
        Vector3(x - 2100.0f - sz, 2100.0f - y - sz, -sz),
        Vector3(x - 2100.0f + sz, 2100.0f - y + sz, sz));

    SpatialCanvas canvasX(bounds, 1, SpatialCanvas::X_AXIS);
    SpatialCanvas canvasY(bounds, 1, SpatialCanvas::Y_AXIS);
    SpatialCanvas canvasZ(bounds, 1, SpatialCanvas::Z_AXIS);

    DrawTriangles drawTrianglesX(canvasX);
    DrawTriangles drawTrianglesY(canvasY);
    DrawTriangles drawTrianglesZ(canvasZ);

    Timer timer;
    DoNothing doNothing;
    timer.reset();
    for (int i = 0; i < 210000; i++)
        tree->process(doNothing, intersection);
    uint64_t elapsed = timer.elapsed();
    cout << "elapsed = " << elapsed << "us" << endl;

    tree->process(drawTrianglesX, intersection);
    tree->process(drawTrianglesY, intersection);
    tree->process(drawTrianglesZ, intersection);

    canvasX.getBitmap().saveFile("intersection_x.bmp");
    canvasY.getBitmap().saveFile("intersection_y.bmp");
    canvasZ.getBitmap().saveFile("intersection_z.bmp");
}

struct DisplayKDTrees {
    DisplayKDTrees(const char* filename, const vol::AABB& bounds) :
        _canvasX(bounds, 1, SpatialCanvas::X_AXIS),
        _canvasY(bounds, 1, SpatialCanvas::Y_AXIS),
        _canvasZ(bounds, 1, SpatialCanvas::Z_AXIS),
        _filename(filename)
    {
        _splits.push(bounds);
    }
    ~DisplayKDTrees() {
        _canvasX.getBitmap().saveFile((_filename + "_X.bmp").c_str());
        _canvasY.getBitmap().saveFile((_filename + "_Y.bmp").c_str());
        _canvasZ.getBitmap().saveFile((_filename + "_Z.bmp").c_str());
    }
    void display(const KDTree& tree) {
        tree.walkEntireTree(*this);
    }
    void triangle(const Triangle& triangle) {
        _canvasX.drawTriangle(triangle,
            SpatialCanvas::Colour(255, 0, 0, 0),
            SpatialCanvas::Colour(0, 255, 0, 0),
            SpatialCanvas::Colour(0, 0, 255, 0));
        _canvasY.drawTriangle(triangle,
            SpatialCanvas::Colour(255, 0, 0, 0),
            SpatialCanvas::Colour(0, 255, 0, 0),
            SpatialCanvas::Colour(0, 0, 255, 0));
        _canvasZ.drawTriangle(triangle,
            SpatialCanvas::Colour(255, 0, 0, 0),
            SpatialCanvas::Colour(0, 255, 0, 0),
            SpatialCanvas::Colour(0, 0, 255, 0));
    }
    void left(SplitAxis axis, float position) {
        const Vector3& min = _splits.top().getMin();
        Vector3 max = _splits.top().getMax();
        switch (axis) {
            case SPLIT_AXIS_X: max.x = position; break;
            case SPLIT_AXIS_Y: max.y = position; break;
            case SPLIT_AXIS_Z: max.z = position; break;
            case SPLIT_LEAF: break;
        }
        _splits.push(vol::AABB(min, max));
        _canvasX.drawAABB(_splits.top(), SpatialCanvas::Colour(0, 0, 0, 0));
        _canvasY.drawAABB(_splits.top(), SpatialCanvas::Colour(0, 0, 0, 0));
        _canvasZ.drawAABB(_splits.top(), SpatialCanvas::Colour(0, 0, 0, 0));
    }
    void right(SplitAxis axis, float position) {
        Vector3 min = _splits.top().getMin();
        const Vector3& max = _splits.top().getMax();
        switch (axis) {
            case SPLIT_AXIS_X: min.x = position; break;
            case SPLIT_AXIS_Y: min.y = position; break;
            case SPLIT_AXIS_Z: min.z = position; break;
            case SPLIT_LEAF: break;
        }
        _splits.push(vol::AABB(min, max));
        _canvasX.drawAABB(_splits.top(), SpatialCanvas::Colour(0, 0, 0, 0));
        _canvasY.drawAABB(_splits.top(), SpatialCanvas::Colour(0, 0, 0, 0));
        _canvasZ.drawAABB(_splits.top(), SpatialCanvas::Colour(0, 0, 0, 0));
    }
    void up() {
        _splits.pop();
    }

    std::stack<vol::AABB> _splits;
    std::string _filename;

    SpatialCanvas _canvasX;
    SpatialCanvas _canvasY;
    SpatialCanvas _canvasZ;
};

void createKDTree()
{
    std::vector<Triangle> triangles;
    makeTestTriangles(triangles);

    vol::AABB bounds(Vector3(-5.0f, -5.0f, -5.0f), Vector3(5.0f, 5.0f, 5.0f));

    KDTree::Ptr tree(KDTree::create(triangles, bounds));
    tree->save("kdtree.dat");
}

void displayKDTree()
{
    vol::AABB bounds(Vector3(-2100.0f, -2100.0f, -2100.0f), Vector3(2100.0f, 2100.0f, 2100.0f));
    KDTree::Ptr tree(KDTree::load("/tmp/collision.dat"));
    DisplayKDTrees("collision_kdtree", bounds).display(*tree);
    checkIntersection(tree, bounds);
}

