#pragma once
#include "vector"
#include "RadeonRays/split_bvh.h"


class Mesh
{
public:
    Mesh()
    {
        bvh = new RadeonRays::SplitBvh(2.0f, 64, 0, 0.001f, 0);
    }
    ~Mesh()
    {
        delete bvh;
    }

    void BuildBVH();
    bool LoadFromFile(const std::string& filename);
public:

    std::vector<Vec4> verticesUVX; // VertexPosition + texture Coord (u/s)
    std::vector<Vec4> normalsUVY; // Normal + texture Coord (v/t)

    RadeonRays::Bvh* bvh;
    std::string name;
};

class MeshInstance
{
public:
    MeshInstance(std::string name, int meshId, Mat4 xform, int matId)
        : name(name)
        , meshID(meshId)
        , transform(xform)
        , materialID(matId)
    {
    }

    ~MeshInstance() {}
public:
    Mat4 transform;
    std::string name;

    int materialID;
    int meshID;
};