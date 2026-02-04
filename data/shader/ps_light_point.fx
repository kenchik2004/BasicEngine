//----------------------------------------------------------------------------
//!	@file	ps_light_directional.fx
//!	@brief	点光源
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
    if (surfaceInfo.depth_ == 1.0f)
    {
        // 背景
        discard;
    }
    
    float3 L = light_info_[0].light_position_ - surfaceInfo.world_position_;
    float distance = length(L);
    
    if (light_info_[0].light_range_ < distance)
    {
        discard;
    }
    
    L = normalize(L);
    
    float3 N = surfaceInfo.normal_;
    float3 V = normalize(eye_position_ - surfaceInfo.world_position_);
    float3 H = normalize(L + V);

    //----------------------------------------------------------
    // 光源計算
    //----------------------------------------------------------
    //float3 lightColor = float3(1, 1, 1) * 10;
	
    float3 diffuse;
    float3 specular;
	
    lighting(light_info_[0].light_color_.rgb,
             N, L, V, H,
             surfaceInfo.roughness_, surfaceInfo.metallic_,
             surfaceInfo.albedo_,
             diffuse, specular);
    
    // 距離減衰
    
   // distance *= 0.01f;
    
    
    float s = saturate(distance / light_info_[0].light_range_);
    float f = light_info_[0].intensity; // f 1.0~4.0 (大きくすると強く減衰する)
    float attenuation = (1 - s * s) * (1 - s * s) / (1 + f * s);
    
    float3 ambient = float3(1.0, 1.0, 1.0) * surfaceInfo.albedo_;
    diffuse += ambient;
    diffuse *= attenuation;
    specular *= attenuation;

    //----------------------------------------------------------
    // 出力
    //----------------------------------------------------------
    PS_OUTPUT_LIGHTING output;
    
    output.diffuse_ = float4(diffuse, 1.0f);
    output.specular_ = float4(specular, 1.0f);

	// 出力パラメータを返す
    return output;
}
