#pragma once
namespace Time
{

#define MICRO2SEC(micro_sec) micro_sec*0.000001
#define SEC2MICRO(sec) sec*1000000
#define MILLI2SEC(micro_sec) micro_sec*0.001
#define SEC2MILLI(sec) sec*1000


	//初期化
	int Init();
	//更新
	void Update();
	void UpdateFPS();
	int FixFPS();
	void ResetTime();
	void FixDrawFPS();
	void FixFixedFPS();
	const double GetTimeFromStart();
	//アプリケーション開始後のゲーム内時間の取得
	const float TimeScale();	//タイムスケールの取得
	const double TimeScaleD();
	void SetTimeScale(const double scale);//タイムスケールの変更
	const float DeltaTime();
	const double DeltaTimeD();
	const float DrawDeltaTime();
	const double DrawDeltaTimeD();
	const float FixedDeltaTime();
	//前フレームとのゲーム内時間差の取得
	const float UnscaledDeltaTime();
	const double FixedDeltaTimeD();
	//前フレームとの実際の時間差の取得
	const double UnscaledDeltaTimeD(); 
	const float SystemTimeFromStart(); //アプリケーション開始後の物理的時間の取得
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

