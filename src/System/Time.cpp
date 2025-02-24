#include "precompile.h"
#include "Time.h"

namespace Time {
	//システム用の時間管理用変数(ulonglong管理)
	unsigned long long sys_time; //<!システム上の時間(
	unsigned long long sys_time_start; //<!システム上のアプリケーション開始時間
	unsigned long long sys_time_prev; //<!システム上の前フレームの時間
	unsigned long long real_sys_time_prev; //<!システム上の前フレームの時間
	//外部で使用するための時間管理用変数(float[秒]管理)
	double time;//<!アプリケーション開始後の経過時間(ゲーム内時間)
	double delta_time; //!<前フレームとの時間差 (Δ)
	double real_delta_time; //!<前フレームとの時間差 (Δ)
	double delta_time_max; //!<前フレームとの時間差の最大値
	double draw_delta_time; //!<前描画フレームとの時間差 (Δ)
	double draw_delta_time_max; //!<前描画フレームとの時間差の最大値
	double fixed_delta_time; //!<前描画フレームとの時間差 (Δ)
	double fixed_delta_time_max; //!<前描画フレームとの時間差の最大値
	double time_scale = 1; //<!タイムスケール(ゲーム内時間の進行スピード)

	double fps_max = 166;
	double draw_fps_max = 60;
	double fixed_fps_max = 50;
	double fps = 0;
	double draw_fps = 0;
	double fixed_fps = 0;
	int Init()
	{
		sys_time = (unsigned long long)SEC2MICRO(GetOSTimeD());
		time = MICRO2SEC((double)sys_time);
		draw_delta_time_max = 1.0 / draw_fps_max;
		delta_time_max = 1.0 / fps_max;
		fixed_delta_time_max = 1.0 / fixed_fps_max;
		sys_time_start = sys_time;
		sys_time_prev = sys_time;
		real_sys_time_prev = sys_time;
		delta_time = 0;
		return 0;
	}
	void Update()
	{

		sys_time = (unsigned long long)(SEC2MICRO(GetOSTimeD()));
		delta_time = (double)(MICRO2SEC((sys_time - sys_time_prev)));
		real_delta_time = (double)(MICRO2SEC((sys_time - real_sys_time_prev)));
		draw_delta_time += delta_time;
		fixed_delta_time += delta_time;
		time += delta_time * time_scale;
		sys_time_prev = sys_time;
		real_sys_time_prev = sys_time;
	}
	void UpdateFPS()
	{
		fps = 1.0 / real_delta_time;

	}
	int FixFPS()
	{
		double now_time = Time::GetOSTimeD();
		double system_time = Time::SystemTimeD();
		double sleep_time = delta_time_max - (now_time - system_time);
		int a = int(SEC2MILLI(sleep_time));
		a = a > 0 ? a : 0;
		Sleep(a);
		while (now_time - system_time < delta_time_max) { now_time = Time::GetOSTimeD(); }
		return 0;
	}
	void ResetTime() {
		sys_time = (unsigned long long)(SEC2MICRO(GetOSTimeD()));
		sys_time_prev = sys_time;
	}

	void FixDrawFPS() {
		draw_fps = 1.0 / draw_delta_time;
		draw_delta_time -= draw_delta_time_max;
	}

	void FixFixedFPS() {
		fixed_fps = 1.0 / fixed_delta_time;
		fixed_delta_time -= fixed_delta_time_max;
	}

	const double GetTimeFromStart()

	{
		return time - MICRO2SEC((sys_time_start));
	}

	const double TimeScaleD()
	{
		return time_scale;
	}
	const float TimeScale()
	{
		return (float)time_scale;
	}
	void SetTimeScale(const double scale)
	{
		time_scale = scale;
	}

	const float DeltaTime()
	{
		return (float)(delta_time * time_scale);
	}

	const double DeltaTimeD()
	{
		return delta_time * time_scale;
	}

	const float DrawDeltaTime()
	{
		return  (float)draw_delta_time;
	}

	const double DrawDeltaTimeD()
	{
		return draw_delta_time;
	}

	const float FixedDeltaTime()
	{
		return  (float)fixed_delta_time;
	}

	const double FixedDeltaTimeD()
	{
		return fixed_delta_time;
	}

	const double UnscaledDeltaTimeD()
	{
		return delta_time;
	}
	const float UnscaledDeltaTime()
	{

		return (float)delta_time;
	}

	const float SystemTimeFromStart()
	{
		return (float)SystemTimeFromStartD();
	}

	const double SystemTimeFromStartD()
	{
		return MICRO2SEC((double)(sys_time - sys_time_start));
	}

	const float SystemTime()
	{
		return (float)SystemTimeD();
	}

	const double SystemTimeD()
	{
		return MICRO2SEC((double)sys_time);
	}

	const float GetOSTime()
	{

		return (float)GetOSTimeD();
	}
	const double GetOSTimeD()
	{

		LARGE_INTEGER integer;
		LARGE_INTEGER integer2;
		QueryPerformanceCounter(&integer);
		QueryPerformanceFrequency(&integer2);
		return (integer.QuadPart / (double)integer2.QuadPart);
	}
	const float GetFPSMAX()
	{
		return (float)fps_max;
	}
	const double GetFPSMAXD()
	{
		return fps_max;
	}
	const float GetDrawFPSMAX()
	{
		return (float)draw_fps_max;
	}
	const double GetDrawFPSMAXD()
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

	void SetFixedFPSMAX(const double& max)
	{
		fixed_fps_max = max;
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
	const float GetFixedDeltaTimeMAX()
	{
		return (float)fixed_delta_time_max;
	}
	const double GetFixedDeltaTimeMAXD()
	{
		return fixed_delta_time_max;
	}
	const float GetDrawDeltaTimeMAX()
	{
		return (float)draw_delta_time_max;
	}
	const double GetDrawDeltaTimeMAXD()
	{
		return draw_delta_time_max;
	}
	const float GetDeltaTimeMAX()
	{
		return (float)delta_time_max;
	}
	const double GetDeltaTimeMAXD()
	{
		return delta_time_max;
	}

	const float GetFPS()
	{
		return (float)fps;
	}
	const double GetFPSD()
	{
		return fps;
	}
}