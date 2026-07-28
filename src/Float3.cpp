#include "float3.h"


float3::float3()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

float3::float3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

float3::float3(VECTOR v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

float3::float3(Vector3 v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

void float3::clear()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

void float3::set(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void float3::set(float3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

float float3::GetLength()
{
	return sqrtf(x * x + y * y + z * z);
}

void float3::normalize()
{
	float length = GetLength();
	if (length > 0.0f) {
		x = x / length;
		y = y / length;
		z = z / length;
	}
}
float3 float3::normalized()
{
	float3 vec = *this;
	float length = vec.GetLength();
	if (length > 0.0f) {
		vec.x = vec.x / length;
		vec.y = vec.y / length;
		vec.z = vec.z / length;
	}
	return vec;
}

void float3::SetLength(float length)
{
	normalize();
	x = x * length;
	y = y * length;
	z = z * length;
}

float3& float3::operator = (const float3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

float3& float3::operator=(const VECTOR& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

float3& float3::operator=(const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

float3& float3::operator += (const float3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

float3& float3::operator -= (const float3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

float3& float3::operator *= (const float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

float3& float3::operator /= (const float f)
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

float3 operator + (const float3& v1, const float3& v2)
{
	float3 v;
	v.x = v1.x + v2.x;
	v.y = v1.y + v2.y;
	v.z = v1.z + v2.z;
	return v;
}

float3 operator - (const float3& v1, const float3& v2)
{
	float3 v;
	v.x = v1.x - v2.x;
	v.y = v1.y - v2.y;
	v.z = v1.z - v2.z;
	return v;
}
float3 operator * (const float3& v, const float f)
{
	float3 r;
	r.x = v.x * f;
	r.y = v.y * f;
	r.z = v.z * f;
	return r;
}
float3 operator / (const float3& v, const float f)
{
	float3 r;
	r.x = v.x / f;
	r.y = v.y / f;
	r.z = v.z / f;
	return r;
}

float GetFloat3Distance(float3& pos1, float3& pos2)
{
	float3 v = pos1 - pos2;

	return v.GetLength();
}


float GetFloat3Dot(float3& v1, float3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float3 GetFloat3Cross(float3& v1, float3& v2)
{
	float3 v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}
float3 GetFloat3VTransform(float3& v, MATRIX& mat)
{

	VECTOR base = v;

	VECTOR change = VTransform(base, mat);

	float3 ret(change);
	return ret;
}

float3 ProjectOnPlane(float3 vec, float3 plane_normal)
{
	plane_normal.normalize();

	if (plane_normal.GetLength() <= FLT_EPSILON)
		return float3();
	
	vec -= plane_normal * GetFloat3Dot(plane_normal, vec);
	return vec;
}
