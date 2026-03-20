#include "dxlib_ps.h.fx"

cbuffer FilterInfo : register(b9) {
	float2 pixel_offset;
}

float4 GaussianBlur(Texture2D texture_, SamplerState samplerState, float2 centerUV, float2 pixelOffset)
{
	//Kernel 7x7
	const int stepCount = 2;
	const float gWeights[stepCount] = { 0.44908,0.05092 };
	const float gOffsets[stepCount] = { 0.53805,2.06278 };

	float4 color = float4(0, 0, 0, 1);

	[unroll]
		for (int i = 0; i < stepCount; i++) {
			float2 offset = gOffsets[i] * pixelOffset;
			float4 c = texture_.Sample(samplerState, centerUV + offset) +
				texture_.Sample(samplerState, centerUV - offset);
			color += gWeights[i] * c;
		}
	//color.a = 1.0;
	return color;
}

PS_OUTPUT main(PS_INPUT input) {
	float2 pixeloffset = pixel_offset;
	PS_OUTPUT output;
	output.color0_ = GaussianBlur(DiffuseTexture, DiffuseSampler, input.uv0_, pixeloffset);
	return output;
}