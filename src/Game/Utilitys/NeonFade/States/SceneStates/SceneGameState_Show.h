#pragma once
#include "Game/Utilitys/NeonFade/States/ISceneState.h"

namespace NeonFade {
	class SceneGame;
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
	};
}

