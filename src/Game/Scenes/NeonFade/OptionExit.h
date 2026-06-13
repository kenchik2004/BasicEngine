#pragma once
#include "OptionData.h"

namespace NeonFade {

	USING_PTR(OptionExit);

	class OptionExit : public OptionData {
	public:
		USING_SUPER(OptionExit);
		int Init() override;
	};
}
