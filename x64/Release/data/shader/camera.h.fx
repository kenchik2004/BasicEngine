//----------------------------------------------------------------------------
//!	@file	camera.h.fx
//!	@brief	カメラ関連
//----------------------------------------------------------------------------
#ifndef CAMERA_H_FX
#define CAMERA_H_FX

cbuffer CameraInfo : register(b10)
{
    matrix mat_view_;           //!< ビュー行列
    matrix mat_proj_; //!< 投影行列
    matrix mat_view_inv; //!< ビュー逆行列
    matrix mat_proj_inv; //!< 投影逆行列
    float3 eye_position_;       //!< カメラの位置
    
    // シャドウ用
    matrix light_view_proj_[4]; //!< ライトのビュー投影行列
};

#endif // CAMERA_H_FX

