#pragma once
#include "Math/Vec2.h"
#include "Math/Vec3.h"

struct RenderOptions
{
public:
    RenderOptions()
    {
        renderResolution = iVec2(1280, 720);
        windowResolution = iVec2(1280, 720);
        uniformLightCol = Vec3(0.3f, 0.3f, 0.3f);
        backgroundCol = Vec3(1.0f, 1.0f, 1.0f);
        tileWidth = 100;
        tileHeight = 100;
        maxDepth = 2;
        maxSpp = -1;
        RRDepth = 2;
        texArrayWidth = 2048;
        texArrayHeight = 2048;
        denoiserFrameCnt = 20;
        enableRR = true;
        enableDenoiser = false;
        enableTonemap = true;
        enableAces = false;
        openglNormalMap = true;
        enableEnvMap = false;
        enableUniformLight = false;
        hideEmitters = false;
        enableBackground = false;
        transparentBackground = false;
        independentRenderSize = false;
        enableRoughnessMollification = false;
        enableVolumeMIS = false;
        envMapIntensity = 1.0f;
        envMapRot = 0.0f;
        roughnessMollificationAmt = 0.0f;
    }

public:
    iVec2 renderResolution;
    iVec2 windowResolution;
    Vec3 uniformLightCol;
    Vec3 backgroundCol;

    int tileWidth;
    int tileHeight;

    int maxSpp;
    int maxDepth;

    bool enableRR;
    int RRDepth;

    int texArrayWidth;
    int texArrayHeight;

    bool enableDenoiser;
    int denoiserFrameCnt;

    bool enableTonemap;
    bool enableAces;
    bool simpleAcesFit;

    bool openglNormalMap;
    bool enableEnvMap;
    bool enableUniformLight;
    bool hideEmitters;

    bool enableBackground;
    bool transparentBackground;
    bool independentRenderSize;
    bool enableRoughnessMollification;

    bool enableVolumeMIS;
    float envMapIntensity;
    float envMapRot;
    float roughnessMollificationAmt;

};
