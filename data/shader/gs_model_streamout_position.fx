//----------------------------------------------------------------------------
//!	@file	gs_model_streamout_position.fx
//!	@brief	���_���W StreamOut �W�I���g�����_�V�F�[�_�[
//----------------------------------------------------------------------------

//---------------------------------------------------------------
// ���_�V�F�[�_�[�o��(�W�I���g���V�F�[�_�[����)
//---------------------------------------------------------------
struct VS_OUTPUT_MODEL
{
	float4	position_       : SV_Position;		//!< ���W       (�X�N���[�����)
    float4  curr_position_  : CURR_POSITION;    //!< ���݂̍��W (�X�N���[�����)
    float4  prev_position_  : PREV_POSITION;    //!< 1�t���[���O�̍��W (�X�N���[�����)
	float3	world_position_ : WORLD_POSITION;	//!< ���[���h���W
	float3	normal_         : NORMAL0;			//!< �@��
	float4	diffuse_        : COLOR0;			//!< Diffuse�J���[
	float2	uv0_            : TEXCOORD0;		//!< �e�N�X�`�����W
};

struct GS_STREAMOUT
{
	float4 position_ : POSITION;
};

//----------------------------------------------------------------------------
//	���C���֐�
//----------------------------------------------------------------------------
[maxvertexcount(3)]
void main(triangle VS_OUTPUT_MODEL input[3], inout TriangleStream<GS_STREAMOUT> outputStream)
{
	GS_STREAMOUT	output;

	output.position_ = input[0].curr_position_;
	outputStream.Append(output);

	output.position_ = input[1].curr_position_;
	outputStream.Append(output);

	output.position_ = input[2].curr_position_;
	outputStream.Append(output);
}
