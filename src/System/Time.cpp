#include "precompile.h"
#include "Time.h"

namespace Time {
	//�V�X�e���p�̎��ԊǗ��p�ϐ�(ulonglong�Ǘ�)
	unsigned long long sys_time; //!<�V�X�e����̎���(�������E�̎��Ԃ��Q��)
	unsigned long long sys_time_start; //!<�V�X�e����̃A�v���P�[�V�����J�n����
	unsigned long long sys_time_prev; //!<�V�X�e����̑O�t���[���̎���
	unsigned long long real_sys_time_prev; //!<�������V�X�e����̎���(�������E�̎��Ԃ��Q��)

	//�O���Ŏg�p���邽�߂̎��ԊǗ��p�ϐ�(double[�b]�Ǘ�)
	double time;//!<�A�v���P�[�V�����J�n��̌o�ߎ���(�Q�[��������)
	double real_time;//!<�A�v���P�[�V�����J�n��̌o�ߎ���(��������)
	double delta_time; //!<�O�t���[���Ƃ̎��ԍ� (��)
	double real_delta_time; //!<�O�t���[���Ƃ̎��ԍ� (��)
	double delta_time_max; //!<�O�t���[���Ƃ̎��ԍ��̍ő�l
	double draw_delta_time; //<�O�`��t���[���Ƃ̎��ԍ� (��)
	double draw_delta_time_max; //!<�O�`��t���[���Ƃ̎��ԍ��̍ő�l
	double fixed_delta_time; //!<�O�`��t���[���Ƃ̎��ԍ� (��)
	double fixed_delta_time_max; //!<�O�`��t���[���Ƃ̎��ԍ��̍ő�l
	double time_scale = 1; //!<�^�C���X�P�[��(�Q�[�������Ԃ̐i�s�X�s�[�h)

	double fps_max = 60;		//!< UpdateFPS(�����X�V)�̍ő�l
	double draw_fps_max = 60;	//!< DrawFPS(�`��)�̍ő�l
	double fixed_fps_max = 50;	//!< PhysicsFPS(�������Z)�̍ő�l
	double fps = 0;				//!< FPS(�����X�V)�̕p�x
	double draw_fps = 0;		//!< DrawFPS(�`��)�̕p�x
	double fixed_fps = 0;		//!< PhysicsFPS(�������Z)�̕p�x

	//������
	int Init()
	{
		sys_time = (unsigned long long)SEC2MICRO(GetOSTimeD());		//!<�V�X�e�����Ԃ�������
		sys_time_start = sys_time;									//!<�A�v���P�[�V�����J�n�������L�^
		sys_time_prev = sys_time;									//!<�O�t���[���̃V�X�e�����Ԃ����݂̂��̂Ƃ��ċL�^
		real_sys_time_prev = sys_time;								//!<���K�̑O�t���[�����Ԃ����݂̂��̂Ƃ��ċL�^
		delta_time = 0;												//!<�O�t���[������̃����Ԃ�������
		real_delta_time = 0;												//!<�O�t���[������̃����Ԃ�������

		time = 0;										//!<�A�v���P�[�V�����J�n��̎��Ԃ�������
		real_time = 0;									//!<�A�v���P�[�V�����J�n��̐��K���Ԃ�������
		delta_time_max = 1.0 / fps_max;					//!<�X�VFPS�̐ݒ�
		draw_delta_time_max = 1.0 / draw_fps_max;		//!<�`��FPS�̐ݒ�
		fixed_delta_time_max = 1.0 / fixed_fps_max;		//!<����FPS�̏�����
		return 0;
	}

	//�X�V
	void Update()
	{
		sys_time = (unsigned long long)(SEC2MICRO(GetOSTimeD()));					//!<�V�X�e�����Ԃ��L�^
		delta_time = (double)(MICRO2SEC((sys_time - sys_time_prev)));				//!<�O�t���[������̌o�ߎ��Ԃ𑪒�
		real_delta_time = (double)(MICRO2SEC((sys_time - real_sys_time_prev)));		//!<���K�̌o�ߎ��Ԃ��L�^
		real_time += delta_time;													//!<���K�̃A�v���P�[�V�������Ԃ����Z
		time += delta_time * time_scale;											//!<�Q�[�������Ԃ����Z
		draw_delta_time += delta_time;												//!<�O��̕`�悩��̌o�ߎ��Ԃ��L�^
		fixed_delta_time += delta_time;												//!<�O��̕����X�V����̌o�ߎ��Ԃ��L�^

		sys_time_prev = sys_time;						//!<�O�t���[���̃V�X�e�����Ԃ��L�^
		real_sys_time_prev = sys_time;					//!<�O�t���[���̐��K�V�X�e�����Ԃ��L�^
	}

	//FPS�X�V
	void UpdateFPS()
	{
		//!<�����ŕ��ʂ�delta_time���g�p����ƁA����΂����s�����ꍇ��FPS�̌v�Z���񐳋K�̂��̂ɂȂ��Ă��܂�
		fps = 1.0 / real_delta_time;		//!<���ۂ̌o�ߎ��Ԃ��g�p����FPS�𑪒�

	}

	//�������w��FPS�ɌŒ�
	int FixFPS()
	{
		// FPS�Œ�ɂ�while���g�p���Ȃ�
		// while�ł��ƁAFPS�g�p����n���݂����ɐH���̂ŁA
		// Chrono���g�p����Sleep�֐��ŃA�v�����ꎞ��~����

		double now_time = Time::GetOSTimeD();														//!<���݂̐��K���Ԃ��擾
		double system_time = Time::SystemTimeD();													//!<���݂̃V�X�e�����Ԃ��擾
		double sleep_time = delta_time_max - (now_time - system_time);								//!<Sleep���Ԃ��v�Z(delta����-���ۂ̌o�ߎ���)
		int sleep_time_int = int(SEC2MILLI(sleep_time));											//!<sleep���Ԃ�Sleep�֐��ɓn�����߁A�b->�~���b�ւ̕ϊ�&int�^�փL���X�g
		sleep_time_int = sleep_time_int > 0 ? sleep_time_int : 0;									//!<�덷�⏈��������sleep���Ԃ������ɂȂ邱�Ƃ�����̂ŁA�␳
		Sleep(sleep_time_int);																		//!<sleep���Ԃ����A�v���P�[�V�������~
		while (now_time - system_time < delta_time_max) { now_time = Time::GetOSTimeD(); }			//!<����sleep���s���Ă��o�ߎ��Ԃ�����Ȃ���΁A���̎�����while�ŏ������~�߂�
		return 0;
	}

	//����΂�(���Ԃ̏㏑���Ȃ̂ł��܂葽�p���Ȃ��ł�������)
	void ResetTime() {
		sys_time = (unsigned long long)(SEC2MICRO(GetOSTimeD()));			//!<�V�X�e����̎��Ԃ��㏑��
		sys_time_prev = sys_time;											//!<�O�t���[���̎��Ԃ��㏑��
	}

	//�`�揈���̌o�ߎ��Ԃ��������E�`��FPS���v��
	void FixDrawFPS() {
		draw_fps = 1.0 / draw_delta_time;				//!<�`��FPS���v��
		draw_delta_time = 0;							//!<�����Čo�ߎ��Ԃ����Z�b�g(�`��o�ߎ��Ԃ́A�f�N�������g����Ɣ���Ȑ��l�ɂȂ�̂�0�ɂ���)
	}

	// ���������̌o�ߎ��Ԃ��X�V�E����������FPS���v��
	void FixFixedFPS() {
		fixed_fps = 1.0 / fixed_delta_time;						//!<����FPS���v��
		fixed_delta_time -= fixed_delta_time_max;				//!<������1�t���[�����f�N�������g(�������0�ɂ���ƕ������Z���s���m�ɂȂ�̂Ńf�N�������g���Ďg��)
	}


	//�^�C���X�P�[���̎擾(float)
	const float TimeScale()
	{
		return (float)time_scale;
	}

	//�^�C���X�P�[���̎擾(double)
	const double TimeScaleD()
	{
		return time_scale;
	}

	//�^�C���X�P�[���̕ύX
	void SetTimeScale(const double scale)
	{
		time_scale = scale;
	}

	//�Q�[���������Ԃ̎擾(float)
	const float DeltaTime()
	{
		return (float)(delta_time * time_scale);
	}

	//�Q�[���������Ԃ̎擾(double)
	const double DeltaTimeD()
	{
		return delta_time * time_scale;
	}

	//�`��p�����Ԃ̎擾(float)
	const float DrawDeltaTime()
	{
		return  (float)draw_delta_time;
	}

	//�`��p�����Ԃ̎擾(double)
	const double DrawDeltaTimeD()
	{
		return draw_delta_time;
	}

	//�����p�����Ԃ̎擾(float)
	const float FixedDeltaTime()
	{
		return  (float)fixed_delta_time;
	}

	//�����p�����Ԃ̎擾(double)
	const double FixedDeltaTimeD()
	{
		return fixed_delta_time;
	}

	//�O�t���[���Ƃ̕����I���ԍ��̎擾(double)
	const double UnscaledDeltaTimeD()
	{
		return delta_time;
	}

	//�O�t���[���Ƃ̕����I���ԍ��̎擾(float)
	const float UnscaledDeltaTime()
	{
		return (float)delta_time;
	}

	//�A�v���P�[�V�����J�n��̕����I���Ԃ̎擾(float)
	const float SystemTimeFromStart()
	{
		return (float)SystemTimeFromStartD();
	}

	//�A�v���P�[�V�����J�n��̕����I���Ԃ̎擾(double)
	const double SystemTimeFromStartD()
	{
		return MICRO2SEC((double)(sys_time - sys_time_start));
	}

	//�A�v���P�[�V�����J�n��̃Q�[�������Ԃ̎擾
	const double GetTimeFromStart()
	{
		return time;
	}

	//�A�v���P�[�V�����J�n��̌������Ԃ̎擾
	const double GetRealTimeFromStart()
	{
		return real_time;
	}
	//�V�X�e�������Ԃ̎擾(float)
	const float SystemTime()
	{
		return (float)SystemTimeD();
	}

	//�V�X�e�������Ԃ̎擾(double)
	const double SystemTimeD()
	{
		return MICRO2SEC((double)sys_time);
	}

	//Windows����擾�������ݎ���(float)
	const float GetOSTime()
	{

		return (float)GetOSTimeD();
	}

	//Windows����擾�������ݎ���(double)
	const double GetOSTimeD()
	{

		//����܂�悭������񂯂ǁAOS�̎��Ԃ��擾
		LARGE_INTEGER integer;
		LARGE_INTEGER integer2;
		QueryPerformanceCounter(&integer);
		QueryPerformanceFrequency(&integer2);
		return (integer.QuadPart / (double)integer2.QuadPart);
	}

	//FPS�̍ő�l���擾(float)
	const float GetFPSMAX()
	{
		return (float)fps_max;
	}

	//FPS�̍ő�l���擾(double)
	const double GetFPSMAXD()
	{
		return fps_max;
	}

	//�`��FPS�̍ő�l���擾(float)
	const float GetDrawFPSMAX()
	{
		return (float)draw_fps_max;
	}

	//�`��FPS�̍ő�l���擾(double)
	const double GetDrawFPSMAXD()
	{
		return draw_fps_max;
	}

	//FPS�̍ő�l��ݒ�
	void SetFPSMAX(const double& max)
	{
		//FPS�̍ő�l��1�������ƃ[�����Z���N�������肢�낢��Ɗ댯�Ȃ̂ŁA��O�X���[
		fps_max = max;
		if (fps_max < 1)
		{
			throw(Exception("FPS_MAX_LOWER_ZERO", DEFAULT_EXCEPTION_PARAM));
		}
		delta_time_max = 1.0 / fps_max;		//!<�n���ꂽFPS_MAX����Adelta���Ԃ̍ő�l���Đݒ�
	}

	//�����X�VFPS�̍ő�l��ݒ�
	void SetFixedFPSMAX(const double& max)
	{
		fixed_fps_max = max;
		//FIXED_FPS�̍ő�l��1�������ƃ[�����Z���N�������肢�낢��Ɗ댯�Ȃ̂ŁA��O�X���[
		if (fixed_fps_max < 1)
		{
			throw(Exception("FIXED_FPS_MAX_LOWER_ZERO", DEFAULT_EXCEPTION_PARAM));
		}
		fixed_delta_time_max = 1.0 / fixed_fps_max;		//!<�n���ꂽFIXED_FPS_MAX����Adelta���Ԃ̍ő�l���Đݒ�
	}


	//�`��FPS�̍ő�l��ݒ�
	void SetDrawFPSMAX(const double& max)
	{
		draw_fps_max = max;
		//DRAW_FPS�̍ő�l��1�������ƃ[�����Z���N�������肢�낢��Ɗ댯�Ȃ̂ŁA��O�X���[
		if (draw_fps_max < 1)
		{
			throw(Exception("DRAW_FPS_MAX_LOWER_ZERO", DEFAULT_EXCEPTION_PARAM));
		}
		draw_delta_time_max = 1.0 / draw_fps_max;		//!<�n���ꂽDRAW_FPS_MAX����Adelta���Ԃ̍ő�l���Đݒ�
	}

	//���������Ԃ̍ő�l�擾(float)
	const float GetFixedDeltaTimeMAX()
	{
		return (float)fixed_delta_time_max;
	}

	//���������Ԃ̍ő�l�擾(double)
	const double GetFixedDeltaTimeMAXD()
	{
		return fixed_delta_time_max;
	}

	//�`�惢���Ԃ̍ő�l�擾(float)
	const float GetDrawDeltaTimeMAX()
	{
		return (float)draw_delta_time_max;
	}

	//�`�惢���Ԃ̍ő�l�擾(double)
	const double GetDrawDeltaTimeMAXD()
	{
		return draw_delta_time_max;
	}

	//�����Ԃ̍ő�l�擾(float)
	const float GetDeltaTimeMAX()
	{
		return (float)delta_time_max;
	}

	//�����Ԃ̍ő�l�擾(double)
	const double GetDeltaTimeMAXD()
	{
		return delta_time_max;
	}

	//FPS�̎擾(float)
	const float GetFPS()
	{
		return (float)fps;
	}

	//FPS�̎擾(double)
	const double GetFPSD()
	{
		return fps;
	}
}