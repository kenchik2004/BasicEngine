//----------------------------------------------------------------------------
//!	@file	vs_model.fx
//!	@brief	DxLib MV1���f�����_�V�F�[�_�[
//----------------------------------------------------------------------------
#include "dxlib_vs_model.h.fx"

// ���_�V�F�[�_�[�̏o��
struct VS_OUTPUT_MODEL
{
    float4 position_ : SV_Position; //!< ���W       (�X�N���[�����)
    float4 curr_position_ : CURR_POSITION; //!< ���݂̍��W (�X�N���[�����)
    float3 world_position_ : WORLD_POSITION; //!< ���[���h���W
    float3 normal_ : NORMAL0; //!< �@��
    float4 diffuse_ : COLOR0; //!< Diffuse�J���[
    float2 uv0_ : TEXCOORD0; //!< �e�N�X�`�����W

};

//----------------------------------------------------------------------------
//	���C���֐�
//----------------------------------------------------------------------------
VS_OUTPUT_MODEL main(VS_INPUT_MV1 input)
{
    VS_OUTPUT_MODEL output;

	//----------------------------------------------------------
	// ���_���W�ϊ�
	//----------------------------------------------------------
    float3 localPosition = input.position_;
    float3x4 matWorld = DxLib_WorldMatrix(input);

    float3 worldPosition = mul(matWorld, float4(localPosition, 1.0)); // �X�L�j���O�v�Z�B���[���h��Ԃ֕ϊ�
    float3 viewPosition = mul(DxLib_ViewMatrix(), float4(worldPosition, 1.0)); // �r���[��Ԃ֕ϊ�
    output.position_ = mul(DxLib_ProjectionMatrix(), float4(viewPosition, 1.0)); // �X�N���[����Ԃ֕ϊ�

    output.world_position_ = worldPosition;

	//------------------------------
	// ���݂̍��W��ʃp�����[�^�[�ŏo��
	// (SV_Position�̓s�N�Z���V�F�[�_�[�ł͒l�̈Ӗ����ω����邽��)
    output.curr_position_ = output.position_;

	//----------------------------------------------------------
	// �o�̓p�����[�^
	//----------------------------------------------------------
    output.normal_ = mul(matWorld, float4(input.normal_, 0.0)); // �@�������[���h��Ԃ֕ϊ�
    output.diffuse_ = input.diffuse_; // Diffuse�J���[
    output.uv0_ = input.uv0_.xy; // �e�N�X�`�����W
  	
	//----------------------------------------------------------
	// �o�̓p�����[�^��Ԃ�
	//----------------------------------------------------------
    return output;
}
