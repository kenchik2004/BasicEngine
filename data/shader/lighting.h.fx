//----------------------------------------------------------------------------
//!	@file	lighting.h.fx
//!	@brief	�Ɩ��v�Z�֘A
//----------------------------------------------------------------------------
#ifndef LIGHTING_H_FX
#define LIGHTING_H_FX
#include "dxlib_ps.h.fx"
// �萔�o�b�t�@
struct LightInfo
{
    float3 light_position_; //!< �����̈ʒu���W
    float pad1;
    float3 direction_; //!< �����̌���
    float pad2;
    float3 light_color_; //!< �����̐F
    float light_range_; //!< �����̉e���͈�
};
cbuffer LightInfoBuffer : register(b11)
{
    LightInfo light_info_[6];
}


// GGX �X�y�L�����[���f�� (Trowbridge Reitz)
// �G�l���M�[�ۑ���(Normalized distribution function) / ���ו\��(microfacet)
//!	@param	[in]	N			�@��(Normal)
//!	@param	[in]	H			�n�[�t�x�N�g��(Half)
//!	@param	[in]	roughness	�\�ʂ̑e�� 0.0(���)�`1.0(���炴��)
//!	@param	[in]	gamma		�X�y�L�����[�̌����e�[���̌W��
float D_GGX(float3 N, float3 H, float roughness)
{
	//                            a^2
	//	D(N, H, a) = -------------------------------------
	//                �� * [ (a^2 - 1)( dot(N, H)^2 + 1]^2
	// ��a �� roughness ��2��

    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;
    float numerator = a2;
    float denominator = (NdotH2 * (a2 - 1.0) + 1.0);
    denominator = PI * denominator * denominator;
    return numerator / denominator;
}

//!	�t���l������ (Schlick�ߎ�)
//!	[in]	f0		�����̔��˗�(fresnel bias)
//!	[in]	NdotV	N��V�̓��ϒl
float3 F_Schlick(float3 f0, float NdotV)
{
    float x = 1.0 - NdotV;
    return f0 + (1.0 - f0) * (x * x * x * x * x);
}

//! G�� Smith ���ȎՕ����f��
float G_Smith_Schlick_GGX(float roughness, float NdotV, float NdotL)
{
    float a = roughness * roughness;
    float k = a * 0.5f;
    float GV = NdotV / (NdotV * (1 - k) + k);
    float GL = NdotL / (NdotL * (1 - k) + k);

    return GV * GL;
}





void lighting(in float3 lightColor, //
                in float3 N, //
                in float3 L, //
                in float3 V, //
                in float3 H, //
                in float roughness,
                in float metallic,
                in float3 albedo,
                out float3 diffuse,
                out float3 specular)
{
    float NdotH = saturate(dot(N, H)) + 0.000001;
    float LdotH = saturate(dot(L, H)) + 0.000001;
    float NdotV = saturate(dot(N, V)) + 0.000001;
    float NdotL = saturate(dot(N, L)) + 0.000001;
    
    float reflectance = 0.04;
    float3 specularColor = lerp(float3(reflectance, reflectance, reflectance), albedo, metallic);
    
    
	//-------------------------------------------------------------
	// diffuse (�f�B�t���[�Y) = �g�U���ˌ�
	//-------------------------------------------------------------
	// Lambert �g�U���ˌ����f��
    // float diffuse = max(0, dot(N, L));
    const float Kd = 1.0 / PI;
    diffuse = lightColor * (NdotL * Kd) * albedo;

	//-------------------------------------------------------------
	// specular (�X�y�L�����[) = ���ʔ��ˌ�
	//-------------------------------------------------------------
#if 0
	// Blinn-phong ���ʔ��ˌ����f��
    float shininess = 200.0;
    // float Ks = (shininess + 2.0) / (2.0 * PI);
    float Ks = shininess * (1.0 / (2.0 * PI)) + (2.0 / (2.0 * PI));
    float specular = pow(saturate(dot(N, H)), shininess) * Ks;
	
    specular *= 0.5;	// ��������
#elif 0
	
	// Cook-Torrance BRDF���f��
	//        D * F * G
	// -----------------------
	//  4 * (NdotL) * (NdotV)
	//
	// �Ɩ��v�Z = (BRDF) * (���̐F,����) * (NdotL)
	//       Cook-Torrance
    float3 brdf = (D_GGX(N, H, roughness) * F_Schlick(specularColor, NdotV) * G_Smith_Schlick_GGX(roughness, NdotV, NdotL))
	              / //--------------------------------------------------------------------------------------------------------------------
	                                       (4.0 * /*NdotL * */NdotV);
	
    float3 specular = brdf * lightColor /* * NdotL*/;
#else
	// Cook-Torrance BRDF �ߎ������� (Optimizing PBR SIGGRAPH2015)
    float roughness4 = roughness * roughness * roughness * roughness;
    float denominator = (NdotH * NdotH * (roughness4 - 1.0) + 1.0) * LdotH;
	
    float3 brdf = roughness4 * rcp(4.0 * PI * denominator * denominator * (roughness + 0.5));
	
    specular = brdf * NdotL * specularColor * lightColor;
#endif	
}









#endif // LIGHTING_H_FX

