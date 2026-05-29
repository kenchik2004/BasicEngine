//----------------------------------------------------------------------------
//!	@file	gbuffer.h.fx
//!	@brief	GBuffer関連
//----------------------------------------------------------------------------
#ifndef GBUFFER_H_FX
#define GBUFFER_H_FX
#define FLT_MAX 3.402823466e+38F


//実は、OctahedronNormalEncondingの改良に成功したのだが、
//現在の実装でも十分な精度が実現できているため、実装は保留している。
//必要があればコメントアウトを外すだけで切り替えが可能にしてある。
//その際は、System/Components/Camera.cppのコメントアウトも外す必要がある。
//#define SIGNED_OCTAHEDRON_NORMAL_VECTOR_ENCODING

//--------------------------------------------------------------
// Gbuffer
// フォーマットについては cpp側の gbuffer.h を参照
//--------------------------------------------------------------
Texture2D GBuffer0 : register(t7);
Texture2D GBuffer1 : register(t8);
Texture2D GBuffer2 : register(t9);
Texture2D DepthBuffer : register(t10);

// 読み込まれたサーフェス情報
struct SurfaceInfo
{
    float3 albedo_;
    float ao_;
    float3 normal_;
    float roughness_;
    float metallic_;
    float emissive_;
    float3 world_position_;
    float depth_;
};
// ピクセルシェーダーの出力構造体 (MRT用)
struct PS_OUTPUT_MRT
{
    float4 color0_ : SV_Target0;
    float4 color1_ : SV_Target1;
    float4 color2_ : SV_Target2;
};

// -----------------------------
// Encode 0~1のfloat2をfloat1にパック
// -----------------------------
float Float2Encode(float2 v)
{
	// 0~15に丸める
    uint x = (uint) (saturate(v.x) * 15.0 + 0.5);
    uint y = (uint) (saturate(v.y) * 15.0 + 0.5);

	// 上位4bitに roughness、下位4bitに metallic
    uint packed = (x << 4) | (y & 0xF);

	// UNORMに格納する場合は 0~255 に変換
    return (float) packed / 255.0;
}

// -----------------------------
// Decode (逆変換)
// -----------------------------
float2 Float2Decode(float packed)
{
    uint p = (uint) (saturate(packed) * 255.0 + 0.5);

    uint x = (p >> 4) & 0xF;
    uint y = p & 0xF;

	// 0~1 に戻す
    float f_x = x / 15.0;
    float f_y = y / 15.0;

    return float2(f_x, f_y);
}




//===========================================================================
// Octahedron Normal Vector Encoding
//===========================================================================
#ifndef SIGNED_OCTAHEDRON_NORMAL_VECTOR_ENCODING
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
PS_OUTPUT_MRT PackSurfaceInfo(float3 albedo_,float ao_,float3 normal_,float roughness_,float metallic_,float3 world_position_,float3 emissive_)
{
    PS_OUTPUT_MRT output;
	
    float emissive_strength = dot(emissive_.rgb, float3(0.299, 0.597, 0.114));
    emissive_strength = saturate(emissive_strength * (1.0 / 64.0));
    float metal_emissive = saturate(metallic_) * 0.5;
    metal_emissive = emissive_strength > 0.0001 ? (emissive_strength * 0.5 + 0.5) : metal_emissive;
	
    float2 normal_encoded = NormalEncode(normal_);
    output.color0_ = float4(albedo_, ao_);
    output.color1_ = float4(normal_encoded.xy, roughness_, 0);
    output.color2_ = float4(world_position_.xyz, metal_emissive);
	
    return output;
}
SurfaceInfo GetSurfaceInfo(int2 position)
{
	//----------------------------------------------------------
	// GBufferを読み込む
	//----------------------------------------------------------
	float4 gbuffer0 = GBuffer0.Load(int3(position, 0));
	float4 gbuffer1 = GBuffer1.Load(int3(position, 0));
	float4 gbuffer2 = GBuffer2.Load(int3(position, 0));
	float depth = DepthBuffer.Load(int3(position, 0)).r;
	float metal_emisive = gbuffer2.a;

	//----------------------------------------------------------
	// 構造体に値を抽出
	//----------------------------------------------------------
	SurfaceInfo s;

	s.albedo_ = pow(abs(gbuffer0.rgb), 2.2); // sRGB→リニア化
	s.ao_ = gbuffer0.a;
	s.normal_ = NormalDecode(gbuffer1.rg);
	s.roughness_ = gbuffer1.b;
	s.metallic_ = saturate(metal_emisive *(metal_emisive<0.5? 2.0:0));
	s.emissive_ = saturate(metal_emisive * 2.0 - 1.0) * 64.0*100;
	s.world_position_ = gbuffer2.rgb;
	s.depth_ = depth;

	return s;
}
#else 

//Octahedron Normal Vector Encodingの改善案
//
//(r:unsigned x,g:unsigned y,b:0,a:符号ビットが2つ入る)
//Octwrapを使わずに、符号ビットをアルファチャンネルに入れることで、より高速に、より高精細にエンコードできるようになる。	
float3 NormalEncode(float3 n)
{
    float3 OutN;
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
	
    OutN.y = n.y * 0.5 + 0.5;
    OutN.x = n.x * 0.5 + OutN.y;
    OutN.y = n.x * -0.5 + OutN.y;

    OutN.z = saturate(n.z * FLT_MAX);
    return OutN;
}
float3 NormalDecode(float3 f)
{
    float3 OutN;

    OutN.x = (f.x - f.y);
    OutN.y = (f.x + f.y) - 1.0;
    OutN.z = f.z * 2.0 - 1.0;
    OutN.z = OutN.z * (1.0 - abs(OutN.x) - abs(OutN.y));
 
    OutN = normalize(OutN);
    return OutN;
}

PS_OUTPUT_MRT PackSurfaceInfo(float3 albedo_, float ao_, float3 normal_, float roughness_, float metallic_, float3 world_position_, float3 emissive_)
{
    PS_OUTPUT_MRT output;
	
    float emissive_strength = dot(emissive_.rgb, float3(0.299, 0.597, 0.114));
    emissive_strength = saturate(emissive_strength * (1.0 / 64.0));
    float metal_emissive = saturate(metallic_) * 0.5;
    metal_emissive = emissive_strength > 0.0001 ? (emissive_strength * 0.5 + 0.5) : metal_emissive;
	
    float3 normal_encoded = NormalEncode(normal_);
    output.color0_ = float4(albedo_, ao_);
    output.color1_ = float4(normal_encoded.xy, roughness_, normal_encoded.z);
    output.color2_ = float4(world_position_.xyz, metal_emissive);
	
    return output;
}
SurfaceInfo GetSurfaceInfo(int2 position)
{
	//----------------------------------------------------------
	// GBufferを読み込む
	//----------------------------------------------------------
    float4 gbuffer0 = GBuffer0.Load(int3(position, 0));
    float4 gbuffer1 = GBuffer1.Load(int3(position, 0));
    float4 gbuffer2 = GBuffer2.Load(int3(position, 0));
    float depth = DepthBuffer.Load(int3(position, 0)).r;
    float metal_emissive = gbuffer2.a;

	//----------------------------------------------------------
	// 構造体に値を抽出
	//----------------------------------------------------------
    SurfaceInfo s;

    s.albedo_ = pow(abs(gbuffer0.rgb), 2.2); // sRGB→リニア化
    s.ao_ = gbuffer0.a;
    s.normal_ = NormalDecode(gbuffer1.rga);
    s.roughness_ = gbuffer1.b;
    s.metallic_ = saturate(metal_emissive * (metal_emissive < 0.5 ? 2.0 : 0));
    s.emissive_ = saturate(metal_emissive * 2.0 - 1.0) * 64.0 * 100;
    s.world_position_ = gbuffer2.rgb;
    s.depth_ = depth;

    return s;
}
#endif

#endif // GBUFFER_H_FX

