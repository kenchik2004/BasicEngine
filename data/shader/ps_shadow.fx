//----------------------------------------------------------------------------
//!	@file	ps_model.fx
//!	@brief	MV1モデルピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h"


// 頂点シェーダーの出力
struct VS_OUTPUT_MODEL
{
    float4 position_ : SV_Position; //!< 座標       (スクリーン空間)
};

typedef VS_OUTPUT_MODEL PS_INPUT_MODEL;
float4 EncodeFloatBilinear(float v)
{
    v = saturate(v); // Clamp to [0, 1]
    uint value = (uint) (v * 16777215.0); // 24bit固定小数点
    float4 rgba;
    rgba.r = (value & 0xFF) / 255.0;
    rgba.g = ((value >> 8) & 0xFF) / 255.0;
    rgba.b = ((value >> 16) & 0xFF) / 255.0;
    rgba.a = 1.0; // 使用しない、または0埋め
    return rgba;
}

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_MODEL input)
{
    PS_OUTPUT output;
    float z = input.position_.z/input.position_.w;
    float depth = z;
    
    output.color0_ = EncodeFloatBilinear(depth); 
    return output;

}
