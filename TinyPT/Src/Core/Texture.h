#pragma once
#include <vector>
#include <algorithm>
#include <string>

class Texture
{
public:
    Texture() :width(0), height(0), components(0) {};
    Texture(std::string texName, unsigned char* data, int w, int h, int c);

    bool LoadTexture(const std::string& filename);

    ~Texture(){}


public:
    int width;
    int height;
    int components;
    std::vector<unsigned char> texData;
    std::string name;

};
