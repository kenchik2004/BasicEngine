#pragma once
#include "Game/Utilitys/NeonFade/States/ISceneState.h"

namespace NeonFade {
	//----------------------------------------
	// 起承転結の「結」
	// ほかの名前が思いつかなかった。許せ。
	//----------------------------------------
	class SceneGame;
	class EnemyTeam;
	class SceneGameState_Ketsu :
		public ISceneState
	{
	public:
		SceneGameState_Ketsu(SceneGame* owner_scene_);
		virtual void OnEnter(ISceneStateMachine* machine) override;
		virtual void OnExit(ISceneStateMachine* machine) override;
		virtual void Update(ISceneStateMachine* machine, float dt) override;
	private:
		SceneGame* owner_scene_game = nullptr;
		std::vector<SafeUniquePtr<EnemyTeam>> enemy_teams;
		TextWP timer_text = nullptr;
	};
}

