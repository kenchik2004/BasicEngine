#pragma once
namespace Time
{

#define MICRO2SEC(micro_sec) micro_sec*0.000001
#define SEC2MICRO(sec) sec*1000000
#define MILLI2SEC(micro_sec) micro_sec*0.001
#define SEC2MILLI(sec) sec*1000


	//������
	int Init();
	//�X�V
	void Update();
	void UpdateFPS();
	int FixFPS();
	const double GetTimeFromStart(); //�A�v���P�[�V�����J�n��̃Q�[�������Ԃ̎擾
	const double TimeScale();	//�^�C���X�P�[���̎擾
	void SetTimeScale(const double scale);//�^�C���X�P�[���̕ύX
	const double DeltaTime();//�O�t���[���Ƃ̃Q�[�������ԍ��̎擾
	const double UnscaledDeltaTime(); //�O�t���[���Ƃ̎��ۂ̎��ԍ��̎擾
	const double SystemTimeFromStart(); //�A�v���P�[�V�����J�n��̕����I���Ԃ̎擾
	const double SystemTime();
	const double GetOSTime();
	const double GetFPSMAX();
	void SetFPSMAX(const double& max);
	const double GetFPS();
};

