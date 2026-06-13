#pragma once
#include <functional>
#include <vector>
#include <string>

namespace NeonFade {

	USING_PTR(OptionData);

	class OptionData : public UIObject {
	public:
		USING_SUPER(OptionData);
		std::function<void()> on_select;
		std::function<void()> on_deselect;
		std::function<void()> on_decide;
		std::function<void()> on_left;
		std::function<void()> on_right;
		std::function<void()> on_idle;

		std::vector<OptionDataWP> child_options;
		bool is_selected = false;

		int Init() override;
		void Update() override;

		void Select();
		void Deselect();
		void Decide();
		void Left();
		void Right();
		void SetText(const std::string& str);

	private:
		TextWP text_comp;
		Vector3 base_scale;
	};
}
