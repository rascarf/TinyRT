#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

void Mesh::BuildBVH()
{
    //TODO 理解
    const int numTris = verticesUVX.size() / 3;
    std::vector<RadeonRays::bbox> bounds(numTris);

#pragma omp parallel for
    for (int i = 0; i < numTris; ++i)
    {
        const Vec3 v1 = Vec3(verticesUVX[i * 3 + 0]);
        const Vec3 v2 = Vec3(verticesUVX[i * 3 + 1]);
        const Vec3 v3 = Vec3(verticesUVX[i * 3 + 2]);

        bounds[i].grow(v1);
        bounds[i].grow(v2);
        bounds[i].grow(v3);
    }

    bvh->Build(&bounds[0], numTris);
}

bool Mesh::LoadFromFile(const std::string& filename)
{
    name = filename;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
        filename.c_str(), 0, true);

    if(!ret)
    {
        printf("Unable to load model\n");
        return false;
    }

    //对每一个Shape
    for(size_t s = 0; s <shapes.size();s++)
    {
        //遍历上面的每一个Face
        size_t index_offset = 0;

        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size() ; f++)
        {
            //遍历face上的每个点
            for(size_t v = 0 ; v < 3; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

                tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

                tinyobj::real_t tx, ty;

                //如果存在uv
                if (!attrib.texcoords.empty())
                {
                    tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                    ty = 1.0 - attrib.texcoords[2 * idx.texcoord_index + 1];
                }
                else
                {
                    if (v == 0)
                        tx = ty = 0;
                    else if (v == 1)
                        tx = 0, ty = 1;
                    else
                        tx = ty = 1;
                }

                verticesUVX.push_back(Vec4(vx, vy, vz, tx));
                normalsUVY.push_back(Vec4(nx, ny, nz, ty));
            }

            index_offset += 3;
        }
    }

    return true;

}
