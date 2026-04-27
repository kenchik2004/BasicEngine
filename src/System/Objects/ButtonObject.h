//---------------------------------------------------------------------------
//! @file   ButtonObject.h
//! @brief  ボタンUIオブジェクト（クリックイベント付きUIコンポーネント）
//---------------------------------------------------------------------------
#pragma once

#define SET_FUNCTION(FUNC) [this]() { FUNC(); }
#include <System/Components/Button.h>
USING_PTR(Button);

//---------------------------------------------------------------------
//! @class ButtonObject
//! @brief ボタン機能を持つUIオブジェクトクラス
//---------------------------------------------------------------------
class ButtonObject : public UIObject
{
public:
	USING_SUPER(ButtonObject);
	//! @brief 初期化処理
	int Init() override;
	//! @brief 更新処理
	void Update() override;
};
