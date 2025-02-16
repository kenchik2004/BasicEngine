#include "precompile.h"
#include "Transform.h"

float3 Transform::AxisX()
{
	float3 vec(1, 0, 0);
	MATRIX mat = MGetRotX(rotation.x);
	mat = MMult(mat, MGetRotY(rotation.y));
	mat = MMult(mat, MGetRotZ(rotation.z));
	vec.VSet(VTransform(vec.VGet(), mat));
	return vec;
}

float3 Transform::AxisY()
{
	float3 vec(0, 1, 0);
	MATRIX mat = MGetRotX(rotation.x);
	mat = MMult(mat, MGetRotY(rotation.y));
	mat = MMult(mat, MGetRotZ(rotation.z));
	vec.VSet(VTransform(vec.VGet(), mat));
	return vec;
}

float3 Transform::AxisZ()
{
	float3 x = AxisX();
	float3 y = AxisY();
	return GetFloat3Cross(x, y);
}
