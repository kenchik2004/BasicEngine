//----------------------------------------------------------------------------
//!	@file	ps_tonemapping.fx
//!	@brief	HDRトーンマッピング
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"

// sRGBから入力する場合の変換行列
// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
static const float3x3 ACESInputMat =
{
    { 0.59719, 0.35458, 0.04823 },
    { 0.07600, 0.90834, 0.01566 },
    { 0.02840, 0.13383, 0.83777 }
};

// AP1(ACES2065-1)から入力する場合の変換行列
// AP1 => RRT_SAT
static const float3x3 AP1_2_RRT_SAT_MAT =
{
    { 0.9708890, 0.0269633, 0.0021476 },
    { 0.0108892, 0.9869630, 0.0021476 },
    { 0.0108892, 0.0269633, 0.9621480 }
};


// 変換後の色空間からsRGBへの変換行列
// ODT_SAT => XYZ => D60_2_D65 => sRGB
static const float3x3 ACESOutputMat =
{
    { 1.60475, -0.53108, -0.07367 },
    { -0.10208, 1.10813, -0.00605 },
    { -0.00327, -0.07276, 1.07602 }
};

//----------------------------------------------------------------------------
//!	@brief	RRTとODTを適用する関数
//----------------------------------------------------------------------------
float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

//----------------------------------------------------------------------------
//!	@brief	ACESトーンマッピングを適用する関数
float3 ACESFitted(float3 color)
{
    color = max(color, 0.0);

    color = mul(AP1_2_RRT_SAT_MAT, color);

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = mul(ACESOutputMat, color);

    // Clamp to [0, 1]
    color = saturate(color);

    return color;
}
//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

	// テクスチャカラーの読み込み
    float4 color = DiffuseTexture.Sample(DiffuseSampler, input.uv0_);
    
	// トーンマッピング適用
    color.rgb = ACESFitted(color.rgb);

	// sRGBへ変換
    color.rgb = pow(abs(color.rgb), 1.0 / 2.2);

    output.color0_ = color;

	// 出力パラメータを返す
    return output;
}
