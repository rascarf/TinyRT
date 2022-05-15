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
    Vec3 baseColor; //������ɫ
    float anisotropic; // ��������

    Vec3 emission; //�Է���
    float padding1; // ����float4���ֽڶ���

    float metallic; // ������
    float roughness; // �ֲڶ�
    float subsurface; // ΢ƽ��
    float specularTint; // TODO ��

    float sheen; //TODO ��
    float sheenTint; // TODO ��
    float clearcoat; // �����
    float clearcoatGloss; // ������ɫ

    float specTrans; // �߹⴫��
    float ior; // ����
    float mediumType; //��������
    float mediumDensity; //�����ܶ�

    Vec3 mediumColor; //�м����ɫ
    float mediumAnisotropy; // ����

    float baseColorTexId; // Albedo��ͼ
    float metallicRoughnessTexID; // MR��ͼ
    float normalmapTexID; // ������ͼ
    float emissionmapTexID; // �Է�����ͼ

    float opacity; //͸����
    float alphaMode; // Alpha������
    float alphaCutoff; //��������
    float padding2; // �ֽڶ���

};