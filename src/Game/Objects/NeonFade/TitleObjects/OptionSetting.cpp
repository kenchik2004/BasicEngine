#include "OptionSetting.h"

namespace NeonFade {

	int OptionSetting::Init() {
		int ret = Super::Init();
		SetText(u8"SETTINGS");
		on_decide = [this]() {
			if (auto sm = settings_manager.lock()) {
				sm->Open();
			}
			};
		return ret;
	}

}
