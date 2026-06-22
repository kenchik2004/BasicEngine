#pragma once
#include "OptionData.h"

namespace NeonFade {

	USING_PTR(OptionStart);

	class OptionStart : public OptionData {
	public:
		USING_SUPER(OptionStart);
		int Init() override;
		void Update() override;
		void Exit() override;
	private:
		float enable_timer = 0.0f;
		static constexpr float enable_delay = 0.5f; // 選択可能になるまでの時間（秒）
		bool disable = true;
	};
}
