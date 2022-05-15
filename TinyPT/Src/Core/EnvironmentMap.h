#pragma once
#include <string>
#include <vector>

#include "stb/stb_image.h"

//Manager Of EnvironmentMap
class EnvironmentMap
{
public:
    EnvironmentMap() : width(0), height(0), img(nullptr), cdf(nullptr) {};
    ~EnvironmentMap() { stbi_image_free(img); delete[] cdf; }

    bool LoadMap(const std::string& filename);
    void BuildCDF();
public:
    int width;
    int height;
    float totalSum;
    float* img;
    float* cdf;
};
