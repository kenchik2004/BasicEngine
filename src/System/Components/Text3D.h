#pragma once
#include "Component.h"
class Text3D :
	public Component
{
public:
	USING_SUPER(Text3D);
	int Init() override;
	void LateDraw() override;
	void SetText(std::string_view text_);
	void SetFontSize(int size_);
	void SetTextColor(const Color& color_) { text_color = color_; }
	int GetFontSize() const { return font_size; }
	Color GetTextColor() const { return text_color; }
	std::string_view GetText() const { return text; }
private:
	std::string text;
	int font_size = 12;
	int font_handle = -1;
	Color text_color = Color::WHITE;

};

