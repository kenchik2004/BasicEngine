#pragma once

USING_PTR(ImageRenderer);
namespace NeonFade {

	class CountDownTextObject :
		public UIObject
	{
	public:
		USING_SUPER(CountDownTextObject);
		int Init() override;
		void Update() override;

	private:
		TextWP text_comp; //!< テキストコンポーネント
		ImageRendererWP image_comp; //!< 画像コンポーネント
		SafeSharedPtr<Texture> bg_tex = nullptr; //!< 背景画像用のテクスチャ
	};

}
