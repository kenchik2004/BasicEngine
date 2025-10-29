//----------------------------------------------------------------------------
//!	@file	gbuffer.h.fx
//!	@brief	GBuffer�֘A
//----------------------------------------------------------------------------
#ifndef GBUFFER_H_FX
#define GBUFFER_H_FX

//--------------------------------------------------------------
// Gbuffer
// �t�H�[�}�b�g�ɂ��Ă� cpp���� gbuffer.h ���Q��
//--------------------------------------------------------------
Texture2D GBuffer0 : register(t7);
Texture2D GBuffer1 : register(t8);
Texture2D GBuffer2 : register(t9);
Texture2D DepthBuffer : register(t10);

// �ǂݍ��܂ꂽ�T�[�t�F�X���
struct SurfaceInfo
{
    float3 albedo_;
    float ao_;
    float3 normal_;
    float roughness_;
    float metallic_;
    float3 world_position_;
    float depth_;
};


//===========================================================================
// Octahedron Normal Vector Encoding
//===========================================================================
float2 NormalEncode(float3 n)
{
    n /= (abs(n.x) + abs(n.y) + abs(n.z));

    float2 octWrap = (1.0 - abs(n.yx)) * (n.xy >= 0.0 ? 1.0 : -1.0);

    n.xy = n.z >= 0.0 ? n.xy : octWrap;
    n.xy = n.xy * 0.5 + 0.5;
    return n.xy;
}
 
float3 NormalDecode(float2 f)
{
    f = f * 2.0 - 1.0;
 
    // https://twitter.com/Stubbesaurus/status/937994790553227264
    float3 n = float3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
    float t = saturate(-n.z);
    n.xy += n.xy >= 0.0 ? -t : t;
    return normalize(n);
}
// -----------------------------
// Encode 0~1��float2��float1�Ƀp�b�N
// -----------------------------
float Float2Encode(float2 v)
{
    // 0~15�Ɋۂ߂�
    uint x = (uint) (saturate(v.x) * 15.0 + 0.5);
    uint y = (uint) (saturate(v.y) * 15.0 + 0.5);

    // ���4bit�� roughness�A����4bit�� metallic
    uint packed = (x << 4) | (y & 0xF);

    // UNORM�Ɋi�[����ꍇ�� 0~255 �ɕϊ�
    return (float) packed / 255.0;
}

// -----------------------------
// Decode (�t�ϊ�)
// -----------------------------
float2 Float2Decode(float packed)
{
    uint p = (uint) (saturate(packed) * 255.0 + 0.5);

    uint x = (p >> 4) & 0xF;
    uint y = p & 0xF;

    // 0~1 �ɖ߂�
    float f_x = x / 15.0;
    float f_y = y / 15.0;

    return float2(f_x, f_y);
}
SurfaceInfo GetSurfaceInfo(int2 position)
{
    //----------------------------------------------------------
    // GBuffer��ǂݍ���
    //----------------------------------------------------------
    float4 gbuffer0 = GBuffer0.Load(int3(position, 0));
    float4 gbuffer1 = GBuffer1.Load(int3(position, 0));
    float4 gbuffer2 = GBuffer2.Load(int3(position, 0));
    float depth = DepthBuffer.Load(int3(position, 0)).r;
    float2 roughness_metallic = Float2Decode(gbuffer1.b);
    
    //----------------------------------------------------------
    // �\���̂ɒl�𒊏o
    //----------------------------------------------------------
    SurfaceInfo s;

    s.albedo_ = pow(abs(gbuffer0.rgb), 2.2); // sRGB�����j�A��
    s.ao_ = gbuffer0.a;
    s.normal_ = NormalDecode(gbuffer1.rg);
    s.roughness_ = gbuffer1.b;
    s.metallic_ = gbuffer1.a;
    s.world_position_ = gbuffer2.rgb;
    s.depth_ = depth;

    return s;
}

#endif // GBUFFER_H_FX

