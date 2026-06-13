//----------------------------------------------------------------------------
//!	@file	shadow.h.fx
//!	@brief	シャドウ関連
//----------------------------------------------------------------------------
#ifndef SHADOW_H_FX
#define SHADOW_H_FX

#include "camera.h.fx"
#include "dxlib_ps.h.fx"

Texture2D ShadowmapTexture : register(t13);
// シャドウデプス比較専用サンプラー
SamplerComparisonState ShadowmapSampler : register(s13);


uint Xorshift(uint seed)
{
    uint x = seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

// 0.0～1.0の範囲でランダムな値を生成
float rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898f, 78.233f))) * 43758.5453f);
}

// 0.0～1.0の範囲でランダムな値を生成
float InterleavedGradientNoise(float2 position_screen)
{
    float3 magic = float3(0.06711056f, 0.00583715f, 52.9829189f);
    return frac(magic.z * frac(dot(position_screen, magic.xy)));
}


//---------------------------------------------------------------------------
// Vogel disk sampling
//---------------------------------------------------------------------------
float2 VogelDiskSample(int index, int numSamples, float angleBias = 0.0f)
{
	// Vogel disk sampling
	// https://www.shadertoy.com/view/4l3yRM
	
	// 
    const float GOLDEN_ANGLE = 2.399963229728653; // 黄金角 (黄金比に基づく角度)
	
    float theta = GOLDEN_ANGLE * float(index) + angleBias;
#if 0
	// リファレンス実装
    float r = sqrt(float(index) + 0.5) / sqrt(float(numSamples));
    return float2(r * cos(theta), r * sin(theta));
#else
	// 最適化版
    float r = sqrt((float(index) + 0.5) / float(numSamples));
    return sin(float2(theta + (PI / 2.0), theta)) * r;
#endif
}

int GetCascadeIndex(float distance)
{
    static const float split_distance[4] =
    {
        50.0f,
       200.0f,
       600.0f,
       1200.0f
    };
    
    int cascadeIndex = 0;
    if (split_distance[0] < distance)
        cascadeIndex++;
    if (split_distance[1] < distance)
        cascadeIndex++;
    if (split_distance[2] < distance)
        cascadeIndex++;
    if (split_distance[3] < distance)
        cascadeIndex++;
    
    return cascadeIndex;
}

float getShadow(float4 inputPosition, float3 worldPosition)
{
    //----------------------------------------------------------
   	// カスケード番号を選択する
    //----------------------------------------------------------
    // カスケードシャドウマップを使用する場合、距離に応じて適切なカスケードを選択
    int cascadeIndex = GetCascadeIndex(inputPosition.w); // inputPosition.w はカメラからの距離
    
    //----------------------------------------------------------
   	// ソフトシャドウサンプリング PCF(Percentage Closer Filtering)
    //----------------------------------------------------------
    float shadow = 1.0f; // シャドウの初期値 (1.0:影なし, 0.0:影あり)
	{
        // ワールド座標をライトのスクリーン空間に投影
        float4 screenPosition = mul(float4(worldPosition, 1.0f), light_view_proj_[cascadeIndex]);
        screenPosition.xyz /= screenPosition.w; // 正規化

        float2 shadowUv = screenPosition.xy * float2(0.5f, -0.5f) + 0.5f; // [-1, 1] → [0, 1] の変換
 
		
        shadowUv.x = shadowUv.x * 0.25f + 0.25f * cascadeIndex;
        
		
		
        shadow = 0.0f;
        float thetaBias = InterleavedGradientNoise(inputPosition.xy) * (2.0f * PI);
        //float thetaBias = rand(input.position_.xy) * 2.0f * PI;
		
        const int SAMPLE_NUM = 16; // サンプリング数 
        [unroll]
        for (int i = 0; i < SAMPLE_NUM; ++i)
        {
            float2 offset = VogelDiskSample(i, SAMPLE_NUM, thetaBias) * (1.0 / 512.0);
		
			// シャドウマップの深度値と比較
			// シャドウバイアス (深度値のオフセット)
            static const float shadowBias[4] =
            {
                0.00002f,
	   0.0005f,
	   0.001f,
	   0.002f
            };

            shadow += ShadowmapTexture.SampleCmp(ShadowmapSampler, shadowUv + offset * float2(0.25f, 1.0f), screenPosition.z - shadowBias[cascadeIndex]);
        }
		
        shadow *= 1.0f / SAMPLE_NUM;
    }
//shadow=shadow*0.5+0.1;
    return cascadeIndex < 4 ? shadow : 1.0;
}
float GetShadowWithCascadeColor(float4 inputPosition, float3 worldPosition, out float3 cascadeColor)
{
    float shadow = getShadow(inputPosition, worldPosition);
    int cascadeIndex = GetCascadeIndex(inputPosition.w);
    static const float3 cascadeColors[4] =
    {
        float3(1.0f, 0.0f, 0.0f), // カスケード1: 赤
        float3(0.0f, 1.0f, 0.0f), // カスケード2: 緑
        float3(0.0f, 0.0f, 1.0f), // カスケード3: 青
        float3(1.0f, 1.0f, 0.0f) // カスケード4: 黄
    };
    // カスケードインデックスに応じた色を設定
    // 範囲外のインデックスの場合は白色を使用
    cascadeColor = cascadeIndex < 4 ? cascadeColors[cascadeIndex] : float3(1.0f, 1.0f, 1.0f);
    return shadow;
}













#endif // SHADOW_H_FX

