#pragma once
#include "Math/Vec3.h"

enum AlphaMode
{
    Opaque,
    Blend,
    Mask
};

enum MediumType
{
    None,
    Absorb,
    Scatter,
    Emissive
};

class Material
{



public:
    Vec3 baseColor; //基础颜色
    float anisotropic; // 各向异性

    Vec3 emission; //自发光
    float padding1; // 必须float4个字节对齐

    float metallic; // 金属度
    float roughness; // 粗糙度
    float subsurface; // 微平面
    float specularTint; // TODO ？

    float sheen; //TODO ？
    float sheenTint; // TODO ？
    float clearcoat; // 清漆层
    float clearcoatGloss; // 清漆颜色

    float specTrans; // 高光传递
    float ior; // 介质
    float mediumType; //介质种类
    float mediumDensity; //介质密度

    Vec3 mediumColor; //中间的颜色
    float mediumAnisotropy; // ？？

    float baseColorTexId; // Albedo贴图
    float metallicRoughnessTexID; // MR贴图
    float normalmapTexID; // 法线贴图
    float emissionmapTexID; // 自发光贴图

    float opacity; //透明度
    float alphaMode; // Alpha的种类
    float alphaCutoff; //丢弃多少
    float padding2; // 字节对齐

};