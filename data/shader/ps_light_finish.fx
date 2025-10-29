//----------------------------------------------------------------------------
//!	@file	ps_light_finish.fx
//!	@brief	照明パスの最終合成
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"

Texture2D LightDiffuseTexture : register(t21); // Diffuseテクスチャ
Texture2D LightSpecularTexture : register(t22); // Specularテクスチャ

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    int2 position = input.position_.xy; // ピクセル位置
	
    float4 diffuse = LightDiffuseTexture.Load(int3(position, 0));
    float4 specular = LightSpecularTexture.Load(int3(position, 0));
	
    PS_OUTPUT output;
    float3 ambient = float3(1.0, 1.0, 1.0)*0.0;
	
    output.color0_ = diffuse + float4(ambient, 0) + specular;

	// 出力パラメータを返す
    return output;
}
