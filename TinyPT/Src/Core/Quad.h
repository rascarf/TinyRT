#pragma once
#include "Pch.h"

class Program;

class Quad
{
public:
    Quad();
    void Draw(Program*);

private:
    GLuint vao;  // �������ݵĽ���
    GLuint vbo; //һ�鵥�����Դ�
};
