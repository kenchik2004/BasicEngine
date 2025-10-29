//---------------------------------------------------------------------------
//!	@file	dxlib_ps.h
//! @brief	DxLib�p�s�N�Z���V�F�[�_�[��`
//---------------------------------------------------------------------------
#ifndef DXLIB_PS_H
#define DXLIB_PS_H
static const float PI = 3.14159265359f;

//===========================================================================
// �s�N�Z���V�F�[�_�[�̓���(2D)
//===========================================================================
struct PS_INPUT
{
    float4 position_ : SV_Position;
    float4 diffuse_ : COLOR0; // Diffuse�J���[

#if DXLIB_VERSION >= 0x324d	// [DxLib] Ver 3.24d����d�l���ύX����Ă���
	float4	specular_ : COLOR1;			// Specular�J���[
	float2	uv0_ : TEXCOORD0;
	float2	uv1_ : TEXCOORD1;
#else
    float2 uv0_ : TEXCOORD0;
#endif
};

//===========================================================================
// �s�N�Z���V�F�[�_�[�̓���(3D)
//===========================================================================
struct PS_INPUT_3D
{
    float4 position_ : SV_Position;
    float3 worldPosition_ : WORLD_POSITION; // ���[���h���W
    float3 normal_ : NORMAL0; // �@��
    float4 diffuse_ : COLOR0; // Diffuse�J���[
    float2 uv0_ : TEXCOORD0; // �e�N�X�`�����W
};

//===========================================================================
// �s�N�Z���V�F�[�_�[�̏o��
//===========================================================================
struct PS_OUTPUT
{
    float4 color0_ : SV_Target0; // �F
};


// [DxLib] �}�e���A���p�����[�^
struct DX_D3D11_CONST_MATERIAL
{
    float4 Diffuse; // �f�B�t���[�Y�J���[
    float4 Specular; // �X�y�L�����J���[
    float4 Ambient_Emissive; // �}�e���A���G�~�b�V�u�J���[ + �}�e���A���A���r�G���g�J���[ * �O���[�o���A���r�G���g�J���[

    float Power; // �X�y�L�����̋���
    float TypeParam0; // �}�e���A���^�C�v�p�����[�^0
    float TypeParam1; // �}�e���A���^�C�v�p�����[�^1
    float TypeParam2; // �}�e���A���^�C�v�p�����[�^2
};

// [DxLib] �t�H�O�p�����[�^
struct DX_D3D11_VS_CONST_FOG
{
    float LinearAdd; // �t�H�O�p�p�����[�^ end / ( end - start )
    float LinearDiv; // �t�H�O�p�p�����[�^ -1  / ( end - start )
    float Density; // �t�H�O�p�p�����[�^ density
    float E; // �t�H�O�p�p�����[�^ ���R�ΐ��̒�

    float4 Color; // �J���[
};

// [DxLib] ���C�g�p�����[�^
struct DX_D3D11_CONST_LIGHT
{
    int Type; // ���C�g�^�C�v( DX_LIGHTTYPE_POINT �Ȃ� )
    int3 Padding1; // �p�f�B���O�P

    float3 Position; // ���W( �r���[��� )
    float RangePow2; // �L�������̂Q��

    float3 Direction; // ����( �r���[��� )
    float FallOff; // �X�|�b�g���C�g�pFallOff

    float3 Diffuse; // �f�B�t���[�Y�J���[
    float SpotParam0; // �X�|�b�g���C�g�p�p�����[�^�O( cos( Phi / 2.0f ) )

    float3 Specular; // �X�y�L�����J���[
    float SpotParam1; // �X�|�b�g���C�g�p�p�����[�^�P( 1.0f / ( cos( Theta / 2.0f ) - cos( Phi / 2.0f ) ) )

    float4 Ambient; // �A���r�G���g�J���[�ƃ}�e���A���̃A���r�G���g�J���[����Z��������

    float Attenuation0; // �����ɂ�錸�������p�p�����[�^�O
    float Attenuation1; // �����ɂ�錸�������p�p�����[�^�P
    float Attenuation2; // �����ɂ�錸�������p�p�����[�^�Q
    float Padding2; // �p�f�B���O�Q
};

// [DxLib] �s�N�Z���V�F�[�_�[�E���_�V�F�[�_�[���ʃp�����[�^
struct DX_D3D11_CONST_BUFFER_COMMON
{
    DX_D3D11_CONST_LIGHT Light[6]; // ���C�g�p�����[�^
    DX_D3D11_CONST_MATERIAL Material; // �}�e���A���p�����[�^
    DX_D3D11_VS_CONST_FOG Fog; // �t�H�O�p�����[�^
};

// [DxLib] �萔�o�b�t�@�s�N�Z���V�F�[�_�[��{�p�����[�^
struct DX_D3D11_PS_CONST_BUFFER_BASE
{
    float4 factorColor_; // �A���t�@�l��

    float preMultipliedAlpha_; // �J���[�ɃA���t�@�l����Z���邩�ǂ���(0.0f:��Z���Ȃ�  1.0f:��Z����)
    float alphaTestRef_; // �A���t�@�e�X�g�Ŏg�p�����r�l
    float2 padding0_;

    int alphaTestCmpMode_; // �A���t�@�e�X�g��r���[�h(DX_CMP_NEVER �Ȃ�)
    int3 padding1_;

    float4 IgnoreTextureColor; // �e�N�X�`���J���[���������p�J���[
};
struct BE_PS_CONST_BUFFER_DEFAULT
{
    float time;
    float delta_time;
    float2 padding;
};

//===========================================================================
//	�萔�o�b�t�@
//===========================================================================

// ���_�V�F�[�_�[�E�s�N�Z���V�F�[�_�[���ʃp�����[�^
cbuffer cbD3D11_CONST_BUFFER_COMMON : register(b0)
{
    DX_D3D11_CONST_BUFFER_COMMON DxLib_Common;
};

// ��{�p�����[�^
cbuffer cbD3D11_CONST_BUFFER_PS_BASE : register(b1)
{
    DX_D3D11_PS_CONST_BUFFER_BASE DxLib_Base;
};

cbuffer cbBE_PS_CONST_BUFFER_DEFAULT : register(b4)
{
    BE_PS_CONST_BUFFER_DEFAULT BE_Default;
}

//===========================================================================
//	�e�N�X�`��
//===========================================================================
SamplerState DiffuseSampler : register(s0); // Diffuse�e�N�X�`���T���v���[
Texture2D DiffuseTexture : register(t0); // Diffuse�e�N�X�`��


SamplerState NormalSampler : register(s1); // �@���}�b�v�e�N�X�`��
Texture2D NormalTexture : register(t1); // �@���}�b�v�e�N�X�`��

SamplerState SpecularMapSampler : register(s2); // �X�y�L�����}�b�v�e�N�X�`��
Texture2D SpecularMapTexture : register(t2); // �X�y�L�����}�b�v�e�N�X�`��

// t3�`t6�̓g�D�[���p�e�N�X�`��

SamplerState SubSampler : register(s7); // �T�u�e�N�X�`��
Texture2D SubTexture : register(t7); // �T�u�e�N�X�`��

SamplerState ShadowMap0Sampler : register(s8); // �V���h�E�}�b�v�O�e�N�X�`��
Texture2D ShadowMap0Texture : register(t8); // �V���h�E�}�b�v�O�e�N�X�`��

SamplerState ShadowMap1Sampler : register(s9); // �V���h�E�}�b�v�P�e�N�X�`��
Texture2D ShadowMap1Texture : register(t9); // �V���h�E�}�b�v�P�e�N�X�`��

SamplerState ShadowMap2Sampler : register(s10); // �V���h�E�}�b�v�Q�e�N�X�`��
Texture2D ShadowMap2Texture : register(t10); // �V���h�E�}�b�v�Q�e�N�X�`��


//===========================================================================
//	PixelShader ���[�e�B���e�B�[
//===========================================================================

//----------------------------------------------------------------------------
//!	Tangent/Binormal��K�v�Ƃ��Ȃ��ڋ�ԕϊ��s��쐬
//!	@param	[in]	N	�@��
//!	@param	[in]	p	���[���h���W
//!	@param	[in]	uv	�e�N�X�`�����W
//----------------------------------------------------------------------------
float3x3 calcCotangentFrame(float3 N, float3 p, float2 uv)
{
	// �אڃs�N�Z���̌��z���擾
    float3 dp1 = ddx(p);
    float3 dp2 = ddy(p);
    float2 duv1 = ddx(uv);
    float2 duv2 = ddy(uv);

    float3 dp1_p = cross(N, dp1);
    float3 dp2_p = cross(dp2, N);

	// �A���ꎟ��������������Tangent��Binormal�����߂�
    float3 T = dp2_p * duv1.x + dp1_p * duv2.x;
    float3 B = dp2_p * duv1.y + dp1_p * duv2.y;

    float inv_max = rsqrt(max(dot(T, T), dot(B, B)));
    return float3x3(T * inv_max, B * inv_max, N);
}

//----------------------------------------------------------------------------
//!	�@���}�b�s���O��K�p���܂�
//!	@param	[in]	N	�@��
//!	@param	[in]	p	���[���h���W
//!	@param	[in]	uv	�e�N�X�`�����W
//----------------------------------------------------------------------------
float3 Normalmap(float3 N, float3 p, float2 uv)
{
	// �@���}�b�v�e�N�X�`����ǂݍ���
    float3 texture_normal = NormalTexture.Sample(NormalSampler, uv).rgb;
	// �f�R�[�h
    texture_normal = texture_normal * 2 - 1;

	// �^���W�F���g��Ԃ�ϊ�
    float3x3 TBN = calcCotangentFrame(N, p, uv);
    return normalize(mul(texture_normal, TBN));
}

#endif // DXLIB_PS_H
