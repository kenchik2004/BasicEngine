//----------------------------------------------------------------------------
//!	@file	shadow.h.fx
//!	@brief	�V���h�E�֘A
//----------------------------------------------------------------------------
#ifndef SHADOW_H_FX
#define SHADOW_H_FX

#include "camera.h.fx"
#include "dxlib_ps.h.fx"

Texture2D ShadowmapTexture : register(t13);
// �V���h�E�f�v�X��r��p�T���v���[
SamplerComparisonState ShadowmapSampler : register(s13);


uint Xorshift(uint seed)
{
    uint x = seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

// 0.0�`1.0�͈̔͂Ń����_���Ȓl�𐶐�
float rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898f, 78.233f))) * 43758.5453f);
}

// 0.0�`1.0�͈̔͂Ń����_���Ȓl�𐶐�
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
    const float GOLDEN_ANGLE = 2.399963229728653; // �����p (������Ɋ�Â��p�x)
	
    float theta = GOLDEN_ANGLE * float(index) + angleBias;
#if 0
	// ���t�@�����X����
    float r = sqrt(float(index) + 0.5) / sqrt(float(numSamples));
    return float2(r * cos(theta), r * sin(theta));
#else
	// �œK����
    float r = sqrt((float(index) + 0.5) / float(numSamples));
    return sin(float2(theta + (PI / 2.0), theta)) * r;
#endif
}


float getShadow(float4 inputPosition, float3 worldPosition)
{
    //----------------------------------------------------------
   	// �J�X�P�[�h�ԍ���I������
    //----------------------------------------------------------
    int cascadeIndex = 0;
	{
        static const float split_distance[4] =
        {
            50.0f,
	   200.0f,
	   600.0f,
	   1200.0f
        };
		
        float distance = inputPosition.w; // �J��������̋���(m)
  
        if (split_distance[0] < distance)
            cascadeIndex++;
        if (split_distance[1] < distance)
            cascadeIndex++;
        if (split_distance[2] < distance)
            cascadeIndex++;
        if (split_distance[3] < distance)
            cascadeIndex++;
    }
    
    //----------------------------------------------------------
   	// �\�t�g�V���h�E�T���v�����O PCF(Percentage Closer Filtering)
    //----------------------------------------------------------
    float shadow = 1.0f; // �V���h�E�̏����l (1.0:�e�Ȃ�, 0.0:�e����)
	{
        float4 screenPosition = mul(float4(worldPosition, 1.0f), light_view_proj_[cascadeIndex]);
        screenPosition.xyz /= screenPosition.w; // ���K��

        float2 shadowUv = screenPosition.xy * float2(0.5f, -0.5f) + 0.5f; // [-1, 1] �� [0, 1] �̕ϊ�
 
		
        shadowUv.x = shadowUv.x * 0.25f + 0.25f * cascadeIndex;
		
		
        shadow = 0.0f;
       float thetaBias = InterleavedGradientNoise(inputPosition.xy) * (2.0f * PI);
        //float thetaBias = rand(input.position_.xy) * 2.0f * PI;
		
        const int SAMPLE_NUM = 16; // �T���v�����O�� 
        for (int i = 0; i < SAMPLE_NUM; ++i)
        {
            float2 offset = VogelDiskSample(i, SAMPLE_NUM, thetaBias) * (1.0 / 512.0);
		
			// �V���h�E�}�b�v�̐[�x�l�Ɣ�r
			// �V���h�E�o�C�A�X (�[�x�l�̃I�t�Z�b�g)
            static const float shadowBias[4] =
        {
            0.00025f,
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














#endif // SHADOW_H_FX

