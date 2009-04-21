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

// temp testing of headers
#include <math/volumes.hpp>
#include "zone.hpp"

using namespace std;

static const int CYCLE_PERIOD = 100000;

void signalShutdown(int signum)
{
    Log::log->info("SIGINT caught: use CTRL+D to shutdown server cleanly");
}

void catchSignals(bool yes)
{
    if (yes) {
        signal(SIGINT, signalShutdown);
    } else {
        signal(SIGINT, SIG_DFL);
    }
}


void serverMain()
{
    // Catch signals.
    catchSignals(true);

    // Change working directory.
    if (chdir(getSettings().directory().c_str()) != 0)
        throw FileException("unable to change to specified working directory");

    // Create standard jobs.
    std::auto_ptr<Idle> jobIdle(new Idle(CYCLE_PERIOD));
    std::auto_ptr<PostOffice> jobPostOffice(new PostOffice);
    std::auto_ptr<NetworkInterface> jobNetwork(new NetworkInterface(*jobPostOffice));
    std::auto_ptr<LoginManager> jobLogin(new LoginManager(*jobPostOffice));
    std::auto_ptr<Zone> testZone(new Zone(*jobPostOffice));

    // Add to pool.
    JobPool pool;
    pool.add(Job::Ptr(jobIdle));
    pool.add(Job::Ptr(jobPostOffice));
    pool.add(Job::Ptr(jobNetwork));
    pool.add(Job::Ptr(jobLogin));
    pool.add(Job::Ptr(testZone));

    // Create worker threads.
    std::vector<boost::shared_ptr<Worker> > workers;
    for (int i = 0; i < getSettings().threadMax(); i++) {
        workers.push_back(boost::shared_ptr<Worker>(new Worker(pool)));
        Log::log->info("Worker thread created");
    }
    
    // Main thread waits.
    while (!feof(stdin))
        fgetc(stdin);

    // Stop catching signals.
    catchSignals(false);
}

extern void testKDTree();

int main(int argc, char* argv[])
{
    testKDTree();

    return 0;

    // Initialise logging.
    Log::Console consoleLog;
    Log::File fileLog("server.log");
    Log::Multi multiLog;
    multiLog.add(consoleLog);
    multiLog.add(fileLog);
    Log::log = &multiLog;

    // Read settings.
    Settings serverSettings(argc, argv);
    setSettings(serverSettings);
    
    Log::log->info("Server starts");
    Log::log->info("Built " __DATE__ " " __TIME__);
    
    try {
        serverMain();
    } catch (std::exception& e) {
        Log::log->error(e.what());
    }
    
    Log::log->info("Server stops");
    
    return 0;
}


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

void checkIntersection(KDTree::Ptr& tree, const vol::AABB& bounds)
{
    vol::AABB intersection(
        Vector3(2.5f, -1.0f, -5.0f),
        Vector3(4.5f, 3.0f, 5.0f));

    SpatialCanvas canvasX(bounds, 5, SpatialCanvas::X_AXIS);
    SpatialCanvas canvasY(bounds, 5, SpatialCanvas::Y_AXIS);
    SpatialCanvas canvasZ(bounds, 5, SpatialCanvas::Z_AXIS);

    DrawTriangles drawTrianglesX(canvasX);
    DrawTriangles drawTrianglesY(canvasY);
    DrawTriangles drawTrianglesZ(canvasZ);

    Timer timer;
    timer.reset();
    tree->process(drawTrianglesX, intersection);
    uint64_t elapsed = timer.elapsed();
    cout << "elapsed = " << elapsed << "us" << endl;
    tree->process(drawTrianglesY, bounds);
    tree->process(drawTrianglesZ, bounds);

    canvasX.getBitmap().saveFile("kdtree_x.bmp");
    canvasY.getBitmap().saveFile("kdtree_y.bmp");
    canvasZ.getBitmap().saveFile("kdtree_z.bmp");
}

void createKDTree()
{
    std::vector<Triangle> triangles;
    makeTestTriangles(triangles);

    vol::AABB bounds(Vector3(-5.0f, -5.0f, -5.0f), Vector3(5.0f, 5.0f, 5.0f));

    KDTree::Ptr tree(KDTree::create(triangles, bounds));
    tree->save("kdtree.dat");

    checkIntersection(tree, bounds);
}

void testKDTree()
{
    vol::AABB bounds(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f));
    KDTree::Ptr tree(KDTree::load("/tmp/collision.dat"));
    checkIntersection(tree, bounds);
}

