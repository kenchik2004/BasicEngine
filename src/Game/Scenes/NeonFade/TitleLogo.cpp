#include "TitleLogo.h"

namespace NeonFade {

	int TitleLogo::Init() {
		auto txt = AddComponent<Text>();
		txt->SetFontSize(190);
		txt->TextColor() = Color::DARK_RED;
		txt->SetText(u8"NEON FADE");
		logo_text = txt;
		anchor_type = LEFT_TOP;
		canvas_anchor_type = LEFT_TOP;
		transform->scale = Vector3(700, 200, 1);
		transform->position = { 120,-20,1 };
		return Super::Init();
	}
}
