//----------------------------------------------------------------------------
//!	@file	lighting.h.fx
//!	@brief	照明計算関連
//----------------------------------------------------------------------------
#ifndef LIGHTING_H_FX
#define LIGHTING_H_FX
#include "dxlib_ps.h.fx"
// 定数バッファ
struct LightInfo
{
    float3 light_position_; //!< 光源の位置座標
    float pad1;
    float3 direction_; //!< 光源の向き
    float pad2;
    float3 light_color_; //!< 光源の色
    float light_range_; //!< 光源の影響範囲
};
cbuffer LightInfoBuffer : register(b11)
{
    LightInfo light_info_[6];
}


// GGX スペキュラーモデル (Trowbridge Reitz)
// エネルギー保存則(Normalized distribution function) / 微細表面(microfacet)
//!	@param	[in]	N			法線(Normal)
//!	@param	[in]	H			ハーフベクトル(Half)
//!	@param	[in]	roughness	表面の粗さ 0.0(つるつる)～1.0(ざらざら)
//!	@param	[in]	gamma		スペキュラーの減衰テールの係数
float D_GGX(float3 N, float3 H, float roughness)
{
	//                            a^2
	//	D(N, H, a) = -------------------------------------
	//                π * [ (a^2 - 1)( dot(N, H)^2 + 1]^2
	// ※a は roughness の2乗

    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;
    float numerator = a2;
    float denominator = (NdotH2 * (a2 - 1.0) + 1.0);
    denominator = PI * denominator * denominator;
    return numerator / denominator;
}

//!	フレネル反射 (Schlick近似)
//!	[in]	f0		初期の反射率(fresnel bias)
//!	[in]	NdotV	NとVの内積値
float3 F_Schlick(float3 f0, float NdotV)
{
    float x = 1.0 - NdotV;
    return f0 + (1.0 - f0) * (x * x * x * x * x);
}

//! G項 Smith 自己遮蔽モデル
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
	// diffuse (ディフューズ) = 拡散反射光
	//-------------------------------------------------------------
	// Lambert 拡散反射光モデル
    // float diffuse = max(0, dot(N, L));
    const float Kd = 1.0 / PI;
    diffuse = lightColor * (NdotL * Kd) * albedo;

	//-------------------------------------------------------------
	// specular (スペキュラー) = 鏡面反射光
	//-------------------------------------------------------------
#if 0
	// Blinn-phong 鏡面反射光モデル
    float shininess = 200.0;
    // float Ks = (shininess + 2.0) / (2.0 * PI);
    float Ks = shininess * (1.0 / (2.0 * PI)) + (2.0 / (2.0 * PI));
    float specular = pow(saturate(dot(N, H)), shininess) * Ks;
	
    specular *= 0.5;	// 強さ調整
#elif 0
	
	// Cook-Torrance BRDFモデル
	//        D * F * G
	// -----------------------
	//  4 * (NdotL) * (NdotV)
	//
	// 照明計算 = (BRDF) * (光の色,強さ) * (NdotL)
	//       Cook-Torrance
    float3 brdf = (D_GGX(N, H, roughness) * F_Schlick(specularColor, NdotV) * G_Smith_Schlick_GGX(roughness, NdotV, NdotL))
	              / //--------------------------------------------------------------------------------------------------------------------
	                                       (4.0 * /*NdotL * */NdotV);
	
    float3 specular = brdf * lightColor /* * NdotL*/;
#else
	// Cook-Torrance BRDF 近似高速化 (Optimizing PBR SIGGRAPH2015)
    float roughness4 = roughness * roughness * roughness * roughness;
    float denominator = (NdotH * NdotH * (roughness4 - 1.0) + 1.0) * LdotH;
	
    float3 brdf = roughness4 * rcp(4.0 * PI * denominator * denominator * (roughness + 0.5));
	
    specular = brdf * NdotL * specularColor * lightColor;
#endif	
}









#endif // LIGHTING_H_FX

