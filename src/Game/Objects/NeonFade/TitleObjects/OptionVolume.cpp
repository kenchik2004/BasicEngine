#include "OptionVolume.h"
#include <string>
#include "Game/Scenes/NeonFade/SceneTitle.h"
#include "Game/Scenes/NeonFade/SceneGame.h"

namespace NeonFade {

	int OptionVolume::Init() {
		int ret = Super::Init();
		anchor_type = CENTER;
		canvas_anchor_type = CENTER;

		{
			// 設定ファイルから音量を読み込む
			float bgm_volume = FileSystem::IniFileManager::Getfloat("Audio", "BGMVolume", 1.0f, "data/config.ini");
			float se_volume = FileSystem::IniFileManager::Getfloat("Audio", "SEVolume", 1.0f, "data/config.ini");
			// BGMとSEの音量は同じ値で管理する
			vol = static_cast<int>(bgm_volume * 100.0f);
		}
		SetText(u8"音量:" + std::to_string(vol) + "%");
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
		on_deselect = [this]() {
			ApplyVolume();
			};
		ApplyVolume();
		return ret;
	}

	void OptionVolume::ApplyVolume()
	{
		FileSystem::IniFileManager::SetFloat("Audio", "BGMVolume", static_cast<float>(vol) / 100.0f, "data/config.ini");
		FileSystem::IniFileManager::SetFloat("Audio", "SEVolume", static_cast<float>(vol) / 100.0f, "data/config.ini");
		auto scene_game = SceneManager::GetScene<SceneGame>();
		if (scene_game)
			scene_game->LoadVolumeSettings();
	}

}
