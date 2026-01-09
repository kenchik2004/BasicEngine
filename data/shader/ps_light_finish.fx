//----------------------------------------------------------------------------
//!	@file	ps_light_finish.fx
//!	@brief	照明パスの最終合成
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"
#include "gbuffer.h.fx"
#include "camera.h.fx"
Texture2D LightDiffuseTexture : register(t21); // Diffuseテクスチャ
Texture2D LightSpecularTexture : register(t22); // Specularテクスチャ

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
	int2 position = input.position_.xy; // ピクセル位置

	SurfaceInfo surfaceInfo = GetSurfaceInfo(position);
	if (surfaceInfo.depth_ == 1.0f)
	{
		PS_OUTPUT output;

		output.color0_.rgb = surfaceInfo.albedo_;
		output.color0_.a = 1.0f;
		output.color0_.rgb = pow(output.color0_.rgb, 1.0 / 2.2);
		return output;
	}
	float4 diffuse = LightDiffuseTexture.Load(int3(position, 0));
	float4 specular = LightSpecularTexture.Load(int3(position, 0));

	PS_OUTPUT output;

	{
		float ao = surfaceInfo.ao_;
		float3 colored_ao = surfaceInfo.albedo_ * ao * ao;
		diffuse.rgb = colored_ao;
		//diffuse.rgb = ao;

	}
	{
		float ao = surfaceInfo.ao_;
		float3 N = surfaceInfo.normal_;
		float3 V = normalize(eye_position_ - surfaceInfo.world_position_);
		float so = dot(N, V) + ao;
		so = saturate(so * so - 1.0 + ao);
		specular.rgb *= so;

	}
	float4 emissive = float4(saturate(surfaceInfo.albedo_.rgb * surfaceInfo.emissive_), 1);
	output.color0_ = diffuse + specular + emissive;
	output.color0_.rgb = pow(saturate(output.color0_.rgb), 1.0 / 2.2);

	// 出力パラメータを返す
	return output;
}
