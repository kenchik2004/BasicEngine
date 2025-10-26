//----------------------------------------------------------------------------
//!	@file	ps_tonemapping.fx
//!	@brief	HDR�g�[���}�b�s���O
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"

float3 ACESFilm(float3 x)
{
    x *= 0.6;
	
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

//----------------------------------------------------------------------------
// ���C���֐�
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT	output;

	// �e�N�X�`���J���[�̓ǂݍ���
	float4	color = DiffuseTexture.Sample(DiffuseSampler, input.uv0_);

	// �g�[���}�b�s���O�K�p
	color.rgb = ACESFilm(color.rgb);

	// sRGB�֕ϊ�
	color.rgb = pow(color.rgb, 1.0 / 2.2);

	output.color0_ = color;

	// �o�̓p�����[�^��Ԃ�
	return output;
}
