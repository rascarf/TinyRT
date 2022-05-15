#pragma once

#include <string>
#include <vector>
#include <map>
#include "EnvironmentMap.h"
#include "RadeonRays\bvh.h"
#include "RadeonRays\bvh_translator.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

class Camera;

enum LightType
{
    RectLight, // 矩形光源
    SphereLight, // 球型光
    DistantLight //平行光
};

struct Light
{
    Vec3 position;
    Vec3 emission;
    Vec3 u;
    Vec3 v;
    float radius;
    float area;
    float type;
};

struct Indices
{
    int x, y, z;
};

class Scene
{
public:
    Scene() : camera(nullptr), envMap(nullptr), initialized(false), dirty(true)
    {
        sceneBvh = new RadeonRays::Bvh(10.0f, 64, false);
    }

    ~Scene();

    int AddMesh(const std::string& filename);
    int AddTexture(const std::string& filename);
    int AddMaterial(const Material& material);
    int AddMeshInstance(const MeshInstance& meshInstance);
    int AddLight(const Light& light);

    void AddCamera(Vec3 eye, Vec3 lookat, float fov);
    void AddEnvMap(const std::string& filename);

    void ProcessScene();
    void RebuildInstances();

public:
    RenderOptions renderOptions;

    std::vector<Indices>vertIndices; // Indices
    std::vector<Vec4>verticesUVX; //VertexPostion and UV.x
    std::vector<Vec4>normalsUVY; //VertexNormal and UV.y

    std::vector<Mesh* > meshes; //Mesh
    std::vector<MeshInstance> meshInstances;
    std::vector<Mat4> transforms; // MeshInstances's transforms
    std::vector<Material> materials;

    std::vector<Light> lights; // Light

    EnvironmentMap* envMap; // EnvironMentMap
    Camera* camera; // Scene Camera

    // Bvh
    RadeonRays::BvhTranslator bvhTranslator; // Produces a flat bvh array for GPU consumption
    RadeonRays::bbox sceneBounds; //The biggest Bounds

    // Texture Data
    std::vector<Texture*> textures; // Textures
    std::vector<unsigned char> textureMapsArray; // TODO ?

    bool initialized;
    bool dirty;
    // To check if scene elements need to be resent to GPU
    bool instancesModified = false;
    bool envMapModified = false;

private:
    RadeonRays::Bvh* sceneBvh;
    void createBLAS();
    void createTLAS();
};





