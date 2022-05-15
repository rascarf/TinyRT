#pragma once
#include "Pch.h"

class Program;

class Quad
{
public:
    Quad();
    void Draw(Program*);

private:
    GLuint vao;  // 顶点数据的解析
    GLuint vbo; //一块单纯的显存
};
