#pragma once
#include "System/Component.h"
class Camera :
	public Component
{
public:
	int Init() override;
	void PreDraw() override;
	void SetCurrentCamera();
	inline void SetPerspective(float perspective_) { perspective = perspective_; SetupCamera_Perspective(DEG2RAD(perspective)); }
	inline float GetPerspective() { return perspective; }
	SafeSharedPtr<Camera> GetCurrentCamera();
	bool is_current_camera = false;
	float camera_near = 0.1f;
	float camera_far = 1000.0f;
	float perspective = 45.0f;
};

