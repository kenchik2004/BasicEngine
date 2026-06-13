#pragma once
#include "OptionData.h"

namespace NeonFade {

	USING_PTR(OptionVolume);

	class OptionVolume : public OptionData {
	public:
		USING_SUPER(OptionVolume);
		int vol = 100;
		int Init() override;
	};
}
