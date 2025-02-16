#pragma once
#include "System/Component.h"
class Transform :
	public Component
{
public:
	USING_SUPER(Transform);

	float3 position;
	float3 rotation;
	float3 scale;
	inline void MovePosition(const float3& dir) { position += dir * Time::DeltaTime(); }
	inline void SetPosition(const float3& pos) { position = pos; }
	float3 AxisX();
	float3 AxisY();
	float3 AxisZ();
};

