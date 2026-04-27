//---------------------------------------------------------------------------
//! @file   Text.h
//! @brief  UIテキストの描画を管理するTextコンポーネント
//---------------------------------------------------------------------------
#pragma once
#pragma once

//---------------------------------------------------------------------
//! @struct CharToken
//! @brief  カラータグ付きのUTF-8文字トークン
//---------------------------------------------------------------------
struct CharToken {
	std::string utf8_char;     // UTF-8 文字（1〜4バイト）
	Color color;    // "red", "black", "default" など
};
USING_PTR(UIObject);
//---------------------------------------------------------------------
//! @class Text
//! @brief UI上にテキストを描画するコンポーネント
//---------------------------------------------------------------------
class Text : public Component
{
public:
	USING_SUPER(Text);
	//! @brief 初期化処理
	int                  Init() override;
	//! @brief 更新処理
	void                  Update() override;
	//! @brief 描画処理（後処理）
	void                  LateDraw() override;
	//! @brief テキスト色への参照を返す
	inline Color& TextColor() { return txt_color; }
	//! @brief 表示中のテキスト文字列を取得する
	inline const std::string& MyText() const { return text; }
	//! @brief 表示テキストを設定する
	void SetText(std::string new_text);
	//! @brief テキストボックスサイズへの参照を返す
	inline Vector3& BoxSize() { return text_box_size; }
	//! @brief テキストボックス位置への参照を返す
	inline Vector3& BoxPos() { return text_box_pos; }
	//! @brief フォントサイズを設定する
	inline int SetFontSize(u32 font_size_ = DEFAULT_FONT_SIZE);
	//! @brief フォント名を設定する
	inline void  SetFont(std::string_view font_name) { font = font_name; }
	//! @brief 描画済み文字数をリセットする
	inline void           ResetDrawChar() { draw_char_num = 0; }
	//! @brief テキスト配置方式の列挙型
	enum ALIGNMENT
	{
		LEFT,   //!< 左揃え
		MIDDLE, //!< 中央揃え
		RIGHT,  //!< 右揃え
		AUTO,   //!< 自動
	};
	//! @brief テキスト配置方式を設定する
	inline void SetAlignment(ALIGNMENT alignment_) { alignment = alignment_; }

	float text_speed = 1.0f; //!< テキストの表示速度（文字/フレーム）
private:
	//! @brief テキストボックス内に自動折り返しで描画する
	void DrawAutoString(std::string str_, Vector3 draw_box_pos, Vector3 draw_box_size, float& draw_char_num, Color def_color, float speed);
	std::string   text = "TEXT";           //!< 表示テキスト文字列
	std::vector<CharToken> tokens;         //!< カラータグ解析済みトークンリスト
	std::string font = "";                 //!< 使用フォント名
	ALIGNMENT     alignment = MIDDLE;      //!< テキスト配置方式
	float         draw_char_num;           //!< 現在描画済みの文字数
	Vector3       text_box_pos = { 0,0,0 }; //!< テキストボックスの位置
	Vector3       text_box_size = { 0,0,0 }; //!< テキストボックスのサイズ
	int           font_size = 1;           //!< フォントサイズ
	int font_handle = -1;                  //!< フォントハンドル
	Color  txt_color = Color::BLACK;       //!< テキストの描画色
};
