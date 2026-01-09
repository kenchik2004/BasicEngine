#include "dxlib_ps.h.fx"

Texture2D ReductionTexture0 : register(t30); // リダクションテクスチャ0
Texture2D ReductionTexture1 : register(t31); // リダクションテクスチャ1
Texture2D ReductionTexture2 : register(t32); // リダクションテクスチャ2
Texture2D ReductionTexture3 : register(t33); // リダクションテクスチャ3
Texture2D ReductionTexture4 : register(t34); // リダクションテクスチャ4

PS_OUTPUT main(PS_INPUT input) {
	PS_OUTPUT output = (PS_OUTPUT)0;
	output.color0_ = ReductionTexture0.Sample(DiffuseSampler, input.uv0_);
	output.color0_ += ReductionTexture1.Sample(DiffuseSampler, input.uv0_);
	output.color0_ += ReductionTexture2.Sample(DiffuseSampler, input.uv0_);
	output.color0_ += ReductionTexture3.Sample(DiffuseSampler, input.uv0_);
	output.color0_ += ReductionTexture4.Sample(DiffuseSampler, input.uv0_);
	

	return output;
}