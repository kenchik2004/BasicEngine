#include "OptionCamera.h"

#include "Game/Scenes/NeonFade/SceneGame.h"

namespace NeonFade {

	std::string OptionCamera::CameraModeToString(CameraMode mode) {
		switch (mode) {
		case NORMAL: return u8"カメラ操作：通常";
		case INVERT_X: return u8"カメラ操作：左右反転";
		case INVERT_Y: return u8"カメラ操作：上下反転";
		case INVERT_XY: return u8"カメラ操作：上下左右反転";
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
		{
			u8 cam_invert_x = FileSystem::IniFileManager::GetBool("Camera", "InvertX", false, u8"data/config.ini");
			u8 cam_invert_y = FileSystem::IniFileManager::GetBool("Camera", "InvertY", false, u8"data/config.ini");
			cam_invert_y <<= 1; // Y軸反転はbit1に割り当てる
			cam_mode = cam_invert_x | cam_invert_y; // bit0: X軸反転, bit1: Y軸反転
		}
		SetText(CameraModeToString(static_cast<CameraMode>(cam_mode)));

		//初期化も兼ねてiniファイルを上書きする
		ApplyCameraOption(static_cast<CameraMode>(cam_mode));

		on_right = [this]() {
			cam_mode++;
			cam_mode %= CAMMODE_MAX;
			SetText(CameraModeToString(static_cast<CameraMode>(cam_mode)));
			ApplyCameraOption(static_cast<CameraMode>(cam_mode));
			auto scene = SceneManager::GetScene<SceneGame>();
			if (scene)
				scene->LoadCameraSettings();
			};
		on_left = [this]() {
			cam_mode += CAMMODE_MAX - 1; // -1 mod CAMMODE_MAX
			cam_mode %= CAMMODE_MAX;
			SetText(CameraModeToString(static_cast<CameraMode>(cam_mode)));
			ApplyCameraOption(static_cast<CameraMode>(cam_mode));
			auto scene = SceneManager::GetScene<SceneGame>();
			if (scene)
				scene->LoadCameraSettings();
			};
		return ret;
	}

}
