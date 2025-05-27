#include "precompile.h"
#include "Camera.h"
#include "System/Scene.h"

int Camera::Init()
{
	if (!GetCurrentCamera())
		SetCurrentCamera();
	return 0;
}

void Camera::PreDraw()
{
	auto owner_ = owner.lock();
	auto owner_trns = owner_->transform;
	if (is_current_camera)
	{
		SetCameraPositionAndTargetAndUpVec(cast(owner_trns->position), cast(owner_trns->position + owner_trns->AxisZ()), cast(owner_trns->AxisY()));
		SetCameraNearFar(camera_near, camera_far);
	}
}

void Camera::SetCurrentCamera()
{
	if (GetCurrentCamera())
		GetCurrentCamera()->is_current_camera = false;
	SceneManager::GetCurrentScene()->SetCurrentCamera(shared_from_this());
	is_current_camera = true;
	PreDraw();
	SetupCamera_Perspective(perspective);
}

SafeSharedPtr<Camera> Camera::GetCurrentCamera()
{
	return SafeStaticCast<Camera>(SceneManager::GetCurrentScene()->GetCurrentCamera().lock());
}
