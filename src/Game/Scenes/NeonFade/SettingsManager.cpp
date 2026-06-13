#include "SettingsManager.h"
#include "OptionCamera.h"
#include "OptionDeadZone.h"
#include "OptionVolume.h"
#include <algorithm>

namespace NeonFade {

	int SettingsManager::Init() {
		settings_opts = SceneManager::Object::Create<OptionManager>("SettingsOptionManager");
		settings_opts->UseBackGround() = true;
		settings_opts->BackGroundColor() = Color(0, 0, 0, 0.7f);
		auto sm = settings_opts.lock();
		if (sm) {
			sm->transform->position = { 0, 0, 0 };
			sm->transform->scale = { 0.0f, 1.0f, 1.0f };
		}

		auto opt_cam = SceneManager::Object::Create<OptionCamera>("OptionCamera");
		opt_cam->transform->position = { 0, 70, 1 };

		auto opt_deadzone = SceneManager::Object::Create<OptionDeadZone>("OptionDeadZone");
		opt_deadzone->transform->position = { 0, 0, 1 };

		auto opt_vol = SceneManager::Object::Create<OptionVolume>("OptionVolume");
		opt_vol->transform->position = { 0, -70, 1 };

		if (sm) {
			sm->AddOption(SafeStaticCast<OptionData>(opt_cam));
			sm->AddOption(SafeStaticCast<OptionData>(opt_deadzone));
			sm->AddOption(SafeStaticCast<OptionData>(opt_vol));
			sm->Sleep();
		}
		return Super::Init();
	}

	void SettingsManager::Update() {
		auto sm = settings_opts.lock();
		if (!sm) return;

		Vector3 scale_closed = { 0.0f, 1.0f, 1.0f };
		Vector3 scale_open = { 600.0f, 400.0f, 1.0f };

		if (current_state == State::FadingIn) {
			fade_timer += Time::DeltaTime();
			float t = std::clamp(fade_timer / fade_duration, 0.0f, 1.0f);

			float easeT = 1.0f - std::pow(1.0f - t, 3.0f);
			sm->transform->scale = Lerp(scale_closed, scale_open, easeT);

			if (t >= 1.0f) {
				current_state = State::Visible;
				sm->WakeUp();
			}
		}
		else if (current_state == State::FadingOut) {
			fade_timer += Time::DeltaTime();
			float t = std::clamp(fade_timer / fade_duration, 0.0f, 1.0f);

			float easeT = t * t * t;
			sm->transform->scale = Lerp(scale_open, scale_closed, easeT);

			if (t >= 1.0f) {
				current_state = State::Hidden;
			}
		}
		else if (current_state == State::Visible) {
			if (Input::GetKeyDown(KeyCode::Escape) || Input::GetPadButtonDown(0, PadButton::Button1)) {
				Close();
			}
		}
	}

	void SettingsManager::Open() {
		current_state = State::FadingIn;
		fade_timer = 0.0f;
		if (auto sm = settings_opts.lock()) {
			sm->transform->scale = { 0.0f, 1.0f, 1.0f };
			sm->WakeUp(); // keep input disabled during animation
		}
		// Disable main options
		if (auto mm = main_opts.lock()) {
			mm->Sleep();
		}
	}

	void SettingsManager::Close() {
		current_state = State::FadingOut;
		fade_timer = 0.0f;
		if (auto sm = settings_opts.lock()) {
			sm->Sleep(); // disable input immediately
		}
		// Re-enable main options
		if (auto mm = main_opts.lock()) {
			mm->WakeUp();
		}
	}

	void SettingsManager::Exit() {
		if (auto o = settings_opts.lock()) {
			SceneManager::Object::Destroy(o);
		}
		Super::Exit();
	}

}
