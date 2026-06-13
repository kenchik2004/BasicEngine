//----------------------------------------------------------------------------
//!	@file	ps_hunted_effect.fx
//!	@brief	確保エフェクト用デプス切り抜きシェーダー
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
    
    //MRTのデプスバッファはNormalTextureに格納されているため、NormalTextureからデプス値を読み込む
    float depth = NormalTexture.Sample(DiffuseSampler, input.uv0_).r;
	// 出力カラー = テクスチャカラー
    output.color0_ = color;
    //デプスが1.0（つまり、背景）であれば、EmissionTextureから切り抜き用のカラーを読み込む
    if (depth == 1.0)
    {
        float4 cutin_color = EmissionTexture.Sample(EmissionSampler, input.uv0_);
        output.color0_ = cutin_color;

    }

	// 出力パラメータを返す
    return output;
}