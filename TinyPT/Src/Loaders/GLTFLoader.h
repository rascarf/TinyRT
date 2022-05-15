#pragma once
#include "Core/Scene.h"

class Scene;
bool LoadGLTF(const std::string& filename, Scene* scene, RenderOptions& renderOptions, Mat4 xform, bool binary);