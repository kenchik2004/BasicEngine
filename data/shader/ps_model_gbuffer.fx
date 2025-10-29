﻿//----------------------------------------------------------------------------
//!	@file	ps_model_gbuffer.fx
//!	@brief	MV1モデルピクセルシェーダー / GBuffer出力
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"
#include "gbuffer.h.fx"

// 頂点シェーダーの出力
struct VS_OUTPUT_MODEL
{
    float4 position_ : SV_Position; //!< 座標       (スクリーン空間)
    float4 curr_position_ : CURR_POSITION; //!< 現在の座標 (スクリーン空間)
    float3 world_position_ : WORLD_POSITION; //!< ワールド座標
    float3 normal_ : NORMAL0; //!< 法線
    float4 diffuse_ : COLOR0; //!< Diffuseカラー
    float2 uv0_ : TEXCOORD0; //!< テクスチャ座標
    float4 prev_position_ : PREV_POSITION; //!< 1フレーム前の座標 (スクリーン空間) ※末尾に追加されているため注意
};

typedef VS_OUTPUT_MODEL PS_INPUT_MODEL;

struct PS_OUTPUT_MRT
{
    float4 color0_ : SV_Target0;
    float4 color1_ : SV_Target1;
    float4 color2_ : SV_Target2;
};






//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT_MRT main(PS_INPUT_MODEL input)
{
    float2 uv = input.uv0_;

    float3 N = normalize(input.normal_); // 法線
	
	//------------------------------------------------------------
	// 法線マップ
	//------------------------------------------------------------
    N = Normalmap(N, input.world_position_, uv);

	//------------------------------------------------------------
	// テクスチャカラーを読み込み
	//------------------------------------------------------------
    float4 textureColor = DiffuseTexture.Sample(DiffuseSampler, uv);
    textureColor = saturate(textureColor);
	
    // アルファテスト
    if (textureColor.a < 0.5)
        discard;
	
    float3 albedo = textureColor.rgb * input.diffuse_.rgb;
	
    float roughness = 0.7; // ラフ度 0.0:つるつる ～ 1.0:ざらざら (別名:glossiness, shininess)
    float metallic = 0.3; // 金属度 0.0:非金属   ～ 1.0:金属     (別名:metalness)
    float2 roughness_metallic = float2(roughness, metallic); //アルファブレンドが避けられないので、一つのチャンネルにまとめて格納
    float ao = 1.0f;
    
    //----------------------------------------------------------
    // 出力
    //----------------------------------------------------------
    PS_OUTPUT_MRT output;
    
    output.color0_ = float4(albedo, ao);
    output.color1_ = float4(NormalEncode(N).xy, roughness,metallic);
    output.color2_ = float4(input.world_position_.xyz, 1.0f);
	// 出力パラメータを返す
    return output;
}
