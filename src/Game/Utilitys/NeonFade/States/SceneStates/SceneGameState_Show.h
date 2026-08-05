//---------------------------------------------------------------------------
//! @file   SceneGameState_Show.h
//! @brief  ゲームの演出シーン状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/ISceneState.h"

namespace NeonFade {



	class SceneGame;
	class EnemyTeam;
	//----------------------------------------
	// 起承転結の「承」
	// ほかの名前が思いつかなかった。許せ。
	//----------------------------------------
	class SceneGameState_Show :
		public ISceneState
	{
	public:
		SceneGameState_Show(SceneGame* owner_scene_);
		virtual void OnEnter(ISceneStateMachine* machine) override;
		virtual void OnExit(ISceneStateMachine* machine) override;
		virtual void Update(ISceneStateMachine* machine, float dt) override;
	private:
		SceneGame* owner_scene_game = nullptr;
		std::vector<SafeUniquePtr<EnemyTeam>> enemy_teams;
		UIObjectWP timer_text = nullptr;

		float tutorial_timing_counter = 0.0f;
		static constexpr float TUTORIAL_TIMING_ABOUT_ENEMY = 10.0f;	//敵の説明をするタイミング
		static constexpr float TUTORIAL_TIMING_ABOUT_EXTRA = 20.0f;	//その他の説明をするタイミング
		void CreateTutorial(u32 index, bool disable_tutorial_on_complete = false);
	};
}

