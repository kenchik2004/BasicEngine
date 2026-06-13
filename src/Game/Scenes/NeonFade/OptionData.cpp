#include "OptionData.h"

namespace NeonFade {

	int OptionData::Init() {
		auto txt = AddComponent<Text>();
		txt->SetFontSize(48);
		txt->TextColor() = Color::WHITE;
		txt->SetText(u8"OPTION");
		text_comp = txt;
		anchor_type = LEFT_TOP;
		canvas_anchor_type = LEFT_TOP;
		transform->scale = Vector3(200, 60, 1);

		base_scale = transform->scale;
		return Super::Init();
	}

	void OptionData::Update() {
		Super::Update();

		// アニメーション (選択時はスケールを少し大きくする)
		Vector3 target_scale = is_selected ? base_scale * 1.2f : base_scale;
		transform->scale = transform->scale + (target_scale - transform->scale) * 0.1f;

		if (is_selected) {
			if (on_select) on_select();
		}
		else {
			if (on_idle) on_idle();
		}
	}

	void OptionData::Select() {
		is_selected = true;
		if (auto t = text_comp.lock()) {
			t->TextColor() = Color::YELLOW;
		}
	}

	void OptionData::Deselect() {
		is_selected = false;
		if (auto t = text_comp.lock()) {
			t->TextColor() = Color::WHITE;
		}
		if (on_deselect) on_deselect();
	}

	void OptionData::Decide() {
		if (on_decide) on_decide();
	}

	void OptionData::Left() {
		if (on_left) on_left();
	}

	void OptionData::Right() {
		if (on_right) on_right();
	}

	void OptionData::SetText(const std::string& str) {
		if (auto t = text_comp.lock()) {
			t->SetText(str);
		}
	}
}
