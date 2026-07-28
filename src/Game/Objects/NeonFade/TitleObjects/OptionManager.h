#pragma once
#include <vector>
#include "OptionData.h"

namespace NeonFade {

	USING_PTR(OptionManager);

	class OptionManager : public UIObject {
	public:
		USING_SUPER(OptionManager);

		int Init() override;
		void Update() override;
		void OnSleep() override;
		void OnWakeUp() override;

		void AddOption(OptionDataWP option);
		const std::vector<OptionDataWP>& GetOptions() const;

	private:
		std::vector<OptionDataWP> options;
		int cursor_index = 0;

		void MoveCursor(int dir);
	};
}
