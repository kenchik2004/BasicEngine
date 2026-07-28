#include "OptionDeadZone.h"
#include <format>
#include <string>

namespace NeonFade {

	void OptionDeadZone::ApplyDeadZone() {
		Input::SetPadLeftDeadZone(0, dead_zone);
		Input::SetPadRightDeadZone(0, dead_zone);
		FileSystem::IniFileManager::SetFloat(u8"Option", u8"dead_zone", dead_zone, "data/config.ini");
	}

	int OptionDeadZone::Init() {
		int ret = Super::Init();
		anchor_type = CENTER;
		canvas_anchor_type = CENTER;
		{
			dead_zone = FileSystem::IniFileManager::Getfloat(u8"Option", u8"dead_zone", 0.0f, "data/config.ini");
		}
		SetText(u8"デッドゾーン：" + std::format("{:.1f}", dead_zone));

		//初期化も兼ねてiniファイルを上書きする
		ApplyDeadZone();

		on_select = [this]() {
			enable_edit = true;
			};
		on_deselect = [this]() {
			enable_edit = false;
			ApplyDeadZone();
			};
		return ret;
	}

	void OptionDeadZone::Update() {
		Super::Update();

		if (enable_edit) {
			if (Input::GetKey(KeyCode::Right) || Input::GetPadButtonRepeat(0, PadButton::Right))
				dead_zone = min(dead_zone + Time::DeltaTime(), 1.0f);
			if (Input::GetKey(KeyCode::Left) || Input::GetPadButtonRepeat(0, PadButton::Left))
				dead_zone = max(dead_zone - Time::DeltaTime(), 0.0f);
			SetText(u8"デッドゾーン：" + std::format("{:.1f}", dead_zone));
		}

	}

}
