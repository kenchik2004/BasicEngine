//----------------------------------------------------------------------------
//!	@file	ps_filter_fade.fx
//!	@brief	�t�F�[�h�t�B���^�[(���U�C�N�G�t�F�N�g��)
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"

// DxLib�ł͒萔�o�b�t�@��b4�ȍ~�����R�ɗ��p�ł���
cbuffer FilterFadeParameter : register(b4)
{
    float4	resolution_;	//!< �𑜓x [��, ����, 1.0f/��, 1.0f/����]
    float	alpha_;			//!< �t�F�[�h�A���t�@�l(0.0f-1.0f)
    uint	mosaicWidth_;	//!< ���U�C�N�̃s�N�Z����
};


//----------------------------------------------------------------------------
// ���C���֐�
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT	output;

	// �s�N�Z���ʒu
	uint2 position = uint2(input.position_.xy);
	
	//-----------------------------------------------------------
	// �y�ʃ��U�C�N���H
	//-----------------------------------------------------------
	position -= position % mosaicWidth_;
	position += mosaicWidth_ / 2;		// �u���b�N���Z���^�����O

	// �e�N�X�`���J���[�̓ǂݍ���
	float2	rcpResolution = resolution_.zw;		// �𑜓x�̋t��
	float2	uv = float2(position) * rcpResolution;
	float4	color = DiffuseTexture.Sample(DiffuseSampler, uv);

	// �t�F�[�h�A�E�g�ňÂ�����
	color.rgb *= alpha_;

	output.color0_ = color;

	// �o�̓p�����[�^��Ԃ�
	return output;
}
