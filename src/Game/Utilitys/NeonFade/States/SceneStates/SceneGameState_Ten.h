#pragma once
#include "Game/Utilitys/NeonFade/States/ISceneState.h"

namespace NeonFade {
	class SceneGame;
	//----------------------------------------
	// 起承転結の「転」
	//----------------------------------------
	class SceneGameState_Ten :
		public ISceneState
	{
	public:
		SceneGameState_Ten(SceneGame* owner_scene_);
		virtual void OnEnter(ISceneStateMachine* machine) override;
		virtual void OnExit(ISceneStateMachine* machine) override;
		virtual void Update(ISceneStateMachine* machine, float dt) override;
	private:
		SceneGame* owner_scene_game = nullptr;
		float exit_timer = 0;
		static constexpr float EXIT_TIME = 2.0f;
	};
}

