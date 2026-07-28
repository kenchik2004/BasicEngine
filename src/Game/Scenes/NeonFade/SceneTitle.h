// SceneTitle.h
#pragma once
#include "DebugCameraScene.h"

namespace NeonFade {
	USING_PTR(SceneGame);
	//---------------------------------------------------------------------
	//! @class SceneTitle
	//! @brief タイトルシーンクラス
	//---------------------------------------------------------------------
	USING_PTR(TitleLogo);
	USING_PTR(OptionManager);
	USING_PTR(SettingsManager);

	/// @brief タイトル画面を構成するシーンクラス
	/// @details メニューの選択や背景の演出を管理する
	class SceneTitle :
		//public DebugCameraScene
		public Scene
	{
	public:
		USING_SUPER(SceneTitle);
		/// @brief リソースのロード処理を行う
		void Load() override;
		/// @brief シーンの初期化処理を行う
		/// @return 初期化の成否を示す整数値
		int Init() override;
	private:

		TitleLogoWP logo; /// @brief タイトルロゴへの弱参照
		OptionManagerWP opt_manager; /// @brief オプションマネージャーへの弱参照
		SettingsManagerWP settings_manager; /// @brief 設定マネージャーへの弱参照

	};

}