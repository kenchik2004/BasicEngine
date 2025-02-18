#pragma once
#include "System/Component.h"
class Transform :
	public Component
{
public:
	USING_SUPER(Transform);

	float3 position = { 0,0,0 };
	float3 rotation = { 0,0,0 };
	float3 scale = { 1,1,1 };
	inline void MovePosition(const float3& dir) { position += dir * Time::DeltaTime(); }
	inline void SetPosition(const float3& pos) { position = pos; }
	float3 AxisX();
	float3 AxisY();
	float3 AxisZ();
	void SetAxisX(float3 v_x);
	void SetAxisY(float3 v_y);
	void SetAxisZ(float3 v_z);
};

