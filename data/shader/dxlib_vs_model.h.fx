//---------------------------------------------------------------------------
//!	@file	dxlib_vs_model.h
//! @brief	DxLib�p MV1���f�����_�V�F�[�_�[��`
//---------------------------------------------------------------------------
#include "dxlib_vs.h.fx"

//============================================================================
// �V�F�[�_�[�o���G�[�V����
//============================================================================

// MV1���f���̒��_�^�C�v
#define DX_MV1_VERTEX_TYPE_1FRAME					(0)				// �P�t���[���̉e�����󂯂钸�_
#define DX_MV1_VERTEX_TYPE_4FRAME					(1)				// �P�`�S�t���[���̉e�����󂯂钸�_
#define DX_MV1_VERTEX_TYPE_8FRAME					(2)				// �T�`�W�t���[���̉e�����󂯂钸�_
#define DX_MV1_VERTEX_TYPE_FREE_FRAME				(3)				// �X�t���[���ȏ�̉e�����󂯂钸�_
#define DX_MV1_VERTEX_TYPE_NMAP_1FRAME				(4)				// �@���}�b�v�p�̏�񂪊܂܂��P�t���[���̉e�����󂯂钸�_
#define DX_MV1_VERTEX_TYPE_NMAP_4FRAME				(5)				// �@���}�b�v�p�̏�񂪊܂܂��P�`�S�t���[���̉e�����󂯂钸�_
#define DX_MV1_VERTEX_TYPE_NMAP_8FRAME				(6)				// �@���}�b�v�p�̏�񂪊܂܂��T�`�W�t���[���̉e�����󂯂钸�_
#define DX_MV1_VERTEX_TYPE_NMAP_FREE_FRAME			(7)				// �@���}�b�v�p�̏�񂪊܂܂��X�t���[���ȏ�̉e�����󂯂钸�_

// �y���ӎ����z
// ���_�^�C�v DX_MV1_VERTEX_TYPE_FREE_FRAME �� DX_MV1_VERTEX_TYPE_NMAP_FREE_FRAME ��
// CPU���ŃX�L�j���O�v�Z����Ă��邽�ߓ��͒��_���̂��̂��A�j���[�V�����ό`���Ă���̂Œ��ӂ��K�v�B

// �X�L�����b�V������
#if ((SHADER_VARIANT == DX_MV1_VERTEX_TYPE_4FRAME     ) || (SHADER_VARIANT == DX_MV1_VERTEX_TYPE_8FRAME) || \
	(SHADER_VARIANT == DX_MV1_VERTEX_TYPE_NMAP_4FRAME) || (SHADER_VARIANT == DX_MV1_VERTEX_TYPE_NMAP_8FRAME))
#define USE_SKINMESH
#endif

// �@���}�b�v����
#if (SHADER_VARIANT >= DX_MV1_VERTEX_TYPE_NMAP_1FRAME)
#define USE_NORMALMAP
#endif

// 8weight�X�L�j���O����
#if (SHADER_VARIANT == DX_MV1_VERTEX_TYPE_8FRAME) || (SHADER_VARIANT == DX_MV1_VERTEX_TYPE_NMAP_8FRAME)
#define USE_BONE8
#endif

// [DxLib] MV1���f���̒��_�V�F�[�_�[����
struct VS_INPUT_MV1
{
	float3 position_      : POSITION;		// ���W(���[�J�����)
	float3 normal_        : NORMAL;			// �@��(���[�J�����)
	float4 diffuse_       : COLOR0;			// �f�B�t���[�Y�J���[
	float4 specular_      : COLOR1;			// �X�y�L�����J���[
	float4 uv0_           : TEXCOORD0;		// �e�N�X�`�����W
	float4 uv1_           : TEXCOORD1;		// �T�u�e�N�X�`�����W

	// �@���}�b�v
#if defined(USE_NORMALMAP)
	float3 tangent_       : TANGENT0;		// �ڐ�  (���[�J�����)
	float3 binormal_      : BINORMAL0;		// �]�@��(���[�J�����)
#endif

// �X�L�j���O���b�V��
#if defined(USE_SKINMESH)
	int4   blendIndices0_ : BLENDINDICES0;	// �{�[�������p�z��C���f�b�N�X1
	float4 blendWeight0_  : BLENDWEIGHT0;	// �{�[�������p�E�G�C�g�l0

	// 8-weight �X�L�j���O
#if defined(USE_BONE8)
	int4   blendIndices1_ : BLENDINDICES1;	// �{�[�������p�z��C���f�b�N�X1
	float4 blendWeight1_  : BLENDWEIGHT1;	// �{�[�������p�E�G�C�g�l1
#endif
#endif // USE_SKINMESH
};

//---------------------------------------------------------------------------
// [DxLib] �r���[�s����擾
//---------------------------------------------------------------------------
float3x4 DxLib_ViewMatrix()
{
	return DxLib_Base.ViewMatrix;
}

//---------------------------------------------------------------------------
// [DxLib] ���e�s����擾
//---------------------------------------------------------------------------
float4x4 DxLib_ProjectionMatrix()
{
	return DxLib_Base.ProjectionMatrix;
}

//---------------------------------------------------------------------------
// [DxLib] �X�L�j���O�s����擾
//!	@param	[in]	blendIndex	�s��ԍ�
//! @attention DxLib�̒��_�`���ŃC���f�b�N�X��3�̔{���ɂȂ��Ă��邽�ߒ���
//---------------------------------------------------------------------------
float3x4	DxLib_getSkinMatrix(in int blendIndex)
{
	float4	m0 = DxLib_LocalWorldMatrix.Mat[blendIndex + 0];
	float4	m1 = DxLib_LocalWorldMatrix.Mat[blendIndex + 1];
	float4	m2 = DxLib_LocalWorldMatrix.Mat[blendIndex + 2];
	return float3x4(m0, m1, m2);
}

//---------------------------------------------------------------------------
// [DxLib] �X�L�j���O�v�Z���ꂽ���[���h�s����擾
//---------------------------------------------------------------------------
float3x4 DxLib_WorldMatrix(in VS_INPUT_MV1 input)
{
	float3x4	matWorld = DxLib_Base.WorldMatrix;

#if defined(USE_SKINMESH)	// �X�L�j���O���b�V��

	matWorld =  DxLib_getSkinMatrix(input.blendIndices0_.x) * input.blendWeight0_.x;
	matWorld += DxLib_getSkinMatrix(input.blendIndices0_.y) * input.blendWeight0_.y;
	matWorld += DxLib_getSkinMatrix(input.blendIndices0_.z) * input.blendWeight0_.z;
	matWorld += DxLib_getSkinMatrix(input.blendIndices0_.w) * input.blendWeight0_.w;

#if defined(USE_BONE8)	// 8-weight �X�L�j���O

	matWorld += DxLib_getSkinMatrix(input.blendIndices1_.x) * input.blendWeight1_.x;
	matWorld += DxLib_getSkinMatrix(input.blendIndices1_.y) * input.blendWeight1_.y;
	matWorld += DxLib_getSkinMatrix(input.blendIndices1_.z) * input.blendWeight1_.z;
	matWorld += DxLib_getSkinMatrix(input.blendIndices1_.w) * input.blendWeight1_.w;

#endif

#endif // USE_SKINMESH

	return matWorld;
}
