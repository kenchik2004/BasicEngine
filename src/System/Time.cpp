#include "precompile.h"
#include "Time.h"

namespace Time {
	//システム用の時間管理用変数(ulonglong管理)
	unsigned long long sys_time; //<!システム上の時間(
	unsigned long long sys_time_start; //<!システム上のアプリケーション開始時間
	unsigned long long sys_time_prev; //<!システム上の前フレームの時間
	//外部で使用するための時間管理用変数(float[秒]管理)
	double time;//<!アプリケーション開始後の経過時間(ゲーム内時間)
	double delta_time; //!<前フレームとの時間差 (Δ)
	double time_scale = 1; //<!タイムスケール(ゲーム内時間の進行スピード)

	double fps_max = 100000;
	double fps;
	int Init()
	{
		sys_time = (unsigned long long)SEC2MICRO(GetOSTime());
		time = MICRO2SEC((double)sys_time);
		sys_time_start = sys_time;
		sys_time_prev = sys_time;
		delta_time = 0;
		return 0;
	}
	void Update()
	{

		sys_time = (unsigned long long)(SEC2MICRO(GetOSTime()));
		delta_time = (double)(MICRO2SEC((sys_time - sys_time_prev)));
		time += delta_time * time_scale;
		sys_time_prev = sys_time;
	}
	void UpdateFPS()
	{
		fps = 1.0 / UnscaledDeltaTime();
	}
	void FixFPS()
	{
		double now_time = Time::GetOSTime();
		double system_time = Time::SystemTime();
		double delta = (1.0 / fps_max);
		double sleep_time = delta - (now_time - system_time);
		int a = int(SEC2MILLI(sleep_time));
		a = a > 0 ? a : 0;
		Sleep(a);
		while (now_time - system_time < delta) { now_time = Time::GetOSTime(); }

	}
	const double GetTimeFromStart()

	{
		return time - MICRO2SEC((sys_time_start));
	}

	const double TimeScale()
	{
		return time_scale;
	}
	void SetTimeScale(const double scale)
	{
		time_scale = scale;
	}

	const double DeltaTime()
	{
		return delta_time * time_scale;
	}

	const double UnscaledDeltaTime()
	{

		return delta_time;
	}

	const double SystemTimeFromStart()
	{
		return MICRO2SEC((double)(sys_time - sys_time_start));
	}

	const double SystemTime()
	{
		return MICRO2SEC((double)sys_time);
	}

	const double GetOSTime()
	{

		LARGE_INTEGER integer;
		LARGE_INTEGER integer2;
		QueryPerformanceCounter(&integer);
		QueryPerformanceFrequency(&integer2);
		return (integer.QuadPart / (double)integer2.QuadPart);
	}
	const double GetFPSMAX()
	{
		return fps_max;
	}
	void SetFPSMAX(const double& max)
	{
		fps_max = max;
	}
	const double GetFPS()
	{
		return fps;
	}
}