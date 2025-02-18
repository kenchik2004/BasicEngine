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
	void FixDrawFPS();
	const double GetTimeFromStart(); //�A�v���P�[�V�����J�n��̃Q�[�������Ԃ̎擾
	const double TimeScale();	//�^�C���X�P�[���̎擾
	void SetTimeScale(const double scale);//�^�C���X�P�[���̕ύX
	const double DeltaTime();
	const double DrawDeltaTime();
	//�O�t���[���Ƃ̃Q�[�������ԍ��̎擾
	const double UnscaledDeltaTime(); //�O�t���[���Ƃ̎��ۂ̎��ԍ��̎擾
	const double SystemTimeFromStart(); //�A�v���P�[�V�����J�n��̕����I���Ԃ̎擾
	const double SystemTime();
	const double GetOSTime();
	const double GetFPSMAX();
	const double GetDrawFPSMAX();
	void SetFPSMAX(const double& max);
	void SetDrawFPSMAX(const double& max);
	const double GetDrawDeltaTimeMAX();
	const double GetDeltaTimeMAX();
	const double GetFPS();
};

