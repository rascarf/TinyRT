#pragma once
#include "Pch.h"
#include "ShaderInclude.h"


class Shader
{
private:
    GLuint object;
public:
    Shader(const ShaderInclude::ShaderSource& sourceObj, GLuint shaderType);

    GLuint getObject() const;
};
