#include <iostream>
#include <physics/kdtree.hpp>
#include <expat.h>
#include <string.h>
#include <core/exception.hpp>
#include <core/core.hpp>
#include <fstream>


using namespace std;


class BuildCollisionKDTree {
    public:
        BuildCollisionKDTree(const char* filename);

        KDTree::Ptr createKDTree() const;

    private:
        BuildCollisionKDTree(const BuildCollisionKDTree&);
        BuildCollisionKDTree& operator=(const BuildCollisionKDTree&);

        static void beginElement(void* thisPtr, const char* name, const char** attrib);
        static void endElement(void* thisPtr, const char* name);

        void newTriangle(const char** attrib);
        void endTriangle();

        void newVertex(const char** attrib);
        void endVertex();

        std::vector<Vector3> _vertices;
        std::vector<Triangle> _triangles;

        std::string _xmlFilename;
        XML_Parser _parser;
};

BuildCollisionKDTree::BuildCollisionKDTree(const char* filename) :
    _xmlFilename(filename), _parser(0)
{
    _parser = XML_ParserCreate(0);
    XML_SetUserData(_parser, this);
    XML_SetElementHandler(_parser, beginElement, endElement);

    std::ifstream file(filename);
    if (!file.is_open()) 
        throw FileException(std::string("could not open `") + filename + "'");

    file.seekg(0, std::ios_base::end);
    int size = file.tellg();
    file.seekg(0, std::ios_base::beg);
    size -= file.tellg();

    char* xmlText = new char[size];
    xmlText[size-1] = 0;

    try {
        file.read(xmlText, size);
        XML_Parse(_parser, xmlText, size, true);

    } catch (...) {
        delete[] xmlText;
        XML_ParserFree(_parser);
        _parser = 0;

        throw;
    }

    delete[] xmlText;
    XML_ParserFree(_parser);
    _parser = 0;
}

KDTree::Ptr BuildCollisionKDTree::createKDTree() const
{
    float min = 0.0f, max = 0.0f;

    for (const auto& triangle : _triangles) {
        min = std::min(min, triangle.getV0().x);
        max = std::max(max, triangle.getV0().x);
        min = std::min(min, triangle.getV1().y);
        max = std::max(max, triangle.getV1().y);
        min = std::min(min, triangle.getV2().z);
        max = std::max(max, triangle.getV2().z);
    }

    KDTree::Ptr ptr(KDTree::create(_triangles, vol::AABB(Vector3(min, min, min), Vector3(max, max, max))));

    cout << "kdtree stats:" << endl;
    cout << "  memory usage = " << (ptr->getMemoryUsage() / 1024) << "kb" << endl;
    cout << "  triangle count = " << _triangles.size() << endl;
    cout << "  min bound = (" << min << ", " << min << ", " << min << ")" << endl;
    cout << "  max bound = (" << max << ", " << max << ", " << max << ")" << endl;

    return ptr;

}

BuildCollisionKDTree::BuildCollisionKDTree(const BuildCollisionKDTree&)
{

}

BuildCollisionKDTree& BuildCollisionKDTree::operator=(const BuildCollisionKDTree&)
{
    return *this;
}

void BuildCollisionKDTree::beginElement(void* thisPtr, const char* name, const char** attrib)
{
    if (strcmp(name, "triangle") == 0) {
        reinterpret_cast<BuildCollisionKDTree*>(thisPtr)->newTriangle(attrib);
    } else if (strcmp(name, "vertex") == 0) {
        reinterpret_cast<BuildCollisionKDTree*>(thisPtr)->newVertex(attrib);
    }
}

void BuildCollisionKDTree::endElement(void* thisPtr, const char* name)
{
    if (strcmp(name, "triangle") == 0) {
        reinterpret_cast<BuildCollisionKDTree*>(thisPtr)->endTriangle();
    } else if (strcmp(name, "vertex") == 0) {
        reinterpret_cast<BuildCollisionKDTree*>(thisPtr)->endVertex();
    }
}

void BuildCollisionKDTree::newTriangle(const char** attrib)
{
    _vertices.clear();
}

void BuildCollisionKDTree::endTriangle()
{
    if (_vertices.size() != 3) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s:%d: error: %s", 
            _xmlFilename.c_str(), int(XML_GetCurrentLineNumber(_parser)), 
            "a triangle must have 3 vertices");

        throw Exception(buffer);
    }

    _triangles.push_back(Triangle(_vertices[0], _vertices[1], _vertices[2]));
}

void BuildCollisionKDTree::newVertex(const char** attrib)
{
    Vector3 vertex = Vector3::ZERO;

    while (*attrib != 0) {
        const char* name = *attrib++;
        float value = atof(*attrib++);

        if (strncmp(name, "x", 2) == 0) {
            vertex.x = value;
        } else if (strncmp(name, "y", 2) == 0) {
            vertex.y = value;
        } else if (strncmp(name, "z", 2) == 0) {
            vertex.z = value;
        }
    }

    _vertices.push_back(vertex);
}

void BuildCollisionKDTree::endVertex()
{

}



int main(int argc, char* argv[])
{
    if (argc != 3) {
        cerr << "usage: zonebuild INFILE OUTFILE" << endl;
        return 1;
    }

    const char* srcFile = argv[1];
    const char* dstFile = argv[2];

    try {
        BuildCollisionKDTree(srcFile).createKDTree()->save(dstFile);
    } catch (std::exception& e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}

