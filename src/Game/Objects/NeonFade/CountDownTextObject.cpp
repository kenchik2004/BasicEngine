#include "CountDownTextObject.h"
#include "Game/Scenes/NeonFade/SceneGame.h"

namespace NeonFade {
	int CountDownTextObject::Init()
	{
		transform->scale = { 600,170,1 };
		text_comp = AddComponent<Text>();
		text_comp->TextColor() = Color::YELLOW;
		text_comp->SetAlignment(Text::ALIGNMENT::RIGHT);
		text_comp->SetText("");
		canvas_anchor_type = UIObject::ANCHOR_TYPE::RIGHT_TOP;
		anchor_type = UIObject::ANCHOR_TYPE::RIGHT_TOP;
		text_comp->SetFont(u8"DSEG7 Modern");
		text_comp->SetFontSize(80);
		sub_text_comp = AddComponent<Text>();
		sub_text_comp->TextColor() = Color::YELLOW;
		sub_text_comp->SetAlignment(Text::ALIGNMENT::LEFT);
		sub_text_comp->SetText(u8"残り時間");
		sub_text_comp->SetFontSize(30);
		Sleep();
		return Super::Init();
	}
	void CountDownTextObject::Update()
	{
		auto scene_game = SafeDynamicCast<SceneGame>(GetScene());
		int min_ = static_cast<int>(max(0.0f, scene_game->GAME_TIMER_MAX - scene_game->GetGameTimer())) / 60;
		float sec_ = max(0.0f, scene_game->GAME_TIMER_MAX - scene_game->GetGameTimer()) - min_ * 60;
		std::string count_down_txt;
		count_down_txt += std::format("{:02d}:{:05.2f}", min_, sec_);

		text_comp->SetText(count_down_txt);
		if (min_ < 1 && sec_ < 10.0f) {
			text_comp->TextColor() = Color::RED;
			sub_text_comp->TextColor() = Color::RED;
			if (sec_ < 5.0f) {
				text_comp->TextColor().a = sinf(Time::GetTimeFromStart() * 5) * 0.5f + 1.0f;
				text_comp->SetFontSize(80 + static_cast<int>(10 * sinf(Time::GetTimeFromStart() * 5)));
			}
		}
		else {
			text_comp->TextColor() = Color::YELLOW;
			sub_text_comp->TextColor() = Color::YELLOW;
		}
	}
}