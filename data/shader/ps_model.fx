﻿//----------------------------------------------------------------------------
//!	@file	ps_model.fx
//!	@brief	MV1モデルピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

// 定数バッファは b0～b14 まで利用可能
uint XorShift(uint seed)
{
    uint x = seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}
cbuffer CameraInfo : register(b11)
{
    matrix mat_view_; //!< ビュー行列
    matrix mat_proj_; //!< 投影行列
    float3 eye_position_; //!< カメラの位置
};
cbuffer LightInfo : register(b13)
{
    matrix mat_light_view_; //!< ライトビュー行列
    float4 light_color_; //!< 光源の色
    float3 light_direction_; //!< 光源の方向
};

// 頂点シェーダーの出力
struct VS_OUTPUT_MODEL
{
    float4 position_ : SV_Position; //!< 座標       (スクリーン空間)
    float4 curr_position_ : CURR_POSITION; //!< 現在の座標 (スクリーン空間)
    float3 world_position_ : WORLD_POSITION; //!< ワールド座標
    float3 normal_ : NORMAL0; //!< 法線
    float4 diffuse_ : COLOR0; //!< Diffuseカラー
    float2 uv0_ : TEXCOORD0; //!< テクスチャ座標
};

typedef VS_OUTPUT_MODEL PS_INPUT_MODEL;

//==============================================================
//デコード
//==============================================================
float DecodeFloatBilinear(float4 rgba)
{
    uint r = (uint) (rgba.r * 255.0 + 0.5);
    uint g = (uint) (rgba.g * 255.0 + 0.5);
    uint b = (uint) (rgba.b * 255.0 + 0.5);
    uint value = r + (g << 8) + (b << 16);
    return (float) value / 16777215.0;
}



//==============================================================
// IBL
//==============================================================
TextureCube IBL_Diffuse : register(t14);
TextureCube IBL_Specular : register(t15);






static const float PI = 3.14159265359;

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

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_MODEL input)
{
    PS_OUTPUT output;
    float2 uv = input.uv0_;
    float3 N = normalize(input.normal_); // 法線
    float3 V = normalize(eye_position_ - input.world_position_);
	
	
	//------------------------------------------------------------
	// 法線マップ
	//------------------------------------------------------------
    N = Normalmap(N, input.world_position_, uv);
	//------------------------------------------------------------
	// テクスチャカラーを読み込み
	//------------------------------------------------------------
    float4 textureColor = DiffuseTexture.Sample(DiffuseSampler, uv);
#if 1
    output.color0_ = textureColor;
    return output;
#endif
    float shadow = 0.0;
#if 1
    float4 lightSpace = mul(float4(input.world_position_.xyz, 1.0f), mat_light_view_);
    lightSpace.xyz /= lightSpace.w;
    float2 UV = lightSpace.xy * float2(0.5, -0.5) + 0.5;
    //UV.y = 1.0 - UV.y;


    float shadowDepth = lightSpace.z;
    for (int y = -2; y <= 2; ++y)
        for (int x = -2; x <= 2; ++x)
        {
            {
                static const float bias = 0.00001;
                
               // uint rand = XorShift(x + y * 5);
                
                float2 offset = float2(x, y) * 0.0002; // * rand;
               
                float4 sample = ShadowMap0Texture.Sample(ShadowMap0Sampler, UV + offset);
    
   //output.color0_ = float4(sample.rrr * 2, 1);
    //return output;
                float mapDepth = DecodeFloatBilinear(sample);
	


                shadow += (shadowDepth > mapDepth + bias) ? 0.0 : 1.0;

            }
        }
    shadow *= 1.0f / 25.0f;
    shadow = all(UV >= 0 && UV <= 1 && shadowDepth > 0) ? shadow : 1.0;
		
	// アルファテスト
    if (textureColor.a < 0.5)
        discard;
#endif
#if 1
	// リニア化 sRGBのテクスチャ → Linearのテクスチャ
    textureColor.rgb = pow(abs(textureColor.rgb), 2.2);
#endif
    float3 albedo = textureColor.rgb;
	
    output.color0_ = textureColor * input.diffuse_;
	
	
#if 1
	
	
	
    float distance = input.position_.w;
	
	
	// LERP: 線形補間 Linear intERPoration
	// result = lerp(A, B, ratio);			0 <= ratio <= 1
	// ※ratioは範囲外になっても動作するが、期待した結果にならない場合はsaturateする
	
    // saturate 飽和 (入力値を0.0	-1.0の間に収める)
	// float n;
	// if (n < 0.0) n = 0.0;
	// if (n > 1.0) n = 1.0;
	// ※saturateは0サイクルで実行可能。コストゼロ

	// abs 絶対値 absolute
	// if(n < 0.0) n = -n;
	// ※absは0サイクルで実行可能。コストゼロ
	// -n;		// 1サイクル
    // abs(n)	// 0サイクル

	// max 最大値
	// min 最小値
	// result = max(a, b);
	// result = min(a, b);
	// aとbの大きい方(または小さい方)を選択する	
	
	// clamp 範囲制限
	// result clamp(x, 0.0, 2.0);
	// 最小値～最大値に収める
	
	
#if 0
	//-------------------------------------------------------------
	// フォグ表現
	//-------------------------------------------------------------
    float ratio = saturate(distance * 0.005);
    output.color0_.rgb = lerp(output.color0_.rgb, float3(0.4, 0.6, 1), ratio);
	
    int2 position = int2(input.position_.xy); // 画面上のピクセル位置
#endif
	//-------------------------------------------------------------
	// CRTスキャンライン表現
	//-------------------------------------------------------------
    // if ((position.y & 1)  == 0)
    //     output.color0_.rgb = 0;
	
	// 市松模様 (checker board)
    //if (( ((position.x >> 4) & 1) ^ ((position.y >> 4) & 1) ) == 1)
    //    output.color0_.rgb = 0;

	// グラデーション	
    //output.color0_.rg = position.xy * float2(1.0 / 1280, 1.0 / 720);
	


	//----------------------------------------------------------
	// RGBの色以外の値を代入してデバッグ表示
	//----------------------------------------------------------
	// output.color0_.rgb = input.world_position_ * 0.1;
	// output.color0_.rgb = normalize(N);
    // output.color0_.rgb = float3(input.uv0_, 0);
	

    float3 L = normalize(light_direction_);
    float3 H = normalize(L + V); // ハーフベクトル
	
	
	
    //float3 light = float3(1.0, 1.0, 1.0); // 0-1の範囲
//  float3 light = float3(1.0, 0.8, 0.6); // 0-1の範囲
	

    float3 lightColor = light_color_.rgb * light_color_.a; // 光源の明るさ, 色

    float NdotL = saturate(dot(N, L)) + 0.000001;
    float NdotV = saturate(dot(N, V)) + 0.000001;

    float NdotH = saturate(dot(N, H)) + 0.000001;
    float LdotH = saturate(dot(L, H)) + 0.000001;
	
    float roughness = 0.5; // ラフ度 0.0:つるつる ～ 1.0:ざらざら (別名:glossiness, shininess)
    float metallic = 0.9; // 金属度 0.0:非金属   ～ 1.0:金属     (別名:metalness)
	
    float3 specularColor = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);

   // shadow = max(shadow, NdotL);
    //output.color0_ = float4(specularColor, 1);
    //return output;
#if 1
	//-------------------------------------------------------------
	// diffuse (ディフューズ) = 拡散反射光
	//-------------------------------------------------------------
	// Lambert 拡散反射光モデル
    //float diffuse = max(0, dot(N, L));
    const float Kd = 1.0 / PI;
    float3 diffuse = lightColor * (saturate(dot(N, L)) * Kd);
#endif
	//-------------------------------------------------------------
	// specular (スペキュラー) = 鏡面反射光
	//-------------------------------------------------------------
#if 0
	// Blinn-phong 鏡面反射光モデル
    float shininess = 50.0;
    // float Ks = (shininess + 2.0) / (2.0 * PI);
    float Ks = shininess * (1.0 / (2.0 * PI)) + (2.0 / (2.0 * PI));
    float specular = pow(saturate(dot(N, H)), shininess) * Ks;
	
    specular *= 0.1;	// 強さ調整
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
	
    float3 specular = brdf * specularColor * lightColor;
#endif	
	
	
	
	
	
	
	
	
	// ambient light 環境光
	// 周辺の明るさを近似
    float3 ambient = float3(0.1, 0.1, 0.1);

	
    output.color0_.rgb *= diffuse + ambient;
    
    output.color0_.rgb += specular;
#if 0
	
	//==========================================================
	// Image Based Lighting
	//==========================================================
	{
        float3 ibl_diffuse = IBL_Diffuse.Sample(DiffuseSampler, N).rgb * Kd;
        output.color0_ = float4(ibl_diffuse, 1.0);
        return output;
        ibl_diffuse *= textureColor.rgb;
        output.color0_.rgb = ibl_diffuse;
		
		// 反射ベクトルR
        float3 R = reflect(-V, N);
		
        float mip = roughness * 7;
        float3 ibl_specular = IBL_Specular.Sample(DiffuseSampler, R).rgb;
        float3 environmentBrdf = pow(1.0 - max(roughness, NdotV), 3.0) + specularColor;
        output.color0_ = float4(ibl_specular, 1.0);
        return output;
        ibl_specular *= environmentBrdf;

        output.color0_.rgb = ibl_diffuse * (1.0 - metallic) + ibl_specular;

		output.color0_.rgb *= 2;
    }
#endif


    //output.color0_.rgb *= 4;
	
	
	//----------------------------------------------------------
	// ガンマ補正 Linear → sRGB
	// ※表示用のガンマカーブ
	//----------------------------------------------------------
	
	// pow べき乗
	// pow(n, x);	nのx乗
    output.color0_.rgb = pow(abs(output.color0_.rgb), 1.0 / 2.2);
    output.color0_.rgb *= shadow;
#endif
	
	
	// 出力パラメータを返す
    return output;
}
