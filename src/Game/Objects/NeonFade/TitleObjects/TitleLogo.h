// TitleLogo.h
#pragma once

USING_PTR(ImageRenderer);
namespace NeonFade {

	USING_PTR(TitleLogo);

	/// @brief タイトルロゴを管理するUIオブジェクトクラス
	/// @details 画面上にゲームタイトルを描画する役割を持つ
	class TitleLogo : public UIObject {
	public:
		USING_SUPER(TitleLogo);
		/// @brief ロゴの初期化処理を行う
		/// @return 初期化の成否を示す整数値
		int Init() override;
	private:
		ImageRendererWP logo_image; /// @brief 画像コンポーネントへの弱参照
		//TextWP logo_text; /// @brief テキストコンポーネントへの弱参照
	};
}