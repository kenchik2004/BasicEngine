#pragma once
#include "DebugCameraScene.h"

namespace NeonFade {
	USING_PTR(SceneGame);
	//---------------------------------------------------------------------
	//! @class SceneTitle
	//! @brief タイトルシーンクラス
	//---------------------------------------------------------------------
	USING_PTR(TitleLogo);
	USING_PTR(OptionManager);
	USING_PTR(SettingsManager);

	class SceneTitle :
		//public DebugCameraScene
		public Scene
	{
	public:
		USING_SUPER(SceneTitle);
		void Load() override;
		int Init() override;
	private:

		TitleLogoWP logo;
		OptionManagerWP opt_manager;
		SettingsManagerWP settings_manager;

	};

}
