#pragma once

USING_PTR(Text);
USING_PTR(TutorialUIText);
namespace NeonFade {
	//! @brief チュートリアル用のUIテキストオブジェクト
	//! @brief 文字送りで表示されるテキストを管理する
	class TutorialUIText :public UIObject {

	public:

		//! @brief コンストラクタ
		//! @param text 表示するテキストの内容
		TutorialUIText(const std::string& text)
			:UIObject()
		{
			text_content = text;
		}

		//! @brief 初期化処理
		int Init() override;

	private:
		TextWP my_text;		//!< Textオブジェクトへの弱参照ポインタ
		std::string text_content = "";	//<! 表示するテキストの内容

	};
}

