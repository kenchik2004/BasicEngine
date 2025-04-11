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

	//初期化
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

	//更新
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

	//FPS更新
	void UpdateFPS()
	{
		fps = 1.0 / real_delta_time;

	}
	//処理を指定FPSに固定・ゲーム更新のFPSを計測
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
	//時飛ばし(時間の上書きなのであまり多用しないでください)
	void ResetTime() {
		sys_time = (unsigned long long)(SEC2MICRO(GetOSTimeD()));
		sys_time_prev = sys_time;
	}
	//描画処理の経過時間を初期化・描画FPSを計測
	void FixDrawFPS() {
		draw_fps = 1.0 / draw_delta_time;
		draw_delta_time = 0;
	}
	//物理処理の経過時間を更新・物理処理のFPSを計測
	void FixFixedFPS() {
		fixed_fps = 1.0 / fixed_delta_time;
		fixed_delta_time -= fixed_delta_time_max;
	}


	//タイムスケールの取得(float)
	const float TimeScale()
	{
		return (float)time_scale;
	}

	//タイムスケールの取得(double)
	const double TimeScaleD()
	{
		return time_scale;
	}

	//タイムスケールの変更
	void SetTimeScale(const double scale)
	{
		time_scale = scale;
	}

	//ゲーム内Δ時間の取得(float)
	const float DeltaTime()
	{
		return (float)(delta_time * time_scale);
	}

	//ゲーム内Δ時間の取得(double)
	const double DeltaTimeD()
	{
		return delta_time * time_scale;
	}

	//描画用Δ時間の取得(float)
	const float DrawDeltaTime()
	{
		return  (float)draw_delta_time;
	}

	//描画用Δ時間の取得(double)
	const double DrawDeltaTimeD()
	{
		return draw_delta_time;
	}

	//物理用Δ時間の取得(float)
	const float FixedDeltaTime()
	{
		return  (float)fixed_delta_time;
	}

	//物理用Δ時間の取得(double)
	const double FixedDeltaTimeD()
	{
		return fixed_delta_time;
	}

	//前フレームとの物理的時間差の取得(double)
	const double UnscaledDeltaTimeD()
	{
		return delta_time;
	}

	//前フレームとの物理的時間差の取得(float)
	const float UnscaledDeltaTime()
	{
		return (float)delta_time;
	}

	//アプリケーション開始後の物理的時間の取得(float)
	const float SystemTimeFromStart()
	{
		return (float)SystemTimeFromStartD();
	}

	//アプリケーション開始後の物理的時間の取得(double)
	const double SystemTimeFromStartD()
	{
		return MICRO2SEC((double)(sys_time - sys_time_start));
	}

	//アプリケーション開始後のゲーム内時間の取得
	const double GetTimeFromStart()
	{
		return time - MICRO2SEC((sys_time_start));
	}
	//システム内時間の取得(float)
	const float SystemTime()
	{
		return (float)SystemTimeD();
	}

	//システム内時間の取得(double)
	const double SystemTimeD()
	{
		return MICRO2SEC((double)sys_time);
	}

	//Windowsから取得した現在時刻(float)
	const float GetOSTime()
	{

		return (float)GetOSTimeD();
	}

	//Windowsから取得した現在時刻(double)
	const double GetOSTimeD()
	{

		LARGE_INTEGER integer;
		LARGE_INTEGER integer2;
		QueryPerformanceCounter(&integer);
		QueryPerformanceFrequency(&integer2);
		return (integer.QuadPart / (double)integer2.QuadPart);
	}

	//FPSの最大値を取得(float)
	const float GetFPSMAX()
	{
		return (float)fps_max;
	}

	//FPSの最大値を取得(double)
	const double GetFPSMAXD()
	{
		return fps_max;
	}

	//描画FPSの最大値を取得(float)
	const float GetDrawFPSMAX()
	{
		return (float)draw_fps_max;
	}

	//描画FPSの最大値を取得(double)
	const double GetDrawFPSMAXD()
	{
		return draw_fps_max;
	}

	//FPSの最大値を設定
	void SetFPSMAX(const double& max)
	{
		fps_max = max;
		if (fps_max < 1)
		{
			throw(Exception("FPS_MAX_LOWER_ZERO", DEFAULT_EXCEPTION_PARAM));
		}
		delta_time_max = 1.0 / fps_max;
	}

	//物理更新FPSの最大値を設定
	void SetFixedFPSMAX(const double& max)
	{
		fixed_fps_max = max;
	}


	//描画FPSの最大値を設定
	void SetDrawFPSMAX(const double& max)
	{
		draw_fps_max = max;
		if (draw_fps_max < 1)
		{
			throw(Exception("DRAW_FPS_MAX_LOWER_ZERO", DEFAULT_EXCEPTION_PARAM));
		}
		draw_delta_time_max = 1.0 / draw_fps_max;
	}

	//物理Δ時間の最大値取得(float)
	const float GetFixedDeltaTimeMAX()
	{
		return (float)fixed_delta_time_max;
	}

	//物理Δ時間の最大値取得(double)
	const double GetFixedDeltaTimeMAXD()
	{
		return fixed_delta_time_max;
	}
	
	//描画Δ時間の最大値取得(float)
	const float GetDrawDeltaTimeMAX()
	{
		return (float)draw_delta_time_max;
	}

	//描画Δ時間の最大値取得(double)
	const double GetDrawDeltaTimeMAXD()
	{
		return draw_delta_time_max;
	}

	//Δ時間の最大値取得(float)
	const float GetDeltaTimeMAX()
	{
		return (float)delta_time_max;
	}

	//Δ時間の最大値取得(double)
	const double GetDeltaTimeMAXD()
	{
		return delta_time_max;
	}

	//FPSの取得(float)
	const float GetFPS()
	{
		return (float)fps;
	}

	//FPSの取得(double)
	const double GetFPSD()
	{
		return fps;
	}
}