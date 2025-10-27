#pragma once
#include "System/Object.h"
#include "System/Components/Camera.h"

class CameraObject :
    public GameObject
{
public:
	USING_SUPER(CameraObject);
	int Init() override;
	void Update() override;
	CameraWP camera; // カメラコンポーネント
};

#if 0
class DebugCameraObject :
	public GameObject
{
public:
	USING_SUPER(DebugCameraObject);
	int Init() override;
	void PostPhysics() override;
	DebugCameraWP camera; // カメラコンポーネント
	CameraWP main_camera; // メインカメラ
};
#endif
