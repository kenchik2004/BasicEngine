#pragma once
#include "OptionData.h"
#include "SettingsManager.h"

namespace NeonFade {

	USING_PTR(OptionSetting);

	class OptionSetting : public OptionData {
	public:
		USING_SUPER(OptionSetting);
		SettingsManagerWP settings_manager;
		int Init() override;
	};
}
