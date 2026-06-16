#include "OptionManager.h"

namespace NeonFade {

	int OptionManager::Init() {
		return Super::Init();
	}

	void OptionManager::AddOption(OptionDataWP option) {
		options.push_back(option);
		if (options.size() == 1) { // 最初の追加時に選択状態にする
			cursor_index = 0;
			if (auto o = options[0].lock()) o->Select();
		}
	}

	const std::vector<OptionDataWP>& OptionManager::GetOptions() const {
		return options;
	}

	void OptionManager::Update() {
		if (options.empty()) return;

		if (Input::GetKeyDown(KeyCode::Up) || Input::GetPadButtonDown(0, PadButton::Up)) {
			MoveCursor(-1);
		}
		if (Input::GetKeyDown(KeyCode::Down) || Input::GetPadButtonDown(0, PadButton::Down)) {
			MoveCursor(1);
		}
		if (Input::GetKeyDown(KeyCode::Left) || Input::GetPadButtonDown(0, PadButton::Left)) {
			if (auto o = options[cursor_index].lock()) {
				o->Left();
			}
		}
		if (Input::GetKeyDown(KeyCode::Right) || Input::GetPadButtonDown(0, PadButton::Right)) {
			if (auto o = options[cursor_index].lock()) {
				o->Right();
			}
		}
		if (Input::GetKeyDown(KeyCode::Return) || Input::GetKeyDown(KeyCode::Space) || Input::GetPadButtonDown(0, PadButton::Button2)) {
			if (auto o = options[cursor_index].lock()) {
				o->Decide();
			}
		}
	}

	void OptionManager::OnSleep() {
		options[cursor_index].lock()->Deselect();
		for (auto& opt : options) {
			if (auto o = opt.lock()) {
				o->Sleep();
			}
		}
	}

	void OptionManager::OnWakeUp() {
		options[cursor_index].lock()->Select();
		for (auto& opt : options) {
			if (auto o = opt.lock()) {
				o->WakeUp();
			}
		}
	}

	void OptionManager::MoveCursor(int dir) {
		if (auto current = options[cursor_index].lock()) {
			current->Deselect();
		}

		cursor_index += dir;
		if (cursor_index < 0) cursor_index = (int)options.size() - 1;
		if (cursor_index >= (int)options.size()) cursor_index = 0;

		if (auto next = options[cursor_index].lock()) {
			next->Select();
		}
	}
}
