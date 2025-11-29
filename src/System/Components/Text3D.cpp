#include "precompile.h"
#include "Text3D.h"

int Text3D::Init()
{
	font_handle = DxLib::CreateFontToHandle(nullptr, font_size, -1, DX_FONTTYPE_ANTIALIASING);
	return 0;
}

void Text3D::LateDraw()
{
	Vector3 draw_pos = owner.lock()->transform->position;
	draw_pos = cast(ConvWorldPosToScreenPos(cast(draw_pos)));
	// 画面外なら描画しない
	if (draw_pos.z < 0.0f)
		return;
	DrawStringFToHandle(draw_pos.x, draw_pos.y, text.c_str(), text_color, font_handle);

}

void Text3D::SetText(std::string_view text_)
{
	text = text_;
}

void Text3D::SetFontSize(int size_)
{
	font_size = size_;
	DeleteFontToHandle(font_handle);
	font_handle = DxLib::CreateFontToHandle(nullptr, font_size, -1, DX_FONTTYPE_ANTIALIASING);
}
