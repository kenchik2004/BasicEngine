#pragma once
#include "OptionData.h"
#include <string>

namespace NeonFade {

	USING_PTR(OptionCamera);

	class OptionCamera : public OptionData {
	public:
		USING_SUPER(OptionCamera);
		u8 cam_mode = 0;
		enum CameraMode {
			NORMAL = 0,
			INVERT_X = 1,
			INVERT_Y = 2,
			INVERT_XY = 3,
			CAMMODE_MAX = 4
		};
		static std::string CameraModeToString(CameraMode mode);
		static void ApplyCameraOption(CameraMode mode);
		int Init() override;
	};
}
