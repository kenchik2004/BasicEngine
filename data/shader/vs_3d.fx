//----------------------------------------------------------------------------
//!	@file	vs_3d.fx
//!	@brief	3D�v���~�e�B�u���_�V�F�[�_�[
//----------------------------------------------------------------------------
#include "dxlib_vs.h.fx"

// ���_�V�F�[�_�[�̏o��
struct VS_OUTPUT_3D
{
	float4	position_      : SV_Position;		//!< ���W (�X�N���[�����)
	float3	worldPosition_ : WORLD_POSITION;	//!< ���[���h���W
	float3	normal_        : NORMAL0;			//!< �@��
	float4	diffuse_       : COLOR0;			//!< Diffuse�J���[
	float2	uv0_           : TEXCOORD0;			//!< �e�N�X�`�����W
};

//----------------------------------------------------------------------------
//	���C���֐�
//----------------------------------------------------------------------------
VS_OUTPUT_3D main(VS_INPUT_3D input)
{
	VS_OUTPUT_3D output;

	//----------------------------------------------------------
	// ���_���W�ϊ�
	//----------------------------------------------------------
	float3		localPosition = input.position_;
	float3x4	matWorld = DxLib_Base.WorldMatrix;

	float3	worldPosition = mul(matWorld              , float4(localPosition, 1.0));	// ���[���h��Ԃ֕ϊ�
	float3	viewPosition  = mul(DxLib_Base.ViewMatrix , float4(worldPosition, 1.0));	// �r���[��Ԃ֕ϊ�
	output.position_ = mul(DxLib_Base.ProjectionMatrix, float4(viewPosition , 1.0));	// �X�N���[����Ԃ֕ϊ�

	output.worldPosition_ = worldPosition;	// ���[���h���W

	//----------------------------------------------------------
	// �o�̓p�����[�^
	//----------------------------------------------------------
	output.normal_  = mul(matWorld, float4(input.normal_, 0.0));	// �@�������[���h��Ԃ֕ϊ�
	output.diffuse_ = input.diffuse_;								// Diffuse�J���[
	output.uv0_     = input.uv0_;									// �e�N�X�`�����W

	//----------------------------------------------------------
	// �o�̓p�����[�^��Ԃ�
	//----------------------------------------------------------
	return output;
}