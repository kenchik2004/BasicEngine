//---------------------------------------------------------------------------
//! @file   Float2.h
//! @brief  2次元浮動小数点ベクトルクラス
//---------------------------------------------------------------------------
#pragma once

//---------------------------------------------------------------------------------
//	float2 �N���X
//---------------------------------------------------------------------------------
class float2
{
public:
	float x; //!< X成分
	float y; //!< Y成分

	float2();									//	�R���X�g���N�^
	float2(float x, float y);

	void clear();								//	�[��������
	void set(float x, float y);				//	�l�̃Z�b�g
	void set(float2& v);

	float GetLength();							//	�����擾
	void normalize();							//	���K��
	float2 normalized();							//	���K�����ꂽ�x�N�g���̎擾(�x�N�g�����̂ɕω��͉����Ȃ�)
	void SetLength(float length);				//	�����ݒ�

	float2& operator = (const float2& v);		//	= ���Z�q�̃I�[�o�[���[�h

	float2& operator += (const float2& v);	//	+= ���Z�q�̃I�[�o�[���[�h
	float2& operator -= (const float2& v);	//	-= ���Z�q�̃I�[�o�[���[�h
	float2& operator *= (const float f);		//	*= ���Z�q�̃I�[�o�[���[�h
	float2& operator /= (const float f);		//	/= ���Z�q�̃I�[�o�[���[�h
};

//	+ ���Z�q�̃I�[�o�[���[�h
float2 operator + (const float2& v1, const float2& v2);
//	- ���Z�q�̃I�[�o�[���[�h
float2 operator - (const float2& v1, const float2& v2);
//	* ���Z�q�̃I�[�o�[���[�h
float2 operator * (const float2& v, const float f);
//	/ ���Z�q�̃I�[�o�[���[�h
float2 operator / (const float2& v, const float f);

//	�Q�� float2 �̋��������߂�
float GetFloat2Distance(float2& pos1, float2& pos2);
//	�Q�� float2 �̓��ς����߂�
float GetFloat2Dot(float2& v1, float2& v2);
//	�Q�� float2 �̊O�ς����߂�
float GetFloat2Cross(float2& v1, float2& v2);
