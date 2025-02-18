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

	void clear();								//	�[��������
	void set(float x, float y, float z);		//	�l�̃Z�b�g
	void set(float3& v);

	float GetLength();							//	�����擾
	void normalize();							//	���K��
	float3 normalized();						//	���K�����ꂽ�x�N�g���̎擾
	void SetLength(float length);				//	�����ݒ�

	float3& operator = (const float3& v);	//	= ���Z�q�̃I�[�o�[���[�h
	float3& operator = (const VECTOR& v);	//	= ���Z�q�̃I�[�o�[���[�h

	float3& operator += (const float3& v);	//	+= ���Z�q�̃I�[�o�[���[�h
	float3& operator -= (const float3& v);	//	-= ���Z�q�̃I�[�o�[���[�h
	float3& operator *= (const float f);		//	*= ���Z�q�̃I�[�o�[���[�h
	float3& operator /= (const float f);		//	/= ���Z�q�̃I�[�o�[���[�h
	operator VECTOR() { VECTOR v; v.x = this->x; v.y = this->y;	v.z = this->z;	return v; }
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