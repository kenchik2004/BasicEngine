#include "TutorialUIText.h"

namespace NeonFade {
	int TutorialUIText::Init() {

		//サイズと位置を設定する
		transform->position = { 0,-400,0 };
		transform->scale = { 1000,300,1 };

		//テキストコンポーネントを追加する
		auto txt = AddComponent<Text>();
		//テキストを設定する
		txt->TextColor() = Color::YELLOW;
		txt->SetFontSize(30);
		txt->SetText(text_content);

		//文字送りモードに設定
		txt->SetAlignment(Text::ALIGNMENT::AUTO);
		//文字送り速度を50文字/秒に設定する
		txt->text_speed = 50.0f;
		Sleep();

		return Super::Init();
	}
}