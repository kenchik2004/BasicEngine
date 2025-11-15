//----------------------------------------------------------------------------
//!	@file	ps_light_finish.fx
//!	@brief	照明パスの最終合成
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"
#include "gbuffer.h.fx"
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
	
    output.color0_ = diffuse + specular;
    
    output.color0_.rgb = pow(output.color0_.rgb, 1.0 / 2.2);

	// 出力パラメータを返す
    return output;
}
