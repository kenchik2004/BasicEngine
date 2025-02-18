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
	void FixDrawFPS();
	const double GetTimeFromStart(); //アプリケーション開始後のゲーム内時間の取得
	const double TimeScale();	//タイムスケールの取得
	void SetTimeScale(const double scale);//タイムスケールの変更
	const double DeltaTime();
	const double DrawDeltaTime();
	//前フレームとのゲーム内時間差の取得
	const double UnscaledDeltaTime(); //前フレームとの実際の時間差の取得
	const double SystemTimeFromStart(); //アプリケーション開始後の物理的時間の取得
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

