//---------------------------------------------------------------------------
//! @file   ButtonObject.cpp
//! @brief  ButtonObjectの実装。UIボタンオブジェクトの処理を行う
//---------------------------------------------------------------------------
#include "Precompile.h"
#include "ButtonObject.h"
#include <System/Components/Button.h>
#include <System/Components/Text.h>

int ButtonObject::Init()
{
	transform->scale = { 100, 30, 0 };
	auto btn = AddComponent<Button>();
	btn->Color() = Color::WHITE;
	auto txt = AddComponent<Text>();
	txt->SetText("START!");
	txt->TextColor() = Color::BLACK;

	return Super::Init();
}

void ButtonObject::Update()
{
	Super::Update();
}
