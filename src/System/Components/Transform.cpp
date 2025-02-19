#include "precompile.h"
#include "Transform.h"

void Transform::AddRotation(Vector3 euler_angles)
{
	Quaternion qx(euler_angles.x, Vector3(1, 0, 0));
	Quaternion qy(euler_angles.y, Vector3(0, 1, 0));
	Quaternion qz(euler_angles.z, Vector3(0, 0, 1));
	rotation *= qx * qy * qz;

}

void Transform::SetRotation(Vector3 euler_angles)
{
	Quaternion qx(euler_angles.x, Vector3(1, 0, 0));
	Quaternion qy(euler_angles.y, Vector3(0, 1, 0));
	Quaternion qz(euler_angles.z, Vector3(0, 0, 1));
	rotation = qx * qy * qz;
}

Vector3 Transform::AxisX()
{
	float3 vec = rotation.getBasisVector0();
	return vec;
}

Vector3 Transform::AxisY()
{
	float3 vec = rotation.getBasisVector1();
	return vec;
}

Vector3 Transform::AxisZ()
{
	float3 vec = rotation.getBasisVector2();
	return vec;
}

