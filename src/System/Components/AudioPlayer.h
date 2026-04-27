//---------------------------------------------------------------------------
//! @file   AudioPlayer.h
//! @brief  音声の再生・停止・ループを管理するAudioPlayerコンポーネント
//---------------------------------------------------------------------------
#pragma once

USING_PTR(AudioPlayer);
//---------------------------------------------------------------------
//! @class AudioPlayer
//! @brief 音声クリップの再生・停止・ループ制御を行うコンポーネント
//---------------------------------------------------------------------
class AudioPlayer :
	public Component
{
public:
	USING_SUPER(AudioPlayer);
	//! @brief コンストラクト処理
	void Construct() override;
	SafeSharedPtr<AudioClip> audio = nullptr; //!< 再生する音声クリップ
	//! @brief 音声クリップを名前で設定する
	void SetAudio(std::string_view name, std::string_view new_name = "");
	//! @brief 音声クリップをポインタで設定する
	void SetAudio(SafeSharedPtr<AudioClip> audio_);

	//! @brief 現在設定されている音声クリップを取得する
	inline SafeSharedPtr<AudioClip> GetAudio() {
		return audio;
	}

	//! @brief 音声を再生する
	//! @param start_pos  再生開始位置（秒）
	//! @param sample_rate サンプルレート（-1でデフォルト）
	void Play(float start_pos = 0, int sample_rate = -1);
	//! @brief 音声が再生中かどうか返す
	bool IsPlaying() const { return is_playing; }
	//! @brief 更新処理（3D音声の位置更新など）
	void Update() override;
	//! @brief 音声を停止する
	void Stop();
	//! @brief 音声ファイルをロードする
	static void Load(std::string_view path, std::string_view name, bool use_3d = true);
	//! @brief 終了処理
	void Exit() override;
	int default_frequency;     //!< デフォルトのサンプリング周波数
	float pitch_rate = 1.0f;   //!< ピッチ変更倍率
	float radius = 200;        //!< 3D音声の減衰半径
	float volume = 1.0f;       //!< 音量（0.0〜1.0）
	bool loop = false;         //!< ループ再生するか
	bool is_3d = true;         //!< 3D音声として扱うか
private:
	bool is_playing = false;              //!< 再生中かどうか
	Vector3 last_position = { 0,0,0 };   //!< 前フレームの位置（ドップラー計算用）
};

