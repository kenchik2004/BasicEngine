//----------------------------------------------------------------------------
//!	@file	ps_ssao.fx
//!	@brief	Screen Space Ambient Occlusion ピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"
#include "gbuffer.h.fx"
#include "lighting.h.fx"
#include "camera.h.fx"
#include "shadow.h.fx"



//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target0
{
	int2 position = input.position_.xy;
	SurfaceInfo surfaceInfo = GetSurfaceInfo(position);
	if (surfaceInfo.depth_ == 1.0f)
	{
		// 背景
		discard;
	}
	input.position_.w = length(surfaceInfo.world_position_ - eye_position_);
	float3 N = surfaceInfo.normal_;
	float3 V = normalize(eye_position_ - surfaceInfo.world_position_);
	//----------------------------------------------------------
	//SSAO処理
	//----------------------------------------------------------

	float ao = 0;
	float3 vN = N;
	//接線ベクトルを計算する際の基準軸を決定するためのcosine値
	float cosine = abs(dot(vN, float3(0, 1, 0)));
	// 接線ベクトルを計算
	// cosineが十分に小さい場合はY軸を基準に、大きい場合はX軸を基準にする
	float3 vT = cross(vN, cosine < 0.7 ? float3(0, 1, 0) : float3(1, 0, 0));
	float3 vB = cross(vT, vN);
	vT = normalize(vT); //接線(横方向)
	vB = normalize(vB); //従法線(縦方向)
	vN = cross(vB, vT);

	float base_angle = rand(input.position_.xy * 0.0001 * BE_Default.delta_time) * 2 * PI;
	float3 sample_center_position = mul(float4(surfaceInfo.world_position_, 1), mat_view_).xyz;

	//半球状にサンプリングを行う
	static const int SAMPLE_COUNT = 64;
	[unroll]
	for (int i = 0; i < SAMPLE_COUNT; i++)
	{
		float2 offset = VogelDiskSample(i, SAMPLE_COUNT, base_angle);
		float height = abs(rand(input.position_.xy * 0.001 + i));

		float3 direction = normalize(float3(offset.x, offset.y, height));
		float3 offset_in_3d = (direction.x * vT) + (direction.y * vB) + (direction.z * vN);
		float3 sample_position = surfaceInfo.world_position_ + offset_in_3d;
		float3 view_position = mul(float4(sample_position, 1), mat_view_).xyz;
		float4 screen_position = mul(float4(view_position, 1), mat_proj_);


		screen_position.xyz /= screen_position.w;


		float2 uv = screen_position.xy * float2(0.5, -0.5) + 0.5;
		uv = saturate(uv);
		float2 resolution = float2(1280, 720);
		SurfaceInfo depth_surface = GetSurfaceInfo(int2(uv * resolution));
		float4 view_depth = mul(float4(screen_position.xy, depth_surface.depth_, 1), mat_proj_inv);
		view_depth.xyz /= view_depth.w;

		if (view_depth.z < view_position.z) {
			float3 dir_center = normalize(sample_center_position - view_position);
			float3 dir_sample = normalize(sample_center_position - view_depth.xyz);
			float cosine = dot(dir_center, dir_sample);
			ao += cosine / SAMPLE_COUNT;
		}
		//ao += 1.0f / SAMPLE_COUNT;
		/*return float4(view_depth.zzz*0.01, 1);
		return float4(view_position.zzz * 0.01, 1);*/

	}
	ao *= 1 - smoothstep(0.1, 50.0, sample_center_position.z);
	
	ao = 1.0 - ao;
	// 出力パラメータを返す
	return float4(1,1,1,ao);
}
