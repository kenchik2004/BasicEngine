//----------------------------------------------------------------------------
//!	@file	camera.h.fx
//!	@brief	�J�����֘A
//----------------------------------------------------------------------------
#ifndef CAMERA_H_FX
#define CAMERA_H_FX

cbuffer CameraInfo : register(b10)
{
    matrix mat_view_;           //!< �r���[�s��
    matrix mat_proj_;           //!< ���e�s��
    float3 eye_position_;       //!< �J�����̈ʒu
    
    // �V���h�E�p
    matrix light_view_proj_[4]; //!< ���C�g�̃r���[���e�s��
};

#endif // CAMERA_H_FX

