#include "TutorialUIButtonImage.h"

namespace NeonFade {
	void TutorialUIButtonImage::LoadPadButtonImages() {

		//既にロードされている場合は何もしない
		if (pad_images_loaded)
			return;

		//ゲームパッドのボタン画像をクローンする
		for (u8 i = 0; i < PAD_BUTTON_COUNT; ++i) {
			released_button_textures[i] = TextureManager::CloneByName("switch_button_released_" + std::to_string(i));
			pushed_button_textures[i] = TextureManager::CloneByName("switch_button_pushed_" + std::to_string(i));
		}

		//ロード完了フラグを立てる
		pad_images_loaded = true;
	}
	void TutorialUIButtonImage::UnloadPadButtonImages() {

		//ゲームパッドのボタン画像を解放する
		for (u8 i = 0; i < PAD_BUTTON_COUNT; ++i) {
			released_button_textures[i] = nullptr;
			pushed_button_textures[i] = nullptr;
		}

		//ロード完了フラグをへし折る
		pad_images_loaded = false;
	}
	int TutorialUIButtonImage::Init() {

		//ゲームパッドのボタン画像をロードする
		if (!pad_images_loaded)
			LoadPadButtonImages();

		//位置を設定する
		transform->position = { 400,-70,0 };


		//ボタンの画像を表示するコンポーネントを追加する
		image_renderer = AddComponent<ImageRenderer>();

		//初期化も兼ねてボタンセットを行う
		SetPadButtonImage(current_button);

		Sleep();
		return Super::Init();
	}
	void TutorialUIButtonImage::Update() {

		//ボタン画像のアニメーションを更新する
		float animation_timer_prev = animation_timer;
		animation_timer += Time::UnscaledDeltaTime();

		//ボタンのインデックスを取得する
		u8 button_index = static_cast<u8>(current_button);


		//ボタン画像のアニメーションを更新する
		if (animation_timer > 0.5f && animation_timer_prev <= 0.5f) {
			//ボタン画像を押下状態に切り替える
			Material* material = image_renderer->GetMaterial();
			material->SetTexture(pushed_button_textures[button_index], Material::TextureType::Diffuse);

			//ボタン画像のサイズを更新する
			Vector2 image_size = image_renderer->GetImageSize();
			//ボタン画像のサイズを小さくし、オブジェクトのスケールに反映する
			image_size *= 0.3f;
			transform->scale = { image_size.x, image_size.y, 1 };
		}
		//ボタン画像のアニメーションを更新する
		if (animation_timer > 1.0f && animation_timer_prev <= 1.0f) {
			//アニメーションタイマーをリセットする
			animation_timer = 0.0f;
			
			//ボタン画像を元に戻す
			Material* material = image_renderer->GetMaterial();
			material->SetTexture(released_button_textures[button_index], Material::TextureType::Diffuse);

			//ボタン画像のサイズを更新する
			Vector2 image_size = image_renderer->GetImageSize();
			//ボタン画像のサイズを小さくし、オブジェクトのスケールに反映する
			image_size *= 0.3f;
			transform->scale = { image_size.x, image_size.y, 1 };
		}
	}
	void TutorialUIButtonImage::SetPadButtonImage(PadButton button) {
		//現在のボタンを更新する
		current_button = button;
		//ボタンのインデックスを取得する
		u8 button_index = static_cast<u8>(button);

		//ボタン画像を設定する
		Material* material = image_renderer->GetMaterial();
		material->SetTexture(released_button_textures[button_index], Material::TextureType::Diffuse);

		//ボタン画像のサイズを更新する
		Vector2 image_size = image_renderer->GetImageSize();
		//ボタン画像のサイズを小さくし、オブジェクトのスケールに反映する
		image_size *= 0.3f;
		transform->scale = { image_size.x, image_size.y, 1 };

	}
}