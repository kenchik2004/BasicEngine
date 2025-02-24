#pragma once
#include "System/Component.h"
class Transform :
	public Component
{
public:
	USING_SUPER(Transform);

	Vector3 position = Vector3(0, 0, 0);
	Quaternion rotation = Quaternion(physx::PxIdentity);
	Vector3 scale = Vector3(1, 1, 1);

	inline void MovePosition(const Vector3& dir) { position += dir * Time::DeltaTime(); }
	inline void SetPosition(const Vector3& pos) { position = pos; }
	void AddRotation(Vector3 euler_angles);
	void AddRotation(Quaternion q);
	void SetRotation(Vector3 euler_angles);
	void SetRotation(Quaternion q);
	Vector3 AxisX();
	Vector3 AxisY();
	Vector3 AxisZ();
	void SetAxisX(Vector3 target);
	void SetAxisY(Vector3 target);
	void SetAxisZ(Vector3 target);
};

