
#pragma once


class float3
{
public:
	float	x;
	float	y;
	float	z;


	float3();
	float3(float x, float y, float z);
	float3(VECTOR v);
	float3(Vector3 v);
	void clear();
	void set(float x, float y, float z);
	void set(float3& v);


	float GetLength();
	void normalize();
	float3 normalized();
	void SetLength(float length);

	float3& operator = (const float3& v);
	float3& operator = (const VECTOR& v);
	float3& operator = (const Vector3& v);
	float3& operator += (const float3& v);
	float3& operator -= (const float3& v);
	float3& operator *= (const float f);
	float3& operator /= (const float f);

	operator VECTOR() { VECTOR v; v.x = this->x; v.y = this->y;	v.z = this->z;	return v; }
	operator Vector3() { Vector3 v; v.x = this->x; v.y = this->y;	v.z = this->z;	return v; }
};

float3 operator + (const float3& v1, const float3& v2);
float3 operator - (const float3& v1, const float3& v2);
float3 operator * (const float3& v, const float f);
float3 operator / (const float3& v, const float f);

float GetFloat3Distance(float3& pos1, float3& pos2);
float GetFloat3Dot(float3& v1, float3& v2);
float3 GetFloat3Cross(float3& v1, float3& v2);
float3 GetFloat3VTransform(float3& v, MATRIX& mat);
float3 ProjectOnPlane(float3 vec, float3 plane_normal);

inline float3 cast(const Vector3& vec) {
	float3 vec_ = vec;
	return vec_;
}

inline Vector3 operator/(const Vector3& base, const Vector3& denuminator)
{
	Vector3 f_;
	// 各成分について、分母が0に近いかを絶対値判定し、0でなければ逆数（1/分母）を求め、0なら0を入れる
	f_.x = physx::PxAbs(denuminator.x) > FLT_EPSILON ? 1.0f / denuminator.x : 0;
	f_.y = physx::PxAbs(denuminator.y) > FLT_EPSILON ? 1.0f / denuminator.y : 0;
	f_.z = physx::PxAbs(denuminator.z) > FLT_EPSILON ? 1.0f / denuminator.z : 0;
	// 逆数を掛け合わせることで、除算を高速に実現します。
	return Vector3(base.x * f_.x, base.y * f_.y, base.z * f_.z);
}

inline float3 cast(const VECTOR& vec) {
	float3 vec_ = vec;
	return vec_;
}

inline MATRIX cast(const mat4x4& mat)
{
	return {
		mat.column0.x, mat.column1.x, mat.column2.x, mat.column0.w,
		mat.column0.y, mat.column1.y, mat.column2.y, mat.column1.w,
		mat.column0.z, mat.column1.z, mat.column2.z, mat.column2.w,
		mat.column3.x, mat.column3.y, mat.column3.z, mat.column3.w
	};
}

inline mat4x4 cast(const MATRIX& mat)
{
	return mat4x4(
		Vector4(mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[0][3]),
		Vector4(mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[1][3]),
		Vector4(mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[2][3]),
		Vector4(mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3])
	);
}

inline Vector3 CastPhysXVec(const float3& v) {
	return Vector3(v.x, v.y, -v.z);
}

inline Quaternion CastPhysXQuat(const Quaternion& q) {
	return Quaternion(-q.x, -q.y, -q.z, q.w);
}

inline mat4x4 CastPhysXMat(const mat4x4& mat) {
	return mat4x4(
		Vector4(mat.column0.x, mat.column1.x, mat.column2.x, mat.column0.w).getNormalized(),
		Vector4(mat.column0.y, mat.column1.y, mat.column2.y, mat.column1.w).getNormalized(),
		Vector4(mat.column0.z, mat.column1.z, mat.column2.z, mat.column2.w).getNormalized(),
		Vector4(mat.column3.x, mat.column3.y, mat.column3.z, mat.column3.w)
	);
}

inline Vector3 QuaternionToEuler(const Quaternion& q) {
	float roll = RAD2DEG(physx::PxAtan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)));

	float pitch = RAD2DEG(physx::PxAsin(2.0f * (q.w * q.y - q.z * q.x)));

	float yaw = RAD2DEG(physx::PxAtan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)));

	return Vector3(roll, pitch, yaw);
}

inline Vector3 QuaternionToRadians(const Quaternion& q) {
	float roll = physx::PxAtan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y));
	float pitch = physx::PxAsin(2.0f * (q.w * q.y - q.z * q.x));
	float yaw = physx::PxAtan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));
	return Vector3(roll, pitch, yaw);
}

inline Quaternion Inverse(Quaternion q) {
	
	if (fabsf(q.magnitudeSquared()) < 1e-6f * 1e-6f)
		return Quaternion(0, 0, 0, 1);

	float invnorm = 1.0f / q.magnitudeSquared();
	return Quaternion(-q.x * invnorm, -q.y * invnorm, -q.z * invnorm, q.w * invnorm);
}

inline Vector3 Lerp(const Vector3& start, const  Vector3& end, const float& t) {
	Vector3 lerp_v;

	float t_ = physx::PxClamp(t, 0.0f, 1.0f);

	lerp_v.x = physx::PxLerp(start.x, end.x, t_);
	lerp_v.y = physx::PxLerp(start.y, end.y, t_);
	lerp_v.z = physx::PxLerp(start.z, end.z, t_);
	return lerp_v;
}

inline Vector3 LerpUnClamped(const Vector3& start, const  Vector3& end, const float& t) {
	Vector3 lerp_v;
	lerp_v.x = physx::PxLerp(start.x, end.x, t);
	lerp_v.y = physx::PxLerp(start.y, end.y, t);
	lerp_v.z = physx::PxLerp(start.z, end.z, t);
	return lerp_v;
}

inline Vector3 Slerp(const Vector3& from, const Vector3& to, float t)
{
	Vector3 v0 = from.getNormalized();
	Vector3 v1 = to.getNormalized();

	float dot = physx::PxClamp(v0.dot(v1), -1.0f, 1.0f);

	if (dot >= 1 - t)
		return v1;

	float theta = acosf(dot);

	float sinTheta = sinf(theta);

	float w1 = sinf((1.0f - t) * theta) / sinTheta;
	float w2 = sinf(t * theta) / sinTheta;

	Vector3 result = (v0 * w1) + (v1 * w2);

	return result.getNormalized();
}

inline Quaternion Slerp(const Quaternion& start, const Quaternion& end, const float& t) {
	Quaternion slerp_q;

	slerp_q = physx::PxSlerp(t, start, end);
	return slerp_q;
}

inline Quaternion EulerToQuaternion(const Vector3& euler) {

	Quaternion qx = Quaternion(DEG2RAD(euler.x), Vector3(1, 0, 0));
	Quaternion qy = Quaternion(DEG2RAD(euler.y), Vector3(0, 1, 0));
	Quaternion qz = Quaternion(DEG2RAD(euler.z), Vector3(0, 0, 1));

	return qx * qy * qz;
}
