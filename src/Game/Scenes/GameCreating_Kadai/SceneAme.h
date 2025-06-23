#pragma once
#include "System/Scene.h"
USING_PTR(TextObject);
namespace AmeGame {

	//! @brief 雨のシーン
	//! @details このシーンは雨のエフェクトを表示するためのシーンです。
	class SceneAme :
		public Scene
	{
	public:
		USING_SUPER(SceneAme);
		//! @brief シーンのロード
		void Load() override;
		//! @brief シーンの初期化
		int Init() override;
		//! @brief シーンの更新
		void Update() override;
		//! @brief シーンの物理更新
		void Draw() override;
		//! @brief シーンの描画
		void LateDraw() override;
		void UnLoad() override;

		SafeSharedPtr<Texture> rain_texture; //!< 雨のテクスチャ
		GameObjectWP camera;
		UIObjectWP txtnavi;
	private:
		bool CheckForLoading(); //!< ロード状況の確認
	};
}

