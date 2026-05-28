#pragma once

namespace NeonFade {
	USING_PTR(SceneGame);
	//---------------------------------------------------------------------
	//! @class SceneTitle
	//! @brief タイトルシーンクラス
	//---------------------------------------------------------------------
	class SceneTitle :public Scene
	{
	public:
		USING_SUPER(SceneTitle);
		void Load() override;
		int Init() override;
		void Update() override;
		void Exit() override;

	};

}
