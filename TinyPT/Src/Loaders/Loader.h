#pragma once
#include "Core/Scene.h"


class Loader
{
    bool LoadSceneFromFile(const std::string& filename, Scene* scene, RenderOptions& renderOptions);
};

