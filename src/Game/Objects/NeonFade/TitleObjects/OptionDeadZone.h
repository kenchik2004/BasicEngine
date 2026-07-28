#pragma once
#include "OptionData.h"

namespace NeonFade {

	USING_PTR(OptionDeadZone);

	class OptionDeadZone : public OptionData {
	public:
		USING_SUPER(OptionDeadZone);
		float dead_zone = 0.0f;
		bool enable_edit = false;

		void ApplyDeadZone();
		int Init() override;
		void Update() override;
	};
}
