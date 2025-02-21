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
	void ResetTime();
	void FixDrawFPS();
	void FixFixedFPS();
	const double GetTimeFromStart();
	//�A�v���P�[�V�����J�n��̃Q�[�������Ԃ̎擾
	const float TimeScale();	//�^�C���X�P�[���̎擾
	const double TimeScaleD();
	void SetTimeScale(const double scale);//�^�C���X�P�[���̕ύX
	const float DeltaTime();
	const double DeltaTimeD();
	const float DrawDeltaTime();
	const double DrawDeltaTimeD();
	const float FixedDeltaTime();
	//�O�t���[���Ƃ̃Q�[�������ԍ��̎擾
	const float UnscaledDeltaTime();
	const double FixedDeltaTimeD();
	//�O�t���[���Ƃ̎��ۂ̎��ԍ��̎擾
	const double UnscaledDeltaTimeD(); 
	const float SystemTimeFromStart(); //�A�v���P�[�V�����J�n��̕����I���Ԃ̎擾
	const double SystemTimeFromStartD(); 
	const float SystemTime();
	const double SystemTimeD();
	const float GetOSTime();
	const double GetOSTimeD();
	const float GetFPSMAX();
	const double GetFPSMAXD();
	const float GetDrawFPSMAX();
	const double GetDrawFPSMAXD();
	void SetFPSMAX(const double& max);
	void SetFixedFPSMAX(const double& max);
	void SetDrawFPSMAX(const double& max);
	const float GetFixedDeltaTimeMAX();
	const double GetFixedDeltaTimeMAXD();
	const float GetDrawDeltaTimeMAX();
	const double GetDrawDeltaTimeMAXD();
	const float GetDeltaTimeMAX();
	const double GetDeltaTimeMAXD();
	const float GetFPS();
	const double GetFPSD();
};

