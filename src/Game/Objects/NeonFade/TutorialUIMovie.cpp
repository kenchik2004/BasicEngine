#include "TutorialUIMovie.h"

namespace NeonFade {

	//! @brief 初期化処理
	int TutorialUIMovie::Init() {
		//サイズと位置を設定する
		static constexpr float ASPECT_RATIO = 1920.0f / 1080.0f;
		static constexpr float movie_height = 600.0f;
		static constexpr float movie_width = movie_height * ASPECT_RATIO;

		transform->position = { 0,100,0 };
		transform->scale = { movie_width, movie_height,1 };

		//画像描画コンポーネントを追加する
		auto movie_renderer = AddComponent<ImageRenderer>();

		//デフォルトの2Dマテリアルを元に新しいマテリアルを作成する
		Material* movie_material = MaterialManager::CreateMaterial("movie_material", MaterialManager::GetDefaultMat2D());

		//動画のテクスチャを取得してマテリアルに設定する
		movie_texture = TextureManager::Get(movie_name);
		movie_material->SetTexture(movie_texture, Material::TextureType::Diffuse);

		//作成したマテリアルを画像描画コンポーネントに設定する
		movie_renderer->SetMaterial(movie_material);
		my_movie = movie_renderer;

		DxLib::ChangeMovieVolumeToGraph(*movie_texture, 0); //動画の音量を0にする


		//デフォルトでは動画の再生を停止しておく
		Sleep();
		return Super::Init();
	}

	//! @brief WakeUpが呼ばれたときの処理
	void TutorialUIMovie::OnWakeUp() {
		//動画の再生を開始する
		if (movie_texture && DxLib::GetMovieStateToGraph(*movie_texture) == 0) {
			//動画をループ再生する
			DxLib::PlayMovieToGraph(*movie_texture, DX_PLAYTYPE_LOOP);

		}
	}
	void TutorialUIMovie::OnSleep() {

		//動画の再生を停止する
		if (movie_texture && DxLib::GetMovieStateToGraph(*movie_texture) == 1) {
			DxLib::PauseMovieToGraph(*movie_texture);
		}

		//動画を先頭に戻す
		if (movie_texture)
			DxLib::SeekMovieToGraph(*movie_texture, 0);
	}

	//! @brief 毎フレームの更新処理
	void TutorialUIMovie::Update() {

		//動画の再生状態を更新する
		//(これを行わないと最新の情報が吐き出しバッファに反映されない)
		if (movie_texture) {
			DxLib::UpdateMovieToGraph(*movie_texture);
		}
	}

	//! @brief 終了処理
	void TutorialUIMovie::Exit() {

		//動画の再生を停止する
		//(OnSleepと同じ処理を行う)
		OnSleep();

		Super::Exit();
	}
}