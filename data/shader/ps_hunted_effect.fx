//----------------------------------------------------------------------------
//!	@file	ps_hunted_effect.fx
//!	@brief	確保エフェクト用BB切り抜きピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

	// テクスチャカラーの読み込み
    float4 color = DiffuseTexture.Sample(DiffuseSampler, input.uv0_);

	// 出力カラー = テクスチャカラー
    output.color0_ = color;
    if (dot(normalize(color.rgb), float3(0, 0, 1)) >= 0.99)
    {
        float4 cutin_color = EmissionTexture.Sample(EmissionSampler, input.uv0_);
        output.color0_ = cutin_color;

    }

	// 出力パラメータを返す
    return output;
}