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
	//FPS�X�V
	void UpdateFPS();


	//����΂�(���Ԃ̏㏑���Ȃ̂ł��܂葽�p���Ȃ��ł�������)
	void ResetTime();


	//�������w��FPS�ɌŒ�E�Q�[���X�V��FPS���v��
	int FixFPS();
	//�`�揈���̌o�ߎ��Ԃ��������E�`��FPS���v��
	void FixDrawFPS();
	//���������̌o�ߎ��Ԃ��X�V�E����������FPS���v��
	void FixFixedFPS();


	//�^�C���X�P�[���̎擾(float)
	const float TimeScale();
	//�^�C���X�P�[���̎擾(double)
	const double TimeScaleD();

	//�^�C���X�P�[���̕ύX
	void SetTimeScale(const double scale);


	//�Q�[���������Ԃ̎擾(float)
	const float DeltaTime();
	//�Q�[���������Ԃ̎擾(double)
	const double DeltaTimeD();
	//�`��p�����Ԃ̎擾(float)
	const float DrawDeltaTime();
	//�`��p�����Ԃ̎擾(double)
	const double DrawDeltaTimeD();
	//�����p�����Ԃ̎擾(float)
	const float FixedDeltaTime();
	//�����p�����Ԃ̎擾(double)
	const double FixedDeltaTimeD();
	//�O�t���[���Ƃ̕����I���ԍ��̎擾(float)
	const float UnscaledDeltaTime();
	//�O�t���[���Ƃ̕����I���ԍ��̎擾(double)
	const double UnscaledDeltaTimeD();


	//�A�v���P�[�V�����J�n��̕����I���Ԃ̎擾(float)
	const float SystemTimeFromStart();
	//�A�v���P�[�V�����J�n��̕����I���Ԃ̎擾(double)
	const double SystemTimeFromStartD();
	//�A�v���P�[�V�����J�n��̃Q�[�������Ԃ̎擾
	const double GetTimeFromStart();

	//�V�X�e�������Ԃ̎擾(float)
	const float SystemTime();
	//�V�X�e�������Ԃ̎擾(double)
	const double SystemTimeD();
	//Windows����擾�������ݎ���(float)
	const float GetOSTime();
	//Windows����擾�������ݎ���(double)
	const double GetOSTimeD();


	//FPS�̍ő�l���擾(float)
	const float GetFPSMAX();
	//FPS�̍ő�l���擾(double)
	const double GetFPSMAXD();
	//�`��FPS�̍ő�l���擾(float)
	const float GetDrawFPSMAX();
	//�`��FPS�̍ő�l���擾(double)
	const double GetDrawFPSMAXD();

	//FPS�̍ő�l��ݒ�
	void SetFPSMAX(const double& max);
	//�����X�VFPS�̍ő�l��ݒ�
	void SetFixedFPSMAX(const double& max);
	//�`��FPS�̍ő�l��ݒ�
	void SetDrawFPSMAX(const double& max);


	//���������Ԃ̍ő�l�擾(float)
	const float GetFixedDeltaTimeMAX();
	//���������Ԃ̍ő�l�擾(double)
	const double GetFixedDeltaTimeMAXD();
	//�`�惢���Ԃ̍ő�l�擾(float)
	const float GetDrawDeltaTimeMAX();
	//�`�惢���Ԃ̍ő�l�擾(double)
	const double GetDrawDeltaTimeMAXD();
	//�����Ԃ̍ő�l�擾(float)
	const float GetDeltaTimeMAX();
	//�����Ԃ̍ő�l�擾(double)
	const double GetDeltaTimeMAXD();


	//FPS�̎擾(float)
	const float GetFPS();
	//FPS�̎擾(double)
	const double GetFPSD();
};

