#pragma once
#include "OptionManager.h"

namespace NeonFade {

	USING_PTR(SettingsManager);

	class SettingsManager : public UIObject {
	public:
		USING_SUPER(SettingsManager);
		enum class State {
			Hidden,
			FadingIn,
			Visible,
			FadingOut
		};

		State current_state = State::Hidden;
		float fade_timer = 0.0f;
		const float fade_duration = 0.3f;

		bool auto_close = false;
		KeyCode close_key = KeyCode::Minus;
		PadButton close_pad_button = PadButton::Button1;

		OptionManagerWP settings_opts;
		OptionManagerWP main_opts;

		int Init() override;
		void Update() override;
		void Exit() override;

		void Open();
		void Close();
		bool IsOpened() const { return current_state == State::Visible || current_state == State::FadingIn; }
	};
}
