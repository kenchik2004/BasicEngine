//----------------------------------------------------------------------------
//!	@file	gs_model_composite_prev_position.fx
//!	@brief	���_���W StreamOut �W�I���g�����_�V�F�[�_�[
//----------------------------------------------------------------------------

//---------------------------------------------------------------
// ���_�V�F�[�_�[�o��(�W�I���g���V�F�[�_�[����)
//---------------------------------------------------------------
struct VS_OUTPUT_MODEL
{
	float4	position_       : SV_Position;		//!< ���W       (�X�N���[�����)
    float4  curr_position_  : CURR_POSITION;    //!< ���݂̍��W (�X�N���[�����)
	float3	world_position_ : WORLD_POSITION;	//!< ���[���h���W
	float3	normal_         : NORMAL0;			//!< �@��
	float4	diffuse_        : COLOR0;			//!< Diffuse�J���[
	float2	uv0_            : TEXCOORD0;		//!< �e�N�X�`�����W
};

struct GS_OUTPUT_MODEL
{
	VS_OUTPUT_MODEL	vs_input;

    //! 1�t���[���O�̍��W (�X�N���[�����) ������𒸓_�ɍ���
    float4	prev_position_ : PREV_POSITION;
};


//----------------------------------------------------------------------------
// ByteAddressBuffer
//----------------------------------------------------------------------------
// 1�t���[���O�̍��W���
// float4 * ���_��
ByteAddressBuffer ScreenPosition_Prev : register(t0);

// 1�t���[���O�̍��W���擾
//!	@param	[in]	���_�ԍ�
float4 DxLib_ScreenPosition_Prev(int vertexIndex)
{
	return asfloat(ScreenPosition_Prev.Load4(vertexIndex * 16));
}

//----------------------------------------------------------------------------
//	���C���֐�
//----------------------------------------------------------------------------
[maxvertexcount(3)]
void main(triangle VS_OUTPUT_MODEL input[3], uint primitiveId: SV_PrimitiveID,  inout TriangleStream<GS_OUTPUT_MODEL> outputStream)
{
	GS_OUTPUT_MODEL	output;

	output.vs_input       = input[0];
	output.prev_position_ = DxLib_ScreenPosition_Prev(primitiveId * 3 + 0);
	outputStream.Append(output);

	output.vs_input = input[1];
	output.prev_position_ = DxLib_ScreenPosition_Prev(primitiveId * 3 + 1);
	outputStream.Append(output);

	output.vs_input = input[2];
	output.prev_position_ = DxLib_ScreenPosition_Prev(primitiveId * 3 + 2);
	outputStream.Append(output);
}
