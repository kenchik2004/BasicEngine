#include "OptionCamera.h"

namespace NeonFade {

	std::string OptionCamera::CameraModeToString(CameraMode mode) {
		switch (mode) {
		case NORMAL: return u8"カメラ操作：通常";
		case INVERT_X: return u8"カメラ操作：X軸反転";
		case INVERT_Y: return u8"カメラ操作：Y軸反転";
		case INVERT_XY: return u8"カメラ操作：X/Y軸反転";
		default: return u8"カメラ操作：UNKNOWN";
		}
	}

	void OptionCamera::ApplyCameraOption(CameraMode mode) {
		FileSystem::IniFileManager::SetBool("Camera", "InvertX", (mode == INVERT_X || mode == INVERT_XY), u8"data/config.ini");
		FileSystem::IniFileManager::SetBool("Camera", "InvertY", (mode == INVERT_Y || mode == INVERT_XY), u8"data/config.ini");
	}

	int OptionCamera::Init() {
		int ret = Super::Init();
		anchor_type = CENTER;
		canvas_anchor_type = CENTER;
		SetText(u8"カメラ操作：通常");

		//初期化も兼ねてiniファイルを上書きする
		ApplyCameraOption(static_cast<CameraMode>(cam_mode));

		on_right = [this]() {
			cam_mode++;
			cam_mode %= CAMMODE_MAX;
			SetText(CameraModeToString(static_cast<CameraMode>(cam_mode)));
			ApplyCameraOption(static_cast<CameraMode>(cam_mode));
			};
		on_left = [this]() {
			cam_mode += CAMMODE_MAX - 1; // -1 mod CAMMODE_MAX
			cam_mode %= CAMMODE_MAX;
			SetText(CameraModeToString(static_cast<CameraMode>(cam_mode)));
			ApplyCameraOption(static_cast<CameraMode>(cam_mode));
			};
		return ret;
	}

}
