#pragma once
#include "System/Component.h"



namespace CreateMatrix {

	//---------------------------------------------------------------------------
	//!  [左手座標系] ビュー行列
	//---------------------------------------------------------------------------
	mat4x4 lookAtLH(const Vector3& eye, const Vector3& lookAt, const Vector3& worldUp);

	//---------------------------------------------------------------------------
	//! [左手座標系] 平行投影行列
	//---------------------------------------------------------------------------
	mat4x4 orthographicOffCenterLH(f32 left, f32 right, f32 bottom, f32 top, f32 near_z, f32 far_z);

	//---------------------------------------------------------------------------
	//! [左手座標系] 投影行列
	//---------------------------------------------------------------------------
	mat4x4 perspectiveFovLH(f32 fovy, f32 aspect_ratio, f32 near_z, f32 far_z);
}

USING_PTR(Camera);
class Camera :
	public Component
{
private:
	bool is_current_camera = false;
	int constant_buffer_handle = -1;
public:
	void Construct() override;
	int Init() override;
	void PreDraw() override;
	void LateDraw() override;
	virtual void PrepareCamera();
	void SetCameraConstantBuffer();
	void SetCurrentCamera();
	void Exit() override;
	inline void SetPerspective(float perspective_) { perspective = perspective_; SetupCamera_Perspective(DEG2RAD(perspective)); }
	inline float GetPerspective() { return perspective; }
	SafeSharedPtr<Camera> GetCurrentCamera();
	SafeSharedPtr<Texture> my_screen = nullptr;
	SafeSharedPtr<Texture> my_screen_depth = nullptr;

	SafeSharedPtr<Texture> gbuffer0 = nullptr;  //!< Gバッファ0 (法線・スペキュラ強度)
	SafeSharedPtr<Texture> gbuffer1 = nullptr;  //!< Gバッファ0 (法線・スペキュラ強度)
	SafeSharedPtr<Texture> gbuffer2 = nullptr;  //!< Gバッファ0 (法線・スペキュラ強度)

	float camera_near = 0.1f;
	float camera_far = 2000.0f;
	float perspective = 45.0f;
};

#if 0
class DebugCamera :
	public Camera
{
public:
	void Construct() override;
	int Init() override;
	void Update() override;
	void PreDraw() override;
	void PrepareCamera() override;
	void Exit() override;
};
#endif