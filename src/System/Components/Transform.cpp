#include "precompile.h"
#include "Transform.h"

float3 Transform::AxisX()
{
	float3 vec(1, 0, 0);
	MATRIX mat = MGetRotX(rotation.x);
	mat = MMult(mat, MGetRotY(rotation.y));
	mat = MMult(mat, MGetRotZ(rotation.z));
	vec = VTransform(vec, mat);
	return vec;
}

float3 Transform::AxisY()
{
	float3 vec(0, 1, 0);
	MATRIX mat = MGetRotX(rotation.x);
	mat = MMult(mat, MGetRotY(rotation.y));
	mat = MMult(mat, MGetRotZ(rotation.z));
	vec = VTransform(vec, mat);
	return vec;
}

float3 Transform::AxisZ()
{
	float3 x = AxisX();
	float3 y = AxisY();
	return GetFloat3Cross(x, y);
}

void Transform::SetAxisX(float3 v_x)
{
}

void Transform::SetAxisY(float3 v_y)
{
}

void Transform::SetAxisZ(float3 v_z)
{
}
