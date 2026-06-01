//---------------------------------------------------------------------------
//! @file   CameraObject.cpp
//! @brief  CameraObjectの実装。カメラオブジェクトの処理を行う
//---------------------------------------------------------------------------
#include "CameraObject.h"
#include "System/Components/Camera.h"

int CameraObject::Init()
{
	auto camera_ = AddComponent<Camera>();
	SetPriority(0);
	camera = camera_;
	camera->camera_near = 0.1f;
	camera->camera_far = 2000.0f;
	camera->perspective = 30.0f;
	camera->SetCurrentCamera();
	transform->status.status_bit.off(CompStat::STATUS::DEBUG_DRAW);
	return 0;
}

void CameraObject::Update()
{
}

