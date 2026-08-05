#pragma once

USING_PTR(ImageRenderer);

namespace NeonFade {
	USING_PTR(TutorialUIButtonImage);
	//! @brief チュートリアルUIのボタン画像を表示するクラス
	class TutorialUIButtonImage :public UIObject {
	public:
		USING_SUPER(TutorialUIButtonImage);

		//! @brief コンストラクタ
		//! @param button_type 表示するゲームパッドのボタンの種類
		TutorialUIButtonImage(PadButton button_type)
			:UIObject()
		{
			current_button = button_type;
		}
		//! @brief ボタンの画像をロードする
		static void LoadPadButtonImages();
		//! @brief ボタンの画像を解放する
		static void UnloadPadButtonImages();

		//! @brief 初期化処理
		int Init() override;

		//! @brief 更新処理
		void Update() override;
		//! @brief 表示するボタンの種類を設定する
		void SetPadButtonImage(PadButton button);

		//! @brief WakeUp時の処理
		void OnWakeUp() override {
			animation_timer = 0.0f;
			transform->scale = { 0.001f, 0.001f, 0.001f };
		}
		//! @brief Sleep時の処理
		void OnSleep() override {
			animation_timer = 0.0f;
		}
	private:

		static inline bool pad_images_loaded = false;	//<! ゲームパッドのボタン画像がロードされているかどうかのフラグ
		static const u8 PAD_BUTTON_COUNT = static_cast<u8>(PadButton::Button4) + 1;	//<! ゲームパッドのボタン数

		float animation_timer = 0.0f;	//<! ボタン画像のアニメーションタイマー

		PadButton current_button = PadButton::Button1;	//<! 現在表示しているボタンの種類

		static inline std::array<SafeSharedPtr<Texture>, PAD_BUTTON_COUNT> released_button_textures = { nullptr };	//<! パッドのボタン画像の配列
		static inline std::array<SafeSharedPtr<Texture>, PAD_BUTTON_COUNT> pushed_button_textures = { nullptr };	//<! パッド(押下状態)のボタン画像の配列

		ImageRendererWP image_renderer = nullptr;		//<! ボタン画像を描画するImageRendererへの弱参照ポインタ

	};

}

