//----------------------------------------------------------------------------
//!	@file	ps_light_directional.fx
//!	@brief	平行光源
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"
#include "gbuffer.h.fx"
#include "lighting.h.fx"
#include "camera.h.fx"
#include "shadow.h.fx"

struct PS_OUTPUT_LIGHTING
{
	float4 diffuse_ : SV_Target0;
	float4 specular_ : SV_Target1;
};


//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT_LIGHTING main(PS_INPUT input)
{
	int2 position = input.position_.xy;

	// GBufferから情報を取得
	SurfaceInfo surfaceInfo = GetSurfaceInfo(position);

	float3 L = normalize(light_info_[0].direction_);
	float3 N = surfaceInfo.normal_;
	float3 V = normalize(eye_position_ - surfaceInfo.world_position_);
	float3 H = normalize(L + V);

	if (surfaceInfo.depth_ == 1.0f)
	{
		// 背景
		discard;
	}

	//----------------------------------------------------------
	// シャドウ
	//----------------------------------------------------------

	// このシェーダー内はプリミティブは2D全体描画のため、本来の3D奥行を再設定
	input.position_.w = length(surfaceInfo.world_position_ - eye_position_);

	float shadow = getShadow(input.position_, surfaceInfo.world_position_);

	// Lambertと暗いほうを選択する
	shadow = min(shadow, saturate(dot(N, L)));


	// 疑似的に影を薄くする
	//shadow = shadow * 0.5 + 0.5;

	//----------------------------------------------------------
	// 光源計算
	//----------------------------------------------------------
	float3 lightColor = light_info_[0].light_color_;

	float3 diffuse;
	float3 specular;

	lighting(lightColor,
		N, L, V, H,
		surfaceInfo.roughness_, surfaceInfo.metallic_,
		surfaceInfo.albedo_,
		diffuse, specular);


	diffuse *= shadow;
	specular *= shadow;

	//----------------------------------------------------------
	// 出力
	//----------------------------------------------------------
	PS_OUTPUT_LIGHTING output;

	float3 ambient = float3(1.0, 1.0, 1.0) * surfaceInfo.albedo_*0;
	output.diffuse_ = float4(diffuse + ambient, 1.0f);
	output.specular_ = float4(specular, 1.0f);

	// 出力パラメータを返す
	return output;
}
