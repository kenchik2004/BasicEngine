#pragma once

//---------------------------------------------------------------------------------
//	float3 �N���X�i x y z ������N���X�j
//---------------------------------------------------------------------------------
class float3
{
public:
	float	x;
	float	y;
	float	z;

	float3();									//	�R���X�g���N�^
	float3(float x, float y, float z);
	float3(VECTOR v);
	float3(Vector3 v);

	void clear();								//	�[��������
	void set(float x, float y, float z);		//	�l�̃Z�b�g
	void set(float3& v);

	float GetLength();							//	�����擾
	void normalize();							//	���K��
	float3 normalized();						//	���K�����ꂽ�x�N�g���̎擾
	void SetLength(float length);				//	�����ݒ�

	float3& operator = (const float3& v);	//	= ���Z�q�̃I�[�o�[���[�h
	float3& operator = (const VECTOR& v);	//	= ���Z�q�̃I�[�o�[���[�h
	float3& operator = (const Vector3& v);	//	= ���Z�q�̃I�[�o�[���[�h

	float3& operator += (const float3& v);	//	+= ���Z�q�̃I�[�o�[���[�h
	float3& operator -= (const float3& v);	//	-= ���Z�q�̃I�[�o�[���[�h
	float3& operator *= (const float f);		//	*= ���Z�q�̃I�[�o�[���[�h
	float3& operator /= (const float f);		//	/= ���Z�q�̃I�[�o�[���[�h
	operator VECTOR() { VECTOR v; v.x = this->x; v.y = this->y;	v.z = this->z;	return v; }
	operator Vector3() { Vector3 v; v.x = this->x; v.y = this->y;	v.z = this->z;	return v; }
};

//	+ ���Z�q�̃I�[�o�[���[�h
float3 operator + (const float3& v1, const float3& v2);
//	- ���Z�q�̃I�[�o�[���[�h
float3 operator - (const float3& v1, const float3& v2);
//	* ���Z�q�̃I�[�o�[���[�h
float3 operator * (const float3& v, const float f);
//	/ ���Z�q�̃I�[�o�[���[�h
float3 operator / (const float3& v, const float f);

//	�Q�� float3 �̋��������߂�
float GetFloat3Distance(float3& pos1, float3& pos2);
//	�Q�� float3 �̓��ς����߂�
float GetFloat3Dot(float3& v1, float3& v2);
//	�Q�� float3 �̊O�ς����߂�
float3 GetFloat3Cross(float3& v1, float3& v2);
//	float3 �� MATRIX �ŕϊ��������̂�Ԃ�
float3 GetFloat3VTransform(float3& v, MATRIX& mat);
//	float3�̃x�N�g���𕽖ʏ�ɓ��e
float3 ProjectOnPlane(float3 vec, float3 plane_normal);
inline float3 cast(const Vector3& vec) {
	float3 vec_ = vec;
	return vec_;
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
		Vector4(mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[0][3]).getNormalized(),
		Vector4(mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[1][3]).getNormalized(),
		Vector4(mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[2][3]).getNormalized(),
		Vector4(mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3])
	);
}
// PhysX�ւ̎󂯓n���p�ϊ�
inline Vector3 CastPhysXVec(const float3& v) {
	return Vector3(v.x, v.y, -v.z);
}

// PhysX�ւ̎󂯓n���p�ϊ�
inline Quaternion CastPhysXQuat(const Quaternion& q) {
	return Quaternion(-q.x, -q.y, -q.z, q.w);
}
inline mat4x4 CastPhysXMat(const mat4x4& mat) {
	return mat4x4(
		Vector4(mat.column0.x, mat.column1.x, mat.column2.x, mat.column0.w),
		Vector4(mat.column0.y, mat.column1.y, mat.column2.y, mat.column1.w),
		Vector4(mat.column0.z, mat.column1.z, mat.column2.z, mat.column2.w),
		Vector4(mat.column3.x, mat.column3.y, mat.column3.z, mat.column3.w)
	);
}

inline Vector3 QuaternionToEuler(const Quaternion& q) {
	// Roll (X-axis rotation)
	float roll = atan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y));

	// Pitch (Y-axis rotation)
	float pitch = asin(2.0f * (q.w * q.y - q.z * q.x));

	// Yaw (Z-axis rotation)
	float yaw = atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));

	return Vector3(roll, pitch, yaw);  // X, Y, Z �ɑΉ�����I�C���[�p��Ԃ�
}
inline Quaternion Inverse(Quaternion q) {
	if (fabsf(q.magnitudeSquared()) < FLT_EPSILON)
		return Quaternion(0, 0, 0, 1);

	float invnorm = 1.0f / q.magnitudeSquared();
	return Quaternion(-q.x * invnorm, -q.y * invnorm, -q.z * invnorm, q.w * invnorm);

}