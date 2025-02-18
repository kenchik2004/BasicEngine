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
	double delta_time_max; //!<前フレームとの時間差の最大値
	double draw_delta_time; //!<前描画フレームとの時間差 (Δ)
	double draw_delta_time_max; //!<前描画フレームとの時間差の最大値
	double time_scale = 1; //<!タイムスケール(ゲーム内時間の進行スピード)

	double fps_max = 166;
	double draw_fps_max = 60;
	double fps = 0;
	double draw_fps = 0;
	int Init()
	{
		sys_time = (unsigned long long)SEC2MICRO(GetOSTime());
		time = MICRO2SEC((double)sys_time);
		draw_delta_time_max = 1.0 / draw_fps_max;
		delta_time_max = 1.0 / fps_max;
		sys_time_start = sys_time;
		sys_time_prev = sys_time;
		delta_time = 0;
		return 0;
	}
	void Update()
	{

		sys_time = (unsigned long long)(SEC2MICRO(GetOSTime()));
		delta_time = (double)(MICRO2SEC((sys_time - sys_time_prev)));
		draw_delta_time += delta_time;
		time += delta_time * time_scale;
		sys_time_prev = sys_time;
	}
	void UpdateFPS()
	{
		fps = 1.0 / UnscaledDeltaTime();

	}
	int FixFPS()
	{
		double now_time = Time::GetOSTime();
		double system_time = Time::SystemTime();
		double sleep_time = delta_time_max - (now_time - system_time);
		int a = int(SEC2MILLI(sleep_time));
		a = a > 0 ? a : 0;
		Sleep(a);
		while (now_time - system_time < delta_time_max) { now_time = Time::GetOSTime(); }
		return 0;
	}


	void FixDrawFPS() {
		draw_fps = 1.0 / draw_delta_time;
		draw_delta_time = 0;
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

	const double DrawDeltaTime()
	{
		return draw_delta_time;
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
	const double GetDrawFPSMAX()
	{
		return draw_fps_max;
	}
	void SetFPSMAX(const double& max)
	{
		fps_max = max;
		if (fps_max < 1)
		{
			throw(Exception("FPS_MAX_LOWER_ZERO", DEFAULT_EXCEPTION_PARAM));
		}
		delta_time_max = 1.0 / fps_max;
	}


	void SetDrawFPSMAX(const double& max)
	{
		draw_fps_max = max;
		if (draw_fps_max < 1)
		{
			throw(Exception("DRAW_FPS_MAX_LOWER_ZERO", DEFAULT_EXCEPTION_PARAM));
		}
		draw_delta_time_max = 1.0 / draw_fps_max;
	}
	const double GetDrawDeltaTimeMAX()
	{
		return draw_delta_time_max;
	}
	const double GetDeltaTimeMAX()
	{
		return delta_time_max;
	}

	const double GetFPS()
	{
		return fps;
	}
}