//---------------------------------------------------------------------------
//! @file   TextObject.h
//! @brief  テキストを表示するUIオブジェクト
//---------------------------------------------------------------------------
#pragma once
#include "System/Components/Text.h"
USING_PTR(Text);

//---------------------------------------------------------------------
//! @class TextObject
//! @brief テキストコンポーネントを持つUIオブジェクトクラス
//---------------------------------------------------------------------
class TextObject :
	public UIObject
{
public:
	USING_SUPER(TextObject);
	//! @brief 初期化処理
	int Init() override;
	TextWP my_text;	//!< テキストコンポーネントへの弱参照

};
