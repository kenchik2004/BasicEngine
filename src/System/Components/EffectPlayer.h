//---------------------------------------------------------------------------
//! @file   EffectPlayer.h
//! @brief  パーティクルエフェクトの再生・停止を管理するEffectPlayerコンポーネント
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------
//! @class EffectPlayer
//! @brief Effekseerを使ったパーティクルエフェクトの再生を管理するコンポーネント
//---------------------------------------------------------------------
class EffectPlayer :
	public Component
{
public:
	USING_SUPER(EffectPlayer);
	//! @brief デフォルトコンストラクタ
	EffectPlayer() = default;
	//! @brief エフェクト名を指定して初期化するコンストラクタ
	EffectPlayer(std::string_view name);
	//! @brief 初期化処理
	int Init() override;
	//! @brief 描画処理（後処理）
	void LateDraw() override;
	//! @brief エフェクトを再生する
	//! @param loop ループ再生するか
	void Play(bool loop = false);
	//! @brief エフェクトを停止する
	void Stop();
	//! @brief エフェクトファイルをロードする
	void Load(std::string_view effect_name_);
	//! @brief 終了処理
	void Exit() override;
	//! @brief 再生速度を設定する
	void SetSpeed(float speed_) { speed = speed_; }
	//! @brief 再生速度を取得する
	float GetSpeed() const { return speed; }
private:
	int effect_handle = -1;   //!< エフェクトリソースのハンドル
	int playing_handle = -1;  //!< 再生中インスタンスのハンドル
	std::string effect_name;  //!< エフェクト名
	float speed = 1.0f;       //!< 再生速度
	bool is_playing = false;  //!< 再生中かどうか
	bool is_loop = false;     //!< ループ再生するか
	static std::unordered_map<std::string, int> effect_handle_map; //!< エフェクト名とハンドルのマップ

};

