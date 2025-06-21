#pragma once
#pragma once
#include <System/Component.h>

struct CharToken {
	std::string utf8_char;     // UTF-8 文字（1〜4バイト）
	Color color;    // "red", "black", "default" など
};
USING_PTR(UIObject);
class Text : public Component
{
public:
	USING_SUPER(Text);
	int                  Init() override;
	void                  Update() override;
	void                  LateDraw() override;
	inline Color& TextColor() { return txt_color; }
	inline const std::string& MyText() const { return text; }
	void SetText(std::string new_text);
	inline Vector3& BoxSize() { return text_box_size; }
	inline Vector3& BoxPos() { return text_box_pos; }
	inline int& FontSize() { return font_size; }
	inline void           ResetDrawChar() { draw_char_num = 0; }
	enum ALIGNMENT
	{
		LEFT,
		MIDDLE,
		RIGHT,
		AUTO,
	};
	inline void SetAlignment(ALIGNMENT alignment_) { alignment = alignment_; }

private:
	void DrawAutoString(std::string str_, Vector3 draw_box_pos, Vector3 draw_box_size, float& draw_char_num, Color def_color, float speed);
	std::string   text = "TEXT";
	std::vector<CharToken> tokens;
	ALIGNMENT     alignment = MIDDLE;
	float         draw_char_num;
	Vector3       text_box_pos = { 0,0,0 };
	Vector3       text_box_size = { 0,0,0 };
	int           font_size = DEFAULT_FONT_SIZE;
	Color  txt_color = Color::BLACK;
};
