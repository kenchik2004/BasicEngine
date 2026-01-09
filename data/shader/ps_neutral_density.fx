#include "dxlib_ps.h.fx"

PS_OUTPUT main(PS_INPUT input) {
	PS_OUTPUT output;
	output.color0_ = DiffuseTexture.Sample(DiffuseSampler, input.uv0_)-0.2;
	output.color0_.rgb *= 0.1;
	return output;
}