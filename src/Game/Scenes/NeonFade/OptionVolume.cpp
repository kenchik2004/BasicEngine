#include "OptionVolume.h"
#include <string>

namespace NeonFade {

	int OptionVolume::Init() {
		int ret = Super::Init();
		anchor_type = CENTER;
		canvas_anchor_type = CENTER;
		SetText(u8"音量: 100%");
		on_left = [this]() {
			vol -= 10;
			if (vol < 0) vol = 0;
			SetText(u8"音量: " + std::to_string(vol) + "%");
			};
		on_right = [this]() {
			vol += 10;
			if (vol > 100) vol = 100;
			SetText(u8"音量: " + std::to_string(vol) + "%");
			};
		return ret;
	}

}
