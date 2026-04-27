//---------------------------------------------------------------------------
//! @file   TextObject.cpp
//! @brief  TextObjectの実装。テキスト表示オブジェクトの処理を行う
//---------------------------------------------------------------------------
#include "TextObject.h"

int TextObject::Init()
{

	my_text = AddComponent<Text>();
	my_text->SetText("START!");
	my_text->TextColor() = Color::BLACK;
	return Super::Init();
}
