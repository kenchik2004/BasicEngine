//---------------------------------------------------------------------------
//! @file   Main.h
//! @brief  アプリケーションのエントリポイントおよびグローバルユーティリティ関数の宣言
//---------------------------------------------------------------------------
#pragma once

//! @file Main.h
//! @brief �A�v���P�[�V�����̃��C���w�b�_�t�@�C��

//! @brief ��ʂ̉���
extern int SCREEN_W;
//! @brief ��ʂ̏c��
extern int SCREEN_H;

//! @brief �x���@����ʓx�@(���W�A��)�ւ̕ϊ�
//! @param degree �x���@�ł̊p�x
//! @return ���W�A���ł̊p�x
float TO_RADIAN(float degree);

//! @brief �ʓx�@(���W�A��)����x���@�ւ̕ϊ�
//! @param radian ���W�A���ł̊p�x
//! @return �x���@�ł̊p�x
float TO_DEGREE(float radian);

//! @brief XZ���ʏ��3D�̉~��`�悷��
//! @param center �~�̒��S���W
//! @param radius �~�̔��a
//! @param color �`��F
//! @param fill �h��Ԃ��t���O (true: �h��Ԃ�, false: �g�̂�)
void DrawCircle3D_XZ(float3 center, float radius, int color, bool fill = false);

//! @brief XZ���ʏ��3D�̋�`��`�悷��
//! @param center ��`�̒��S���W
//! @param half_w ��`�̉����̔���
//! @param half_h ��`�̏c���̔���
//! @param color �`��F
//! @param fill �h��Ԃ��t���O (true: �h��Ԃ�, false: �g�̂�)
void DrawBox3D_XZ(float3 center, float half_w, float half_h, int color, bool fill = false);

