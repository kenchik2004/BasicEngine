#pragma once

USING_PTR(ImageRenderer);
namespace NeonFade {

	USING_PTR(TutorialUIMovie);

	//! @brief チュートリアル用のUIオブジェクトで、動画を再生するためのクラス
	class TutorialUIMovie :public UIObject {

	public:

		//! @brief コンストラクタ
		//! @param movie 再生する動画の名前
		TutorialUIMovie(const std::string& movie)
			:UIObject()
		{
			movie_name = movie;
		}

		//! @brief 初期化処理
		int Init();
		//! @brief WakeUpが呼ばれたときの処理
		void OnWakeUp() override;
		//! @brief Sleepが呼ばれたときの処理
		void OnSleep() override;
		//! @brief 毎フレームの更新処理
		void Update() override;
		//! @brief 終了処理
		void Exit() override;

	private:
		ImageRendererWP my_movie;		//<! 自身のImageRendererへの弱参照ポインタ
		SafeSharedPtr<Texture> movie_texture;		//<! 再生する動画のテクスチャへのスマートポインタ
		std::string movie_name = "";		//<! 再生する動画の名前
	};
}

