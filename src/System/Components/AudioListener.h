//---------------------------------------------------------------------------
//! @file   AudioListener.h
//! @brief  音声の受聴位置を管理するAudioListenerコンポーネント
//---------------------------------------------------------------------------
#pragma once

USING_PTR(AudioListener);
//---------------------------------------------------------------------
//! @class AudioListener
//! @brief 3Dオーディオの受聴位置（リスナー）を管理するコンポーネント
//---------------------------------------------------------------------
class AudioListener final :
	public Component
{
	bool is_current_listener = false; //!< 現在アクティブなリスナーか
public:
	USING_SUPER(AudioListener);
	//! @brief コンストラクト処理
	void Construct() override;
	//! @brief 初期化処理
	int Init() override;
	//! @brief 更新処理（リスナー位置の更新）
	void Update() override;
	//! @brief このコンポーネントをアクティブなリスナーに設定する
	void SetCurrentListener();
	//! @brief 現在アクティブなAudioListenerを取得する
	SafeSharedPtr<AudioListener> GetCurrentListener();

};

