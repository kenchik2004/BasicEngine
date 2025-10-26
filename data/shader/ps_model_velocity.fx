//----------------------------------------------------------------------------
//!	@file	ps_model_velocity.fx
//!	@brief	���x�o�b�t�@�𐶐� �s�N�Z���V�F�[�_�[
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"

// ���_�V�F�[�_�[�̏o��
struct VS_OUTPUT_MODEL
{
	float4	position_       : SV_Position;      //!< ���W       (�X�N���[�����)
    float4  curr_position_  : CURR_POSITION;    //!< ���݂̍��W (�X�N���[�����)
	float3	world_position_ : WORLD_POSITION;   //!< ���[���h���W
	float3	normal_         : NORMAL0;          //!< �@��
	float4	diffuse_        : COLOR0;           //!< Diffuse�J���[
	float2	uv0_            : TEXCOORD0;        //!< �e�N�X�`�����W
    float4  prev_position_  : PREV_POSITION;    //!< 1�t���[���O�̍��W (�X�N���[�����) �������ɒǉ�����Ă��邽�ߒ���
};

typedef	VS_OUTPUT_MODEL	PS_INPUT_MODEL;

//----------------------------------------------------------------------------
// ���C���֐�
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_MODEL input)
{
	PS_OUTPUT	output;

	//----------------------------------------------------------
	// �X�N���[�����W���v�Z���邽�߂�W�ŏ��Z
	// -1�`+1 �̃X�N���[����Ԃ̍��W�����߂�
	//----------------------------------------------------------
	float2	curr_position = input.curr_position_.xy / input.curr_position_.w;
	float2	prev_position = input.prev_position_.xy / input.prev_position_.w;

	// UV��Ԃ̈ړ��ʂ��v�Z
	float2	velocity = (curr_position - prev_position) * float2(0.5, -0.5);

	// �J���[�ŏc���̑��x�����ɒ��F���ďo��
	output.color0_.rg = velocity * 100;
	output.color0_.b  = 0.0;
	output.color0_.a  = 1.0;

	// �o�̓p�����[�^��Ԃ�
	return output;
}
