#pragma once

#include <vector>
#include "Shader.h"

class Program
{
public:
    Program(const std::vector<Shader> shaders);
    ~Program();
    void Use();
    void StopUsing();
    GLuint getObject();

private:
    GLuint object;
};
