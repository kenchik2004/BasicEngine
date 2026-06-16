#pragma once
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
		TextWP sub_text_comp; //!< サブテキストコンポーネント
	};

}
