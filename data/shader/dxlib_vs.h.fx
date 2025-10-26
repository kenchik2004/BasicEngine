//---------------------------------------------------------------------------
//!	@file	dxlib_vs.h
//! @brief	DxLib�p���_�V�F�[�_�[��`
//---------------------------------------------------------------------------

//===========================================================================
// [DxLib] 3D�v���~�e�B�u�̒��_�V�F�[�_�[����
//===========================================================================
struct VS_INPUT_3D
{
	float3	position_  : POSITION0;		// ���W  (���[�J�����)
	float4	position1_ : POSITION1;		// �\�����W
	float3	normal_    : NORMAL0;		// �@��  (���[�J�����)
	float3	tangent_   : TANGENT0;		// �ڐ�  (���[�J�����)
	float3	binormal_  : BINORMAL0;		// �]�@��(���[�J�����)
	float4	diffuse_   : COLOR0;		// Diffuse�J���[
	float4	specular_  : COLOR1;		// Specular�J���[
	float2	uv0_       : TEXCOORD0;		// �e�N�X�`�����W
	float2	uv1_       : TEXCOORD1;		// �T�u�e�N�X�`�����W
};


// [DxLib] �}�e���A���p�����[�^
struct DX_D3D11_CONST_MATERIAL
{
	float4		Diffuse;				// �f�B�t���[�Y�J���[
	float4		Specular;				// �X�y�L�����J���[
	float4		Ambient_Emissive;		// �}�e���A���G�~�b�V�u�J���[ + �}�e���A���A���r�G���g�J���[ * �O���[�o���A���r�G���g�J���[

	float		Power;					// �X�y�L�����̋���
	float		TypeParam0;				// �}�e���A���^�C�v�p�����[�^0
	float		TypeParam1;				// �}�e���A���^�C�v�p�����[�^1
	float		TypeParam2;				// �}�e���A���^�C�v�p�����[�^2
};

// [DxLib] �t�H�O�p�����[�^
struct DX_D3D11_VS_CONST_FOG
{
	float		LinearAdd;				// �t�H�O�p�p�����[�^ end / ( end - start )
	float		LinearDiv;				// �t�H�O�p�p�����[�^ -1  / ( end - start )
	float		Density;				// �t�H�O�p�p�����[�^ density
	float		E;						// �t�H�O�p�p�����[�^ ���R�ΐ��̒�

	float4		Color;					// �J���[
};

// [DxLib] ���C�g�p�����[�^
struct DX_D3D11_CONST_LIGHT
{
	int			Type;					// ���C�g�^�C�v( DX_LIGHTTYPE_POINT �Ȃ� )
	int3		Padding1;				// �p�f�B���O�P

	float3		Position;				// ���W( �r���[��� )
	float		RangePow2;				// �L�������̂Q��

	float3		Direction;				// ����( �r���[��� )
	float		FallOff;				// �X�|�b�g���C�g�pFallOff

	float3		Diffuse;				// �f�B�t���[�Y�J���[
	float		SpotParam0;				// �X�|�b�g���C�g�p�p�����[�^�O( cos( Phi / 2.0f ) )

	float3		Specular;				// �X�y�L�����J���[
	float		SpotParam1;				// �X�|�b�g���C�g�p�p�����[�^�P( 1.0f / ( cos( Theta / 2.0f ) - cos( Phi / 2.0f ) ) )

	float4		Ambient;				// �A���r�G���g�J���[�ƃ}�e���A���̃A���r�G���g�J���[����Z��������

	float		Attenuation0;			// �����ɂ�錸�������p�p�����[�^�O
	float		Attenuation1;			// �����ɂ�錸�������p�p�����[�^�P
	float		Attenuation2;			// �����ɂ�錸�������p�p�����[�^�Q
	float		Padding2;				// �p�f�B���O�Q
};

// [DxLib] �s�N�Z���V�F�[�_�[�E���_�V�F�[�_�[���ʃp�����[�^
struct DX_D3D11_CONST_BUFFER_COMMON
{
	DX_D3D11_CONST_LIGHT	Light[6];	// ���C�g�p�����[�^
	DX_D3D11_CONST_MATERIAL	Material;	// �}�e���A���p�����[�^
	DX_D3D11_VS_CONST_FOG	Fog;		// �t�H�O�p�����[�^
};

// [DxLib] ��{�p�����[�^
struct DX_D3D11_VS_CONST_BUFFER_BASE
{
	float4x4	AntiViewportMatrix;		// �A���`�r���[�|�[�g�s��

	float4x4	ProjectionMatrix;		// ���e�s��
	float3x4	ViewMatrix;				// �r���[�s��
	float3x4	WorldMatrix;			// ���[���h�s��

	float4		ToonOutLineSize;		// �g�D�[���̗֊s���̑傫��
	float		DiffuseSource;			// �f�B�t���[�Y�J���[( 0.0f:�}�e���A��  1.0f:���_ )
	float		SpecularSource;			// �X�y�L�����J���[  ( 0.0f:�}�e���A��  1.0f:���_ )
	float		MulSpecularColor;		// �X�y�L�����J���[�l�ɏ�Z����l( �X�y�L�������������Ŏg�p )
	float		Padding;
};

// [DxLib] ���̑��̍s��
struct DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX
{
	float4		ShadowMapLightViewProjectionMatrix[3][4];	// �V���h�E�}�b�v�p�̃��C�g�r���[�s��ƃ��C�g�ˉe�s�����Z��������
	float4		TextureMatrix[3][2];						// �e�N�X�`�����W����p�s��
};

// [DxLib] �X�L�j���O���b�V���p�̃��[���h�s��
struct DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX {
	float4		Matrix[54 * 3];								// ���[���h�s��3x4
};

// �萔�o�b�t�@��DxLib��b0�`b3���g�p���Ă��܂��B

//===========================================================================
// �萔�o�b�t�@
//===========================================================================

// ���_�V�F�[�_�[�E�s�N�Z���V�F�[�_�[���ʃp�����[�^
cbuffer cbD3D11_CONST_BUFFER_COMMON : register(b0)
{
	DX_D3D11_CONST_BUFFER_COMMON	DxLib_Common;
};

// ��{�p�����[�^
cbuffer cbD3D11_CONST_BUFFER_VS_BASE : register(b1)
{
	DX_D3D11_VS_CONST_BUFFER_BASE	DxLib_Base;
};

// ���̑��̍s��
cbuffer cbD3D11_CONST_BUFFER_VS_OTHERMATRIX : register(b2)
{
	DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX	DxLib_OtherMatrix;
};

// �X�L�j���O���b�V���p�̃��[�J�����[���h�s��
cbuffer cbD3D11_CONST_BUFFER_VS_LOCALWORLDMATRIX : register(b3)
{
	DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX	DxLib_LocalWorldMatrix;
};
