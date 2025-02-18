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
	const double GetTimeFromStart(); //アプリケーション開始後のゲーム内時間の取得
	const double TimeScale();	//タイムスケールの取得
	void SetTimeScale(const double scale);//タイムスケールの変更
	const double DeltaTime();//前フレームとのゲーム内時間差の取得
	const double UnscaledDeltaTime(); //前フレームとの実際の時間差の取得
	const double SystemTimeFromStart(); //アプリケーション開始後の物理的時間の取得
	const double SystemTime();
	const double GetOSTime();
	const double GetFPSMAX();
	void SetFPSMAX(const double& max);
	const double GetFPS();
};

