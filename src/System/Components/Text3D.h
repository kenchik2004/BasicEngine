//---------------------------------------------------------------------------
//! @file   Text3D.h
//! @brief  3Dワールド空間にテキストを描画するText3Dコンポーネント
//---------------------------------------------------------------------------
#pragma once
#include "Component.h"
//---------------------------------------------------------------------
//! @class Text3D
//! @brief 3Dワールド空間内にテキストをビルボード表示するコンポーネント
//---------------------------------------------------------------------
class Text3D :
	public Component
{
public:
	USING_SUPER(Text3D);
	//! @brief 初期化処理
	int Init() override;
	//! @brief 描画処理（後処理）
	void LateDraw() override;
	//! @brief 表示テキストを設定する
	void SetText(std::string_view text_);
	//! @brief フォントサイズを設定する
	void SetFontSize(int size_);
	//! @brief テキスト色を設定する
	void SetTextColor(const Color& color_) { text_color = color_; }
	//! @brief フォントサイズを取得する
	int GetFontSize() const { return font_size; }
	//! @brief テキスト色を取得する
	Color GetTextColor() const { return text_color; }
	//! @brief 表示テキストを取得する
	std::string_view GetText() const { return text; }
private:
	std::string text;            //!< 表示テキスト文字列
	int font_size = 12;          //!< フォントサイズ
	int font_handle = -1;        //!< フォントハンドル
	Color text_color = Color::WHITE; //!< テキストの描画色

};

